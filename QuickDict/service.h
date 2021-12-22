#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QString>

class Service : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged);
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged);

public:
    explicit Service(const QString &interfaceName, QObject *parent = nullptr);
    virtual ~Service();

    QString name() const { return m_name; }
    /**
     * @param name name is used as key in config.
     */
    void setName(const QString &name);
    QString description() const { return m_description; }
    void setDescription(const QString &description);
    void setEnabled(bool enabled = true);
    bool enabled() const { return m_enabled; }
    Q_INVOKABLE inline void setDisabled(bool disabled = true) { setEnabled(!disabled); }
    Q_INVOKABLE void toggle();

Q_SIGNALS:
    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);
    void enabledChanged(bool enabled);

protected:
    /**
     * @return @c true if successful, @c false otherwise.
     */
    virtual bool doSetEnabled(bool enabled) { return true; }

private:
    void saveConfig();
    void loadConfig();

    bool m_enabled = false;
    QString m_interfaceName;
    QString m_name;
    QString m_description;
};

#endif // INTERFACE_H
