#include "monitorinterface.h"

Q_LOGGING_CATEGORY(monitor, "qd.monitor")

MonitorInterface::MonitorInterface(QObject *parent)
    : Interface("/Monitor/", parent)
{}

MonitorInterface::~MonitorInterface() {}
