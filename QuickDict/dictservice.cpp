#include "dictservice.h"

Q_LOGGING_CATEGORY(qdDict, "qd.dict")

DictService::DictService(QObject *parent)
    : Service("/Dict/", parent)
{}

DictService::~DictService() {}
