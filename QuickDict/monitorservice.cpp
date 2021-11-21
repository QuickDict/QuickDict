#include "monitorservice.h"

Q_LOGGING_CATEGORY(qdMonitor, "qd.monitor")

MonitorService::MonitorService(QObject *parent)
    : Service("/Monitor/", parent)
{}

MonitorService::~MonitorService() {}
