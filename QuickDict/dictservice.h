#ifndef DICTSERVICE_H
#define DICTSERVICE_H

#include <QObject>

class DictInterface;

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

private:
    QList<DictInterface *> m_dicts;
};

#endif // DICTSERVICE_H
