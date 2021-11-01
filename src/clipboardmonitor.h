#ifndef CLIPBOARDMONITOR_H
#define CLIPBOARDMONITOR_H

#include "monitorinterface.h"
#include <QClipboard>

class ClipboardMonitor : public MonitorInterface
{
    Q_OBJECT
public:
    explicit ClipboardMonitor(QObject *parent = nullptr);
    ~ClipboardMonitor();

    void doSetState(State state) override;

private Q_SLOTS:
    void onChanged(QClipboard::Mode mode);
};

#endif // CLIPBOARDMONITOR_H
