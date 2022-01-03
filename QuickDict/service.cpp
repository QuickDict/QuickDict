#include "service.h"
#include "configcenter.h"
#include "quickdict.h"

Service::Service(const QString &interfaceName, QObject *parent)
    : QObject(parent)
    , m_interfaceName(interfaceName)
{}

Service::~Service() {}

void Service::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;

        if (!m_name.isEmpty()) {
            QVariant enabled = QuickDict::instance()->configCenter()->value(m_interfaceName + m_name);
            if (enabled.isValid())
                loadConfig();
            else
                saveConfig();
        }

        emit nameChanged(m_name);
    }
}

void Service::setDescription(const QString &description)
{
    if (m_description != description) {
        m_description = description;
        emit descriptionChanged(m_description);
    }
}

void Service::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        if (!doSetEnabled(enabled))
            return;
        m_enabled = enabled;
        saveConfig();

        emit enabledChanged(m_enabled);
    }
}

void Service::toggle()
{
    setEnabled(!enabled());
}

void Service::saveConfig()
{
    if (!m_name.isEmpty())
        QuickDict::instance()->configCenter()->setValue(m_interfaceName + m_name, static_cast<int>(m_enabled));
}

void Service::loadConfig()
{
    if (!m_name.isEmpty()) {
        QVariant enabled = QuickDict::instance()->configCenter()->value(m_interfaceName + m_name);
        if (enabled.isValid())
            setEnabled(static_cast<bool>(enabled.toInt()));
    }
}
