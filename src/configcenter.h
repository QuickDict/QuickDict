#ifndef CONFIGCENTER_H
#define CONFIGCENTER_H

#include <QMutex>
#include <QSettings>

class ConfigCenter : QObject
{
    Q_OBJECT
public:
    ~ConfigCenter();

    static ConfigCenter &instance() { return _instance; }
    static void setConfigFile(const QString &fileName, QSettings::Format format = QSettings::IniFormat);
    inline static QSettings &settings() { return *instance().config; }
    static QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
    static void setValue(const QString &key, const QVariant &value);

Q_SIGNALS:
    void valueChanged(const QString &key, const QVariant &value);

private:
    explicit ConfigCenter(QObject *parent = nullptr);

private:
    QSettings *config = nullptr;
    static ConfigCenter _instance;
    QMutex mutex;
};

#endif // CONFIGCENTER_H
