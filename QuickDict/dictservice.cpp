#include "dictservice.h"
#include "dictinterface.h"

DictService::DictService(QObject *parent)
    : QObject(parent)
{}

DictService::~DictService() {}

void DictService::registerDict(DictInterface *dict)
{
    m_dicts.push_back(dict);
    handleDict(dict, dict->state());
    connect(dict, &DictInterface::stateChanged, this, &DictService::onDictStateChanged);
}

void DictService::onDictStateChanged(Interface::State state)
{
    DictInterface *dict = qobject_cast<DictInterface *>(sender());
    if (dict)
        handleDict(dict, state);
}

void DictService::handleDict(DictInterface *dict, Interface::State state)
{
    if (Interface::State::Enabled == state) {
        connect(this, &DictService::query, dict, &DictInterface::query);
        connect(dict, &DictInterface::queryResult, this, &DictService::queryResult);
    } else {
        disconnect(this, &DictService::query, dict, &DictInterface::query);
        disconnect(dict, &DictInterface::queryResult, this, &DictService::queryResult);
    }
}
