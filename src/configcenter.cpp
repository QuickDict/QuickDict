#include "configcenter.h"
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
    QStringList groupSplit = m_config.group().split(Qt::SkipEmptyParts);
    if (key.startsWith('/')) {
        for (const auto &_ : groupSplit)
            m_config.endGroup();
    }
    QVariant v = m_config.value(key, defaultValue);
    if (!m_config.contains(key) && store) {
        m_config.setValue(key, defaultValue);
        m_config.sync();
        QString absoluteKey = "/" + (groupSplit + key.split(Qt::SkipEmptyParts)).join('/');
        emit valueChanged(absoluteKey, defaultValue);
    }
    if (key.startsWith('/')) {
        for (const auto &group : groupSplit)
            m_config.beginGroup(group);
    }
    return v;
}

void ConfigCenter::setValue(const QString &key, const QVariant &value)
{
    QMutexLocker locker(&m_mutex);
    QStringList groupSplit = m_config.group().split(Qt::SkipEmptyParts);
    if (key.startsWith('/')) {
        for (const auto &_ : groupSplit)
            m_config.endGroup();
    }
    m_config.setValue(key, value);
    m_config.sync();
    QString absoluteKey = "/" + (groupSplit + key.split(Qt::SkipEmptyParts)).join('/');
    emit valueChanged(absoluteKey, value);
    if (key.startsWith('/')) {
        for (const auto &group : groupSplit)
            m_config.beginGroup(group);
    }
}
