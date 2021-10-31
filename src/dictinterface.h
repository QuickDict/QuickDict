#ifndef DICTINTERFACE_H
#define DICTINTERFACE_H

#include <QJsonObject>
#include <QObject>
#include <QString>

class DictInterface : public QObject
{
    Q_OBJECT
public:
    explicit DictInterface(QObject *parent = nullptr);
    virtual ~DictInterface();

Q_SIGNALS:
    void query(const QString &text);
    void queryResult(const QJsonObject &result);
};

#endif // DICTINTERFACE_H
