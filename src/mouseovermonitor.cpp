#include "mouseovermonitor.h"
#include "ocrengine.h"
#include "quickdict.h"
#include <QCursor>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QTimer>

MouseOverMonitor::MouseOverMonitor(QObject *parent)
    : MonitorInterface(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &MouseOverMonitor::timeout);
}

MouseOverMonitor::~MouseOverMonitor() {}

void MouseOverMonitor::doSetState(State state)
{
    if (state == State::Enabled) {
        m_previousCursorMoving = true;
        m_previousCursor = QCursor::pos();
        m_timer->start(m_idleInterval);
    } else {
        m_timer->stop();
    }
}

void MouseOverMonitor::timeout()
{
    m_timer->stop();
    QPoint cursor = QCursor::pos();
    int interval;

    if (m_previousCursorMoving && cursor == m_previousCursor) {
        // mouse stops now
        m_previousCursorMoving = false;
        interval = m_idleInterval;
        QImage screen = QGuiApplication::primaryScreen()->grabWindow(0).toImage();
        emit QuickDict::instance()->ocrEngine()->extractText(screen, cursor);
    } else if (m_previousCursorMoving && cursor != m_previousCursor) {
        // mouse keeps moving
        m_previousCursorMoving = true;
        m_previousCursor = cursor;
        interval = m_busyInterval;
    } else if (!m_previousCursorMoving && cursor == m_previousCursor) {
        // mouse keeps stands-still
        m_previousCursorMoving = false;
        interval = m_idleInterval;
    } else {
        // mouse moves now
        m_previousCursorMoving = true;
        m_previousCursor = cursor;
        interval = m_busyInterval;
    }
    m_timer->start(interval);
}
