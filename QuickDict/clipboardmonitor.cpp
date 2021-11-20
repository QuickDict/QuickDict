#include "clipboardmonitor.h"
#include <QGuiApplication>

ClipboardMonitor::ClipboardMonitor(QObject *parent)
    : MonitorInterface(parent)
{
    setName(tr("ClipboardMonitor"));
    setDescription(tr("ClipboardMonitor monitors clipboard text."));
}

ClipboardMonitor::~ClipboardMonitor() {}

void ClipboardMonitor::doSetState(State state)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (state == State::Enabled) {
        connect(clipboard, &QClipboard::changed, this, &ClipboardMonitor::onChanged);
    } else {
        disconnect(clipboard, &QClipboard::changed, this, &ClipboardMonitor::onChanged);
    }
}

void ClipboardMonitor::onChanged(QClipboard::Mode mode)
{
    emit query(QGuiApplication::clipboard()->text(mode));
}
