#include "dictservice.h"

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
