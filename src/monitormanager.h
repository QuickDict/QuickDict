#ifndef MONITORMANAGER_H
#define MONITORMANAGER_H

#include <QList>
#include <QObject>

class MonitorInterface;

class MonitorManager : public QObject
{
    Q_OBJECT
public:
    explicit MonitorManager(QObject *parent = nullptr);
    virtual ~MonitorManager();

    void registerMonitor(MonitorInterface *monitor);

private:
    QList<MonitorInterface *> m_monitors;
};

#endif // MONITORMANAGER_H
