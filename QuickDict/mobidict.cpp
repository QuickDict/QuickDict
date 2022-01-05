#include "mobidict.h"
#include "quickdict.h"
#include <QFileInfo>

MobiDict::MobiDict(QObject *parent)
    : DictService(parent)
{
    m_dictIndex = new MobiIndex;

    connect(this, &MobiDict::query, this, &MobiDict::onQuery);
}

MobiDict::~MobiDict()
{
    if (loaded()) {
        unloadDict();
        unloadIndex();
    }
    delete m_dictIndex;
}

void MobiDict::setSource(const QString &source)
{
    if (source == m_dictFileName)
        return;

    m_dictFileName = source;
    m_indexFileName = m_dictFileName + ".index"; // TODO: save index data to cache dir

    if (loaded()) {
        unloadDict();
        unloadIndex();
        setLoaded(false);
    }
    if (enabled() && !m_dictFileName.isEmpty()) {
        if (loadDict() && loadOrBuildIndex())
            setLoaded(true);
    }
    emit sourceChanged(m_dictFileName);
}

void MobiDict::setSerialNumber(const QString &serialNumber)
{
    if (serialNumber == m_serialNumber)
        return;
    m_serialNumber = serialNumber;
    emit serialNumberChanged(m_serialNumber);
}

void MobiDict::setSorted(bool sorted)
{
    if (m_sorted == sorted)
        return;
    m_sorted = sorted;
    emit sortedChanged(m_sorted);
}

void MobiDict::setLoaded(bool loaded)
{
    if (m_loaded == loaded)
        return;
    m_loaded = loaded;
    emit loadedChanged(m_loaded);
}

bool MobiDict::doSetEnabled(bool enabled)
{
    if (enabled && !m_dictFileName.isEmpty()) {
        if (loadDict() && loadOrBuildIndex()) {
            setLoaded(true);
            return true;
        } else {
            return false;
        }
    } else if (!enabled && loaded()) {
        unloadDict();
        unloadIndex();
        setLoaded(false);
    }
    return true;
}

void MobiDict::onQuery(const QString &text)
{
    auto node = m_dictIndex->findEntry(text);
    if (node) {
        qCDebug(qdDict) << "Dict:" << name() << "query:" << text << "count:" << node->_value.size();
        for (const MobiEntry &entry : node->_value) {
            QString definition = QString::fromUtf8(reinterpret_cast<const char *>(m_rawMarkup->flow->data + entry.first),
                                                   entry.second);

            QJsonObject result{{"engine", name()}, {"text", text}, {"result", definition}, {"type", "lookup"}};
            emit queryResult(result);
        }
    } else {
        qCDebug(qdDict) << "Dict:" << name() << "query: No entry for" << text;
    }
}

bool MobiDict::loadDict()
{
    MOBIData *mobi_data = mobi_init();
    if (nullptr == mobi_data) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to call mobi_init";
        return false;
    }

    FILE *dict_fp = fopen(m_dictFileName.toStdString().c_str(), "rb");
    if (nullptr == dict_fp) {
        mobi_free(mobi_data);
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_dictFileName;
        return false;
    }

    MOBI_RET mobi_ret = mobi_load_file(mobi_data, dict_fp);
    fclose(dict_fp);
    if (mobi_ret != MOBI_SUCCESS) {
        mobi_free(mobi_data);
        qCWarning(qdDict) << "Dict:" << name() << "error:" << libmobi_msg(mobi_ret);
        return false;
    }

    if (!serialNumber().isEmpty()) {
        mobi_ret = mobi_drm_setkey_serial(mobi_data, serialNumber().toStdString().c_str());
        if (mobi_ret != MOBI_SUCCESS) {
            qCWarning(qdDict) << "Dict:" << name() << "error:" << libmobi_msg(mobi_ret);
            return false;
        }
    }

    m_rawMarkup = mobi_init_rawml(mobi_data);
    if (nullptr == m_rawMarkup) {
        mobi_free(mobi_data);
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to call mobi_init_rawml";
        return false;
    }

    mobi_ret = mobi_parse_rawml_opt(m_rawMarkup,
                                    mobi_data,
                                    false, /* parse toc */
                                    true,  /* parse dic */
                                    false /* reconstruct */);
    if (mobi_ret != MOBI_SUCCESS) {
        mobi_free(mobi_data);
        mobi_free_rawml(m_rawMarkup);
        qCWarning(qdDict) << "Dict:" << name() << "error:" << libmobi_msg(mobi_ret);
        return false;
    }

    qCDebug(qdDict) << "Dict:" << name() << "entries:" << m_rawMarkup->orth->total_entries_count;

    mobi_free(mobi_data);

    return true;
}

bool MobiDict::unloadDict()
{
    mobi_free_rawml(m_rawMarkup);
    return true;
}

bool MobiDict::loadOrBuildIndex()
{
    if (needBuildIndex())
        return buildIndex();
    else
        return loadIndex();
}

bool MobiDict::needBuildIndex()
{
    QFileInfo dictFileInfo(m_dictFileName);
    QFileInfo indexFileInfo(m_indexFileName);
    if (!dictFileInfo.exists()
        || (dictFileInfo.exists() && indexFileInfo.exists()
            && dictFileInfo.lastModified() <= indexFileInfo.lastModified()))
        return false;
    return true;
}

bool MobiDict::buildIndex()
{
    qCDebug(qdDict) << "Dict:" << name() << "status: Building indexes...";

    const size_t count = m_rawMarkup->orth->total_entries_count;

    std::vector<std::pair<QString, MobiEntry>> entries;
    if (!sorted())
        entries.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        const MOBIIndexEntry *orth_entry = &m_rawMarkup->orth->entries[i];
        QString text = QString::fromUtf8(orth_entry->label);
        MobiEntry entry;
        entry.first = mobi_get_orth_entry_start_offset(orth_entry);
        entry.second = mobi_get_orth_entry_text_length(orth_entry);
        if (!sorted()) {
            entries.emplace_back(text, entry);
        } else {
            if (!m_dictIndex->addEntry(text, entry)) {
                qCWarning(qdDict) << "Dict:" << name() << "error: Failed to build indexes";
                return false;
            }
        }
    }

    if (!sorted()) {
        std::sort(entries.begin(), entries.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.first < rhs.first;
        });
        for (const auto &entry : entries)
            m_dictIndex->addEntry(entry.first, entry.second);
        entries.clear();
    }

    FILE *index_fp = fopen(m_indexFileName.toStdString().c_str(), "wb+");
    if (index_fp) {
        qCDebug(qdDict) << "Dict:" << name() << "status: Saving indexes...";
        m_dictIndex->serialize(index_fp);
        fclose(index_fp);
    } else {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_indexFileName;
    }

    return true;
}

bool MobiDict::loadIndex()
{
    qCDebug(qdDict) << "Dict:" << name() << "status: Loading indexes...";

    FILE *index_fp = fopen(m_indexFileName.toStdString().c_str(), "rb");
    if (nullptr == index_fp) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_indexFileName;
        return false;
    }
    m_dictIndex->deserialize(index_fp);
    fclose(index_fp);

    return true;
}

bool MobiDict::unloadIndex()
{
    m_dictIndex->clear();
    return true;
}
