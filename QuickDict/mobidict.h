#ifndef MOBIDICT_H
#define MOBIDICT_H

#include "dictindex.h"
#include "dictservice.h"
#include <libmobi/src/mobi.h>

using MobiEntry = std::pair<uint, uint>;
using MobiIndex = DictIndex<QString, MobiEntry>;

class MobiDict : public DictService
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString loaded READ loaded NOTIFY loadedChanged)

public:
    explicit MobiDict(QObject *parent = nullptr);
    virtual ~MobiDict();

    inline QString source() const { return m_dictFileName; }
    void setSource(const QString &source);

    inline bool loaded() const { return m_loaded; }
    void setLoaded(bool loaded);

Q_SIGNALS:
    void sourceChanged(const QString &source);
    void loadedChanged(bool loaded);

protected:
    bool doSetEnabled(bool enabled) override;

private:
    void onQuery(const QString &text);
    bool loadDict();
    bool unloadDict();
    bool loadOrBuildIndex();
    bool needBuildIndex();
    bool buildIndex();
    bool loadIndex();
    bool unloadIndex();

    QString m_dictFileName;
    QString m_indexFileName;
    // FILE *m_dictFile = nullptr;
    // FILE *m_indexFile = nullptr;
    MOBIRawml *m_rawMarkup = nullptr;
    MobiIndex *m_dictIndex = nullptr;
    bool m_loaded = false;
};

#endif // MOBIDICT_H
