#include "quickdict.h"
#include "configcenter.h"
#include "dictservice.h"
#include "monitorservice.h"
#include <QFontMetrics>
#include <QGuiApplication>
#include <QJSValue>
#include <QScreen>
#include <QTimer>

Q_LOGGING_CATEGORY(qd, "qd.default")

QuickDict *QuickDict::_instance = nullptr;

QuickDict::QuickDict(QObject *parent)
    : QObject(parent)
#ifdef ENABLE_HUNSPELL
    // TODO: make it configurable
    , m_hunspell(new Hunspell("/usr/share/hunspell/en_US.aff", "/usr/share/hunspell/en_US.dic"))
#endif
{
    m_pixelScale = qApp->primaryScreen()->physicalDotsPerInch() / 160.0;
}

QuickDict::~QuickDict()
{
#ifdef ENABLE_HUNSPELL
    delete m_hunspell;
#endif
}

void QuickDict::createInstance()
{
    if (!_instance)
        _instance = new QuickDict;
}

void QuickDict::setConfigCenter(ConfigCenter *configCenter)
{
    m_configCenter = configCenter;
    connect(m_configCenter, &ConfigCenter::valueChanged, this, &QuickDict::onConfigChanged);
}

QString QuickDict::sourceLanguage() const
{
    return configCenter()->value("/lang/sl", "en_US").toString();
}

void QuickDict::setSourceLanguage(const QString &sourceLang)
{
    configCenter()->setValue("/lang/sl", sourceLang);
}

QString QuickDict::targetLanguage() const
{
    return configCenter()->value("/lang/tl", "en_US").toString();
}

void QuickDict::setTargetLanguage(const QString &targetLang)
{
    configCenter()->setValue("/lang/tl", targetLang);
}

void QuickDict::loadConfig()
{
    QString sl = configCenter()->value("/lang/sl", "en_US").toString();
    emit sourceLanguageChanged(sl);
    QString tl = configCenter()->value("/lang/tl", "en_US").toString();
    emit targetLanguageChanged(tl);
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

void QuickDict::registerMonitor(MonitorService *monitor)
{
    qCInfo(qdMonitor) << "Register monitor:" << monitor->name();
    m_monitors.push_back(monitor);
    handleMonitor(monitor, monitor->enabled());
    connect(monitor, &MonitorService::enabledChanged, this, &QuickDict::onMonitorEnabledChanged);
    emit monitorsChanged();
}

void QuickDict::registerDict(DictService *dict)
{
    qCInfo(qdDict) << "Register dict:" << dict->name();
    m_dicts.push_back(dict);
    handleDict(dict, dict->enabled());
    connect(dict, &DictService::enabledChanged, this, &QuickDict::onDictEnabledChanged);
    emit dictsChanged();
}

QList<QObject *> QuickDict::monitors() const
{
    QList<QObject *> l;
    for (MonitorService *monitor : m_monitors)
        l.append(qobject_cast<QObject *>(monitor));
    return l;
}

MonitorService *QuickDict::monitor(const QString &name) const
{
    for (MonitorService *monitor : m_monitors) {
        if (monitor->name() == name)
            return monitor;
    }
    return nullptr;
}

QList<QObject *> QuickDict::dicts() const
{
    QList<QObject *> l;
    for (DictService *dict : m_dicts)
        l.append(qobject_cast<QObject *>(dict));
    return l;
}

DictService *QuickDict::dict(const QString &name) const
{
    for (DictService *dict : m_dicts) {
        if (dict->name() == name)
            return dict;
    }
    return nullptr;
}

QStringList QuickDict::availableLocales() const
{
    QStringList l;
    l << QLocale(QLocale::Chinese, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Chinese, QLocale::Taiwan).name();
    l << QLocale(QLocale::English, QLocale::AnyCountry).name();
    l << QLocale(QLocale::French, QLocale::AnyCountry).name();
    l << QLocale(QLocale::German, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Persian, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Italian, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Japanese, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Urdu, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Korean, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Russian, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Spanish, QLocale::AnyCountry).name();
    l << QLocale(QLocale::Vietnamese, QLocale::AnyCountry).name();
    return l;
}

QObject *QuickDict::findChild(const QString &name, QObject *parent) const
{
    if (!parent)
        parent = qApp;
    return parent->findChild<QObject *>(name);
}

QRect QuickDict::textBoundingRect(const QFont &font, const QString &text) const
{
    QFontMetrics fm(font);
    return fm.boundingRect(text);
}

void QuickDict::onMonitorEnabledChanged(bool enabled)
{
    MonitorService *monitor = qobject_cast<MonitorService *>(sender());
    handleMonitor(monitor, enabled);
}

void QuickDict::onDictEnabledChanged(bool enabled)
{
    DictService *dict = qobject_cast<DictService *>(sender());
    handleDict(dict, enabled);
}

void QuickDict::handleMonitor(MonitorService *monitor, bool enabled)
{
    qCInfo(qdMonitor) << "Monitor:" << monitor->name() << "enabled:" << enabled;
    if (enabled) {
        connect(monitor, &MonitorService::query, this, &QuickDict::query);
    } else {
        disconnect(monitor, &MonitorService::query, this, &QuickDict::query);
    }
}

void QuickDict::handleDict(DictService *dict, bool enabled)
{
    qCInfo(qdDict) << "Dict:" << dict->name() << "enabled:" << enabled;
    if (enabled) {
        connect(this, &QuickDict::query, dict, &DictService::query);
        connect(dict, &DictService::queryResult, this, &QuickDict::queryResult);
    } else {
        disconnect(this, &QuickDict::query, dict, &DictService::query);
        disconnect(dict, &DictService::queryResult, this, &QuickDict::queryResult);
    }
}

void QuickDict::onConfigChanged(const QString &key, const QVariant &value)
{
    if (key == QStringLiteral("/lang/sl")) {
        emit sourceLanguageChanged(value.toString());
    } else if (key == QStringLiteral("/lang/tl")) {
        emit targetLanguageChanged(value.toString());
    }
}
