#include "mdxdict.h"
#include "quickdict.h"
#include "utils.h"

#ifdef ENABLE_OPENCC
#include <opencc/opencc.h>
#endif
#ifdef ENABLE_HUNSPELL
#include <hunspell/hunspell.hxx>
#endif
#ifdef ENABLE_UNAC
#include <unac/unac.h>
#endif

#include <QDir>
#include <QFileInfo>

MdxDict::MdxDict(QObject *parent)
    : LocalDict(parent)
{
    m_dictIndex = new MdxIndex;

    connect(this, &MdxDict::query, this, &MdxDict::onQuery);
}

MdxDict::~MdxDict()
{
    if (loaded()) {
        unloadDict();
        unloadIndex();
    }
    delete m_dictIndex;
}

void MdxDict::onQuery(const QString &text)
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

    for (QString text_ : qAsConst(textList)) {
#ifdef ENABLE_OPENCC
        text_ = QString::fromStdString(QuickDict::instance()->openccConverter()->Convert(text_.toStdString()));
#endif
#ifdef ENABLE_UNAC
        std::string utf8Text = text_.toStdString();
        char *unaccented = nullptr;
        size_t len;
        if (unac_string("UTF8", utf8Text.c_str(), utf8Text.size(), &unaccented, &len) != -1) {
            text_ = QString::fromUtf8(unaccented, len);
            free(unaccented);
        }
#endif
        auto node = m_dictIndex->findEntry(text_);
        if (!node) {
            qCDebug(qdDict) << "Dict:" << name() << "query: No entry for" << text_;
            return;
        }
        qCDebug(qdDict) << "Dict:" << name() << "query:" << text_ << "count:" << node->_value.size();
        for (const MdxEntry &entry : node->_value) {
            uint64_t block = std::get<0>(entry);
            uint64_t relative_offset = std::get<1>(entry);
            uint64_t length = std::get<2>(entry);
            unsigned char *block_compressed = (unsigned char *) malloc(m_mdxData->record.compressed_block_sizes[block]);
            if (!block_compressed) {
                qCWarning(qdDict) << "Dict:" << name() << "error: Failed to allocate memory";
                return;
            }
            if (fseek(m_dictFile, m_mdxData->record.record_block_offsets[block], SEEK_SET) == -1) {
                qCWarning(qdDict) << "Dict:" << name() << "error: Failed to seek file";
                free(block_compressed);
                return;
            }
            if (fread(block_compressed, 1, m_mdxData->record.compressed_block_sizes[block], m_dictFile)
                != m_mdxData->record.compressed_block_sizes[block]) {
                qCWarning(qdDict) << "Dict:" << name() << "error: Failed to read file";
                free(block_compressed);
                return;
            }
            unsigned char *block_uncompressed = (unsigned char *) malloc(
                m_mdxData->record.uncompressed_block_sizes[block]);
            if (!block_uncompressed) {
                qCWarning(qdDict) << "Dict:" << name() << "error: Failed to allocate memory";
                free(block_compressed);
                return;
            }
            MDX_RET ret = mdx_uncompress(block_compressed,
                                         m_mdxData->record.compressed_block_sizes[block],
                                         &block_uncompressed,
                                         &m_mdxData->record.uncompressed_block_sizes[block]);
            if (ret != MDX_NO_ERROR) {
                qCWarning(qdDict) << "Dict:" << name() << "error: Failed to uncompress data";
                free(block_compressed);
                free(block_uncompressed);
                return;
            }

            QString definition = QString::fromUtf8(reinterpret_cast<const char *>(block_uncompressed + relative_offset),
                                                   length);
            free(block_compressed);
            free(block_uncompressed);
            if (!m_styleSheet.isEmpty())
                definition = QString("<style>%1</style>%2").arg(m_styleSheet, definition);
            QJsonObject result{{"engine", name()}, {"text", text_}, {"result", definition}, {"type", "lookup"}};
            emit queryResult(result);
        }
    }
}

