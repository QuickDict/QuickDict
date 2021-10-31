#ifndef MONITORINTERFACE_H
#define MONITORINTERFACE_H

#include <QObject>
#include <QString>

class MonitorInterface : public QObject
{
    Q_OBJECT
public:
    enum class State {
        Disabled,
        Enabled,
    };

    explicit MonitorInterface(QObject *parent = nullptr);
    virtual ~MonitorInterface();

    QString name() const;
    void setName(const QString &name);
    QString description() const;
    void setDescription(const QString &description);

    void setEnabled(bool enabled = true);
    inline void setDisabled(bool disabled = true) { setEnabled(!disabled); }
    inline bool isEnabled() const { return m_state == State::Enabled; }
    void toggle();

    void setState(State state);
    inline State state() const { return m_state; }

Q_SIGNALS:
    void stateChanged(State state);
    void query(const QString &text);

protected:
    virtual void doSetState(State state) = 0;

private:
    State m_state = State::Disabled;
    QString m_name;
    QString m_description;
};

#endif // MONITORINTERFACE_H
