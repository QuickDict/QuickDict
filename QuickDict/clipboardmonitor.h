#ifndef CLIPBOARDMONITOR_H
#define CLIPBOARDMONITOR_H

#include "monitorservice.h"
#include <QClipboard>

class ClipboardMonitor : public MonitorService
{
    Q_OBJECT
public:
    explicit ClipboardMonitor(QObject *parent = nullptr);
    ~ClipboardMonitor();

protected:
    void doSetEnabled(bool enabled) override;

private Q_SLOTS:
    void onChanged(QClipboard::Mode mode);
};

#endif // CLIPBOARDMONITOR_H
