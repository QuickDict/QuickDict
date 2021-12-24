#include "dictservice.h"
#include "quickdict.h"

Q_LOGGING_CATEGORY(qdDict, "qd.dict")

DictService::DictService(QObject *parent)
    : Service("/Dict/", parent)
{}

DictService::~DictService() {}

void DictService::setDelegate(QQmlComponent *delegate)
{
    if (m_delegate != delegate) {
        m_delegate = delegate;
        emit delegateChanged();
    }
}

void DictService::classBegin() {}

void DictService::componentComplete()
{
    QuickDict::instance()->registerDict(this);
}
