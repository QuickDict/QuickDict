#ifndef MONITORINTERFACE_H
#define MONITORINTERFACE_H

#include "interface.h"
#include <QLoggingCategory>

class MonitorInterface : public Interface
{
    Q_OBJECT
public:
    explicit MonitorInterface(QObject *parent = nullptr);
    virtual ~MonitorInterface();

Q_SIGNALS:
    void query(const QString &text);
};

Q_DECLARE_LOGGING_CATEGORY(monitor)

#endif // MONITORINTERFACE_H
