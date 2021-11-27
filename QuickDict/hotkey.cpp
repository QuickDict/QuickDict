#include "hotkey.h"

Hotkey::Hotkey(const QString &sequence, QObject *parent)
    : QHotkey(sequence, true, parent)
{}

Hotkey::~Hotkey() {}

QString Hotkey::sequence() const
{
    return shortcut().toString();
}

bool Hotkey::setSequence(const QString &sequence)
{
    if (isRegistered())
        setRegistered(false);
    bool registered = setShortcut(QKeySequence(sequence), true);
    emit sequenceChanged(sequence);
    return registered;
}
