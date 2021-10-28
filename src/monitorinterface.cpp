#include "monitorinterface.h"

MonitorInterface::MonitorInterface(QObject *parent)
    : QObject(parent)
{}

MonitorInterface::~MonitorInterface() {}

void MonitorInterface::setEnabled(bool enabled)
{
    if (enabled)
        setState(State::Enabled);
    else
        setState(State::Disabled);
};

void MonitorInterface::toggle()
{
    setEnabled(!isEnabled());
}

void MonitorInterface::setState(State state)
{
    if (m_state != state) {
        doSetState(state);
        m_state = state;
        emit stateChanged(m_state);
    }
}
