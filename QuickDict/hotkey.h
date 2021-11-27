#ifndef HOTKEY_H
#define HOTKEY_H

#include <QHotkey>

class Hotkey : public QHotkey
{
    Q_OBJECT
    Q_PROPERTY(QString sequence READ sequence WRITE setSequence NOTIFY sequenceChanged);

public:
    explicit Hotkey(const QString &sequence = QString(), QObject *parent = nullptr);
    virtual ~Hotkey();

    QString sequence() const;
    bool setSequence(const QString &sequence);

Q_SIGNALS:
    void sequenceChanged(const QString &sequence);
};

#endif // HOTKEY_H
