#ifndef MONITORMANAGER_H
#define MONITORMANAGER_H

#include <QList>
#include <QObject>

class MonitorInterface;

class MonitorService : public QObject
{
    Q_OBJECT
public:
    explicit MonitorService(QObject *parent = nullptr);
    virtual ~MonitorService();

    Q_INVOKABLE void registerMonitor(MonitorInterface *monitor);

Q_SIGNALS:
    void query(const QString &text);

private:
    QList<MonitorInterface *> m_monitors;
};

#endif // MONITORMANAGER_H
