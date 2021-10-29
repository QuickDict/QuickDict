#include "configcenter.h"
#include <QCoreApplication>
#include <QMutexLocker>

ConfigCenter ConfigCenter::_instance;

ConfigCenter::ConfigCenter(QObject *parent)
    : QObject(parent)
    , config(new QSettings("settings.ini", QSettings::IniFormat, this))
{}

ConfigCenter::~ConfigCenter() {}

void ConfigCenter::setConfigFile(const QString &fileName, QSettings::Format format)
{
    delete _instance.config;
    _instance.config = new QSettings(fileName, format);
}

QVariant ConfigCenter::value(const QString &key, const QVariant &defaultValue)
{
    QMutexLocker locker(&instance().mutex);
    // a `key` starts with '/' means it's an absolute path
    QSettings &s = settings();
    QStringList groupSplit = s.group().split(Qt::SkipEmptyParts);
    if (key.startsWith('/')) {
        for (const auto &_ : groupSplit)
            s.endGroup();
    }
    QVariant v = settings().value(key, defaultValue);
    if (key.startsWith('/')) {
        for (const auto &group : groupSplit)
            s.beginGroup(group);
    }
    return v;
}

void ConfigCenter::setValue(const QString &key, const QVariant &value)
{
    QMutexLocker locker(&instance().mutex);
    // a `key` starts with '/' means it's an absolute path
    QSettings &s = settings();
    QStringList groupSplit = s.group().split(Qt::SkipEmptyParts);
    if (key.startsWith('/')) {
        for (const auto &_ : groupSplit)
            s.endGroup();
    }
    settings().setValue(key, value);
    settings().sync();
    if (key.startsWith('/')) {
        for (const auto &group : groupSplit)
            s.beginGroup(group);
    }
    emit instance().valueChanged(QString("/%0/%1").arg(settings().group()).arg(key), value);
}
