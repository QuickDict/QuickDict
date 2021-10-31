#ifndef MOUSEOVERMONITOR_H
#define MOUSEOVERMONITOR_H

#include "monitorinterface.h"
#include <QPoint>
#include <QString>

class OcrResult;
class QTimer;

class MouseOverMonitor : public MonitorInterface
{
    Q_OBJECT
public:
    explicit MouseOverMonitor(QObject *parent = nullptr);
    ~MouseOverMonitor();

    void doSetState(State state) override;

    void setIdleInterval(int interval) { m_idleInterval = interval; };
    inline int idleInterval() const { return m_idleInterval; }
    void setBusyInterval(int interval) { m_busyInterval = interval; };
    inline int busyInterval() const { return m_busyInterval; }

private Q_SLOTS:
    void timeout();
    void onExtractTextResult(const OcrResult &result);

private:
    int m_idleInterval = 500;
    int m_busyInterval = 100;
    bool m_previousCursorMoving = false;

    QString m_previousWord;
    QPoint m_previousCursor;
    QTimer *m_timer;
};

#endif // MOUSEOVERMONITOR_H
