#include "mouseovermonitor.h"
#include "ocrengine.h"
#include "quickdict.h"
#include <QCursor>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QTimer>

MouseOverMonitor::MouseOverMonitor(QObject *parent)
    : MonitorService(parent)
    , m_timer(new QTimer(this))
{
    setName(tr("MouseOverMonitor"));
    setDescription(tr("MouseOverMonitor monitors text under cursor."));

    connect(m_timer, &QTimer::timeout, this, &MouseOverMonitor::timeout);
    connect(QuickDict::instance()->ocrEngine(),
            &OcrEngine::extractTextResult,
            this,
            &MouseOverMonitor::onExtractTextResult);
}

MouseOverMonitor::~MouseOverMonitor() {}

bool MouseOverMonitor::doSetEnabled(bool enabled)
{
    if (enabled) {
        m_previousCursorMoving = true;
        m_previousCursor = QCursor::pos();
        m_timer->start(m_idleInterval);
        QuickDict::instance()->ocrEngine()->start();
    } else {
        m_timer->stop();
        QuickDict::instance()->ocrEngine()->stop();
    }
    return true;
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

void MouseOverMonitor::onExtractTextResult(const OcrResult &result)
{
    emit query(result.text);
}
