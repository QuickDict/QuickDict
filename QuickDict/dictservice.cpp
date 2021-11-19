#include "dictservice.h"
#include "dictinterface.h"

DictService::DictService(QObject *parent)
    : QObject(parent)
{}

DictService::~DictService() {}

void DictService::registerDict(DictInterface *dict)
{
    m_dicts.push_back(dict);
    connect(this, &DictService::query, dict, &DictInterface::query);
    connect(dict, &DictInterface::queryResult, this, &DictService::queryResult);
}
