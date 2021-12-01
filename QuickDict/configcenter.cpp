#include "configcenter.h"
#include "quickdict.h"
#include <QCoreApplication>
#include <QMutexLocker>

ConfigCenter::ConfigCenter(const QString &fileName, QSettings::Format format, QObject *parent)
    : QObject(parent)
    , m_config(fileName, format)
{}

ConfigCenter::~ConfigCenter() {}

QVariant ConfigCenter::value(const QString &key, const QVariant &defaultValue, bool store)
{
    QMutexLocker locker(&m_mutex);
    QStringList groupSplit = m_config.group().split("/", Qt::SkipEmptyParts);
    if (key.startsWith('/') && !m_config.group().isEmpty()) {
        for (const auto &_ : qAsConst(groupSplit)) {
            Q_UNUSED(_)
            m_config.endGroup();
        }
    }
    QVariant v = m_config.value(key, defaultValue);
    if (!m_config.contains(key) && store) {
        m_config.setValue(key, defaultValue);
        m_config.sync();
        QString absoluteKey = "/" + (groupSplit + key.split("/", Qt::SkipEmptyParts)).join('/');
        emit valueChanged(absoluteKey, defaultValue);
    }
    if (key.startsWith('/') && !m_config.group().isEmpty()) {
        for (const auto &group : qAsConst(groupSplit))
            m_config.beginGroup(group);
    }
    return v;
}

void ConfigCenter::setValue(const QString &key, const QVariant &value)
{
    QMutexLocker locker(&m_mutex);
    QStringList groupSplit = m_config.group().split("/", Qt::SkipEmptyParts);
    if (key.startsWith('/') && !m_config.group().isEmpty()) {
        for (const auto &_ : qAsConst(groupSplit)) {
            Q_UNUSED(_)
            m_config.endGroup();
        }
    }
    if (m_config.value(key) != value) {
        m_config.setValue(key, value);
        m_config.sync();
        QString absoluteKey = "/" + (groupSplit + key.split("/", Qt::SkipEmptyParts)).join('/');
        qCDebug(qd) << "Config" << absoluteKey << ":" << value;
        emit valueChanged(absoluteKey, value);
        if (key.startsWith('/') && !m_config.group().isEmpty()) {
            for (const auto &group : qAsConst(groupSplit))
                m_config.beginGroup(group);
        }
    }
}
