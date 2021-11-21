#ifndef DICTSERVICE_H
#define DICTSERVICE_H

#include "dictinterface.h"

class DictService : public QObject
{
    Q_OBJECT
public:
    explicit DictService(QObject *parent = nullptr);
    virtual ~DictService();

    Q_INVOKABLE void registerDict(DictInterface *dict);

Q_SIGNALS:
    void query(const QString &text);
    void queryResult(const QJsonObject &result);

private Q_SLOTS:
    void onDictStateChanged(Interface::State state);

private:
    void handleDict(DictInterface *dict, Interface::State state);

    QList<DictInterface *> m_dicts;
};

#endif // DICTSERVICE_H
