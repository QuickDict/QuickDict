#include "quickdict.h"
#include <QJSValue>
#include <QTimer>

Q_LOGGING_CATEGORY(qd, "qd.main")

QuickDict *QuickDict::_instance = new QuickDict;

QuickDict::QuickDict(QObject *parent)
    : QObject(parent)
{}

QuickDict::~QuickDict() {}

void QuickDict::setTimeout(const QVariant &function, int delay)
{
    QJSValue callable = function.value<QJSValue>();
    if (callable.isCallable()) {
        // NOTE: `fuction` must be passed to lambda by value!
        QTimer::singleShot(delay, this, [this, function]() {
            QJSValue callable = function.value<QJSValue>();
            callable.call();
        });
    } else {
        qCCritical(qd) << "function is not callable";
    }
}
