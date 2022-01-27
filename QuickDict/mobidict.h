#ifndef MOBIDICT_H
#define MOBIDICT_H

#include "dictindex.h"
#include "localdict.h"
#include <libmobi/src/mobi.h>

using MobiKey = QString;
using MobiEntry = std::pair<uint, uint>;
using MobiIndex = DictIndex<MobiKey, MobiEntry>;

class MobiDict : public LocalDict
{
    Q_OBJECT
    Q_PROPERTY(QString serialNumber READ serialNumber WRITE setSerialNumber NOTIFY serialNumberChanged)

public:
    explicit MobiDict(QObject *parent = nullptr);
    virtual ~MobiDict();

    inline QString serialNumber() const { return m_serialNumber; }
    void setSerialNumber(const QString &serialNumber);

Q_SIGNALS:
    void serialNumberChanged(const QString &serialNumber);

protected:
    void onQuery(const QString &text);
    bool loadDict() override;
    bool unloadDict() override;
    bool loadOrBuildIndex();
    bool needBuildIndex();
    bool buildIndex() override;
    bool loadIndex() override;
    bool unloadIndex() override;

    FILE *m_dictFile = nullptr;
    FILE *m_indexFile = nullptr;
    MOBIRawml *m_mobiRawml = nullptr;
    MobiIndex *m_dictIndex = nullptr;
    QString m_serialNumber;
};

#endif // MOBIDICT_H
