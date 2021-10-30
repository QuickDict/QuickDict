#include "dictservice.h"

DictService::DictService(QObject *parent)
    : QObject(parent)
{}

DictService::~DictService() {}

void DictService::registerDict(DictInterface *dict)
{
    m_dicts.push_back(dict);
}
