#include "monitorservice.h"
#include "monitorinterface.h"

MonitorService::MonitorService(QObject *parent)
    : QObject(parent)
{}

MonitorService::~MonitorService() {}

void MonitorService::registerMonitor(MonitorInterface *monitor)
{
    m_monitors.push_back(monitor);
    connect(monitor, &MonitorInterface::query, this, &MonitorService::query);
}
