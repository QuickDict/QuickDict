#ifndef MDXDICT_H
#define MDXDICT_H

#include "dictindex.h"
#include "localdict.h"
#include <libmdx/mdx.h>

using MdxKey = QString;
using MdxEntry = std::tuple<uint64_t, uint64_t, uint64_t>;
using MdxIndex = DictIndex<MdxKey, MdxEntry>;

class MdxDict : public LocalDict
{
    Q_OBJECT

public:
    explicit MdxDict(QObject *parent = nullptr);
    virtual ~MdxDict();

protected:
    void onQuery(const QString &text);
    bool loadDict() override;
    bool unloadDict() override;
    bool loadOrBuildIndex();
    bool needBuildIndex();
    bool buildIndex() override;
    bool loadIndex() override;
    bool unloadIndex() override;

    QString m_styleSheet;
    FILE *m_dictFile = nullptr;
    FILE *m_indexFile = nullptr;
    MdxIndex *m_dictIndex = nullptr;
    mdx_data *m_mdxData = nullptr;
};

#endif // MDXDICT_H
