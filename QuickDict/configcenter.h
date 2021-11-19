#ifndef CONFIGCENTER_H
#define CONFIGCENTER_H

#include <QMutex>
#include <QSettings>

class ConfigCenter : public QObject
{
    Q_OBJECT
public:
    explicit ConfigCenter(const QString &fileName,
                          QSettings::Format format = QSettings::IniFormat,
                          QObject *parent = nullptr);
    ~ConfigCenter();

    inline QSettings *settings() { return &m_config; }
    /**
     * @param key a key starts with '/' means it is absolute in group.
     * @param store stores defaultValue in key if such key doesn't exist and if store is true.
     */
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant(), bool store = false);
    /**
     * @param key a key starts with '/' means it is absolute in group.
     */
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);

Q_SIGNALS:
    void valueChanged(const QString &key, const QVariant &value);

private:
    QSettings m_config;
    QMutex m_mutex;
};

#endif // CONFIGCENTER_H
