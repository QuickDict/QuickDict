#include "monitormanager.h"

MonitorManager::MonitorManager(QObject *parent)
    : QObject(parent)
{}

MonitorManager::~MonitorManager() {}

void MonitorManager::registerMonitor(MonitorInterface *monitor)
{
    m_monitors.push_back(monitor);
}
