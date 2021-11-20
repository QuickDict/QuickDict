#ifndef MONITORINTERFACE_H
#define MONITORINTERFACE_H

#include <QObject>
#include <QString>

class MonitorInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged);

public:
    enum class State {
        Disabled,
        Enabled,
    };

    /**
     * @see MonitorInterface::setName.
     */
    explicit MonitorInterface(const QString &name = QString(),
                              const QString &description = QString(),
                              QObject *parent = nullptr);
    virtual ~MonitorInterface();

    QString name() const { return m_name; }
    /**
     * @param name name is used as key in config.
     */
    void setName(const QString &name);
    QString description() const { return m_description; }
    void setDescription(const QString &description);

    void setEnabled(bool enabled = true);
    inline void setDisabled(bool disabled = true) { setEnabled(!disabled); }
    inline bool isEnabled() const { return m_state == State::Enabled; }
    void toggle();

    void setState(State state);
    inline State state() const { return m_state; }

Q_SIGNALS:
    void stateChanged(MonitorInterface::State state);
    void query(const QString &text);

    void nameChanged(const QString &name);
    void descriptionChanged(const QString &description);

protected:
    virtual void doSetState(State state){};

private:
    void saveConfig();
    void loadConfig();

    State m_state = State::Disabled;
    QString m_name;
    QString m_description;
};

#endif // MONITORINTERFACE_H
