#include "quickdict.h"
#include <QGuiApplication>
#include <QJSValue>
#include <QScreen>
#include <QTimer>

Q_LOGGING_CATEGORY(qd, "qd.default")

QuickDict *QuickDict::_instance = nullptr;

QuickDict::QuickDict(QObject *parent)
    : QObject(parent)
{
    m_pixelScale = qApp->primaryScreen()->physicalDotsPerInch() / 160.0;
}

QuickDict::~QuickDict() {}

void QuickDict::createInstance()
{
    if (!_instance)
        _instance = new QuickDict;
}

void QuickDict::setTimeout(const QVariant &function, int delay)
{
    QJSValue callable = function.value<QJSValue>();
    if (callable.isCallable()) {
        // NOTE: `fuction` must be passed to lambda by value!
        QTimer::singleShot(delay, this, [function]() {
            QJSValue callable = function.value<QJSValue>();
            callable.call();
        });
    } else {
        qCCritical(qd) << "function is not callable";
    }
}

qreal QuickDict::dp(qreal value) const
{
    return value * m_pixelScale * m_dpScale * m_uiScale;
}

qreal QuickDict::sp(qreal value) const
{
    return value * m_pixelScale * m_spScale * m_uiScale;
}

void QuickDict::setDpScale(qreal dpScale)
{
    m_dpScale = dpScale;
    emit dpScaleChanged();
}

void QuickDict::setSpScale(qreal spScale)
{
    m_spScale = spScale;
    emit spScaleChanged();
}

void QuickDict::setUiScale(qreal uiScale)
{
    m_uiScale = uiScale;
    emit uiScaleChanged();
}
