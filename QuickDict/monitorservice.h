#ifndef MONITORSERVICE_H
#define MONITORSERVICE_H

#include "service.h"
#include <QLoggingCategory>

class MonitorService : public Service
{
    Q_OBJECT
public:
    explicit MonitorService(QObject *parent = nullptr);
    virtual ~MonitorService();

Q_SIGNALS:
    void query(const QString &text);
};

Q_DECLARE_LOGGING_CATEGORY(qdMonitor)

#endif // MONITORSERVICE_H
