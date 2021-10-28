#include "configcenter.h"
#include <QCoreApplication>

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
    return settings().value(key, defaultValue);
}

void ConfigCenter::setValue(const QString &key, const QVariant &value)
{
    settings().setValue(key, value);
    settings().sync();
    // FIXME: absolute key
    emit _instance.valueChanged(key, value);
}
