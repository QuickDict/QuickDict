#include "monitorinterface.h"
#include "configcenter.h"
#include "quickdict.h"

MonitorInterface::MonitorInterface(const QString &name, const QString &description, QObject *parent)
    : QObject(parent)
{
    setName(name);
    setDescription(description);
}

MonitorInterface::~MonitorInterface() {}

void MonitorInterface::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        loadConfig();

        emit nameChanged(m_name);
    }
}

void MonitorInterface::setDescription(const QString &description)
{
    if (m_description != description) {
        m_description = description;
        emit descriptionChanged(m_description);
    }
}

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
        saveConfig();

        emit stateChanged(m_state);
    }
}

void MonitorInterface::saveConfig()
{
    if (!m_name.isEmpty())
        QuickDict::instance()->configCenter()->setValue("/Monitor/" + m_name, static_cast<int>(m_state));
}

void MonitorInterface::loadConfig()
{
    if (!m_name.isEmpty()) {
        QVariant state = QuickDict::instance()->configCenter()->value("/Monitor/" + m_name);
        if (state.isValid())
            setState(static_cast<State>(state.toInt()));
    }
}
