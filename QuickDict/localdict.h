#ifndef LOCALDICT_H
#define LOCALDICT_H

#include "dictservice.h"

class LocalDict : public DictService
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool sorted READ sorted WRITE setSorted NOTIFY sortedChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

public:
    explicit LocalDict(QObject *parent = nullptr);
    virtual ~LocalDict();

    inline QString source() const { return m_dictFileName; }
    void setSource(const QString &source);

    inline bool sorted() const { return m_sorted; }
    void setSorted(bool sorted);

    inline bool loaded() const { return m_loaded; }
    void setLoaded(bool loaded);

Q_SIGNALS:
    void sourceChanged(const QString &source);
    void sortedChanged(bool sorted);
    void loadedChanged(bool loaded);

protected:
    bool doSetEnabled(bool enabled) override;
    virtual bool loadDict() = 0;
    virtual bool unloadDict() = 0;
    bool loadOrBuildIndex();
    bool needBuildIndex();
    virtual bool buildIndex() = 0;
    virtual bool loadIndex() = 0;
    virtual bool unloadIndex() = 0;

    QString m_dictFileName;
    QString m_indexFileName;
    bool m_sorted = false; // defaults to unsorted
    bool m_loaded = false;
};

#endif // LOCALDICT_H
