#include "mobidict.h"
#include "quickdict.h"
#ifdef ENABLE_OPENCC
#include <opencc/opencc.h>
#endif
#ifdef ENABLE_HUNSPELL
#include <hunspell/hunspell.hxx>
#endif
#ifdef ENABLE_UNAC
#include <unac/unac.h>
#endif

MobiDict::MobiDict(QObject *parent)
    : LocalDict(parent)
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

void MobiDict::setSerialNumber(const QString &serialNumber)
{
    if (serialNumber == m_serialNumber)
        return;
    m_serialNumber = serialNumber;
    emit serialNumberChanged(m_serialNumber);
}

void MobiDict::onQuery(const QString &text)
{
    QString trimmed = text.trimmed();
    QStringList textList;
#ifdef ENABLE_HUNSPELL
    std::vector<std::string> l = QuickDict::instance()->hunspell()->stem(trimmed.toStdString());
    if (!l.empty()) {
        for (const auto &s : l)
            textList.append(QString::fromStdString(s));
    } else {
        textList << trimmed;
    }
#else
    textList << trimmed.toLower();
#endif

    for (QString _text : qAsConst(textList)) {
#ifdef ENABLE_OPENCC
        _text = QString::fromStdString(QuickDict::instance()->openccConverter()->Convert(_text.toStdString()));
#endif
#ifdef ENABLE_UNAC
        std::string utf8Text = _text.toStdString();
        char *unaccented = nullptr;
        size_t len;
        if (unac_string("UTF8", utf8Text.c_str(), utf8Text.size(), &unaccented, &len) != -1) {
            _text = QString::fromUtf8(unaccented, len);
            free(unaccented);
        }
#endif
        auto node = m_dictIndex->findEntry(_text);
        if (node) {
            qCDebug(qdDict) << "Dict:" << name() << "query:" << _text << "count:" << node->_value.size();
            for (const MobiEntry &entry : node->_value) {
                QString definition = QString::fromUtf8(reinterpret_cast<const char *>(m_rawMarkup->flow->data
                                                                                      + entry.first),
                                                       entry.second);

                QJsonObject result{{"engine", name()}, {"text", _text}, {"result", definition}, {"type", "lookup"}};
                emit queryResult(result);
            }
        } else {
            qCDebug(qdDict) << "Dict:" << name() << "query: No entry for" << _text;
        }
    }
}

bool MobiDict::loadDict()
{
    MOBIData *mobi_data = mobi_init();
    if (nullptr == mobi_data) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to call mobi_init";
        return false;
    }

    m_dictFile = fopen(m_dictFileName.toStdString().c_str(), "rb");
    if (nullptr == m_dictFile) {
        mobi_free(mobi_data);
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_dictFileName;
        return false;
    }

    MOBI_RET mobi_ret = mobi_load_file(mobi_data, m_dictFile);
    fclose(m_dictFile);
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

bool MobiDict::buildIndex()
{
    qCDebug(qdDict) << "Dict:" << name() << "status: Building indexes...";

    const size_t count = m_rawMarkup->orth->total_entries_count;

    std::vector<std::pair<MobiKey, MobiEntry>> entries;
    bool needSort = !sorted();
#if defined(ENABLE_OPENCC) || defined(ENABLE_UNAC)
    needSort = true;
#endif
    if (needSort)
        entries.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        const MOBIIndexEntry *orth_entry = &m_rawMarkup->orth->entries[i];
        QString text;
#ifdef ENABLE_OPENCC
        text = QString::fromStdString(QuickDict::instance()->openccConverter()->Convert(orth_entry->label));
#else
        text = QString::fromUtf8(orth_entry->label);
#endif
#ifdef ENABLE_UNAC
        std::string utf8Text = text.toStdString();
        char *unaccented = nullptr;
        size_t len;
        if (unac_string("UTF8", utf8Text.c_str(), utf8Text.size(), &unaccented, &len) != -1) {
            text = QString::fromUtf8(unaccented, len);
            free(unaccented);
        }
#endif
        text = text.toLower();
        MobiEntry entry;
        entry.first = mobi_get_orth_entry_start_offset(orth_entry);
        entry.second = mobi_get_orth_entry_text_length(orth_entry);
        if (needSort) {
            entries.emplace_back(text, entry);
        } else {
            if (!m_dictIndex->addEntry(text, entry)) {
                qCWarning(qdDict) << "Dict:" << name() << "error: Failed to build indexes";
                return false;
            }
        }
    }

    if (needSort) {
        std::sort(entries.begin(), entries.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.first < rhs.first;
        });
        for (const auto &entry : entries)
            m_dictIndex->addEntry(entry.first, entry.second);
        entries.clear();
    }

    m_indexFile = fopen(m_indexFileName.toStdString().c_str(), "wb+");
    if (nullptr == m_indexFile) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_indexFileName;
        return false;
    }
    qCDebug(qdDict) << "Dict:" << name() << "status: Saving indexes...";
    m_dictIndex->serialize(m_indexFile);
    fclose(m_indexFile);

    return true;
}

bool MobiDict::loadIndex()
{
    qCDebug(qdDict) << "Dict:" << name() << "status: Loading indexes...";

    m_indexFile = fopen(m_indexFileName.toStdString().c_str(), "rb");
    if (nullptr == m_indexFile) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_indexFileName;
        return false;
    }
    m_dictIndex->deserialize(m_indexFile);
    fclose(m_indexFile);

    return true;
}

bool MobiDict::unloadIndex()
{
    m_dictIndex->clear();
    return true;
}