bool MdxDict::loadDict()
{
    m_mdxData = new mdx_data;
    QFileInfo dictFileInfo(m_dictFileName);
    QFile styleFile(dictFileInfo.dir().filePath(dictFileInfo.completeBaseName() + ".css"));
    if (styleFile.exists()) {
        styleFile.open(QIODevice::ReadOnly);
        m_styleSheet = styleFile.readAll();
        styleFile.close();
    }

    m_dictFile = fopen_unicode(m_dictFileName.toStdString().c_str(), "rb");
    if (nullptr == m_dictFile) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_dictFileName;
        delete m_mdxData;
        return false;
    }

    MDX_RET ret;
    ret = mdx_init(m_dictFile, m_mdxData);
    if (ret != MDX_NO_ERROR) {
        qCWarning(qdDict) << "Dict:" << name() << "error:" << mdx_error_string(ret);
        delete m_mdxData;
        return false;
    }

    ret = mdx_parse_record_indexes(m_dictFile, m_mdxData);
    if (ret != MDX_NO_ERROR) {
        qCWarning(qdDict) << "Dict:" << name() << "error:" << mdx_error_string(ret);
        delete m_mdxData;
        return false;
    }

    qCDebug(qdDict) << "Dict:" << name() << "entries:" << m_mdxData->record.num_total_entries;

    return true;
}

bool MdxDict::unloadDict()
{
    mdx_free(m_mdxData);
    m_mdxData = nullptr;
    return true;
}

bool MdxDict::buildIndex()
{
    qCDebug(qdDict) << "Dict:" << name() << "status: Building indexes...";

    MDX_RET ret;
    ret = mdx_parse_keyword_indexes(m_dictFile, m_mdxData);
    if (ret != MDX_NO_ERROR) {
        qCWarning(qdDict) << "Dict:" << name() << "error:" << mdx_error_string(ret);
        return false;
    }

    std::vector<std::pair<MdxKey, MdxEntry>> entries;
    bool needSort = !sorted();
#if defined(ENABLE_OPENCC) || defined(ENABLE_UNAC)
    needSort = true;
#endif
    if (needSort)
        entries.reserve(m_mdxData->record.num_total_entries);

    size_t accumulated_length = 0;
    size_t entry_count = 0;
    for (size_t block = 0; block < m_mdxData->record.num_blocks; ++block) {
        // NOTE: need to check entry_count
        while (m_mdxData->keyword.record_offsets[entry_count]
                   < accumulated_length + m_mdxData->record.uncompressed_block_sizes[block]
               && entry_count < m_mdxData->record.num_total_entries) {
            uint64_t relative_offset = m_mdxData->keyword.record_offsets[entry_count] - accumulated_length;
            uint64_t length;
            if (entry_count < m_mdxData->record.num_total_entries - 1) {
                length = m_mdxData->keyword.record_offsets[entry_count + 1]
                         - m_mdxData->keyword.record_offsets[entry_count] - 8 - 1 /* null terminator */;
            } else {
                length = m_mdxData->record.uncompressed_block_sizes[block] - (relative_offset + 8 + 1);
            }
            // FIXME: encoding conversion
            char *keyword = (char *) m_mdxData->keyword.keywords[entry_count];
            QString text;
#ifdef ENABLE_OPENCC
            text = QString::fromStdString(QuickDict::instance()->openccConverter()->Convert(keyword));
#else
            text = QString::fromUtf8(keyword);
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
            MdxEntry entry{block, relative_offset, length};
            if (needSort) {
                entries.emplace_back(text, entry);
            } else {
                if (!m_dictIndex->addEntry(text, entry)) {
                    qCWarning(qdDict) << "Dict:" << name() << "error: Failed to build indexes";
                    return false;
                }
            }
            ++entry_count;
        }
        accumulated_length += m_mdxData->record.uncompressed_block_sizes[block];
    }

    if (needSort) {
        std::sort(entries.begin(), entries.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.first < rhs.first;
        });
        for (const auto &entry : entries)
            m_dictIndex->addEntry(entry.first, entry.second);
        entries.clear();
    }

    m_indexFile = fopen_unicode(m_indexFileName.toStdString().c_str(), "wb+");
    if (nullptr == m_indexFile) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_indexFileName;
        return false;
    }
    qCDebug(qdDict) << "Dict:" << name() << "status: Saving indexes...";
    m_dictIndex->serialize(m_indexFile);
    qCDebug(qdDict) << "Dict:" << name() << "status: Saving indexes finished...";
    fclose(m_indexFile);

    return true;
}

bool MdxDict::loadIndex()
{
    qCDebug(qdDict) << "Dict:" << name() << "status: Loading indexes...";

    m_indexFile = fopen_unicode(m_indexFileName.toStdString().c_str(), "rb");
    if (nullptr == m_indexFile) {
        qCWarning(qdDict) << "Dict:" << name() << "error: Failed to open file" << m_indexFileName;
        return false;
    }
    m_dictIndex->deserialize(m_indexFile);
    fclose(m_indexFile);

    return true;
}

bool MdxDict::unloadIndex()
{
    m_dictIndex->clear();
    return true;
}
