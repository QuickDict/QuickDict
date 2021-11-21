#ifndef DICTSERVICE_H
#define DICTSERVICE_H

#include "service.h"
#include <QJsonObject>
#include <QLoggingCategory>

class DictService : public Service
{
    Q_OBJECT
public:
    explicit DictService(QObject *parent = nullptr);
    virtual ~DictService();

Q_SIGNALS:
    void query(const QString &text);
    void queryResult(const QJsonObject &result);
};

Q_DECLARE_LOGGING_CATEGORY(qdDict)

#endif // DICTSERVICE_H
