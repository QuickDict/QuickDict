#include "clipboardmonitor.h"
#include <QGuiApplication>

ClipboardMonitor::ClipboardMonitor(QObject *parent)
    : MonitorService(parent)
{
    setName(tr("ClipboardMonitor"));
    setDescription(tr("ClipboardMonitor monitors clipboard text."));
}

ClipboardMonitor::~ClipboardMonitor() {}

bool ClipboardMonitor::doSetEnabled(bool enabled)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (enabled) {
        connect(clipboard, &QClipboard::changed, this, &ClipboardMonitor::onChanged);
    } else {
        disconnect(clipboard, &QClipboard::changed, this, &ClipboardMonitor::onChanged);
    }
    return true;
}

void ClipboardMonitor::onChanged(QClipboard::Mode mode)
{
    emit query(QGuiApplication::clipboard()->text(mode));
}
