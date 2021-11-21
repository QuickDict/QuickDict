#include "clipboardmonitor.h"
#include "configcenter.h"
#include "dictinterface.h"
#include "dictservice.h"
#include "monitorinterface.h"
#include "monitorservice.h"
#include "mouseovermonitor.h"
#include "ocrengine.h"
#include "quickdict.h"

#if ENABLE_KWIN_BLUR
#include <KWindowEffects>
#endif
#include <QApplication>
#include <QDir>
#include <QHotkey>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QWindow>

static QFile logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    logFile.write(qFormatLogMessage(type, context, message).toUtf8().append('\n'));
    logFile.flush();
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<OcrResult>("OcrResult");
    // qmlRegisterType<Interface>("com.quickdict.components", 1, 0, "Interface");
    qmlRegisterType<MonitorInterface>("com.quickdict.components", 1, 0, "Monitor");
    qmlRegisterType<DictInterface>("com.quickdict.components", 1, 0, "Dict");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // QQmlApplicationEngine requires QAppliction
    QApplication app(argc, argv);
    app.setOrganizationName("QuickDict");
    app.setOrganizationDomain("https://github.com/QuickDict/QuickDict");
    app.setApplicationName("QuickDict");
    app.setWindowIcon(QIcon(":/images/QuickDict-32x32.png"));

    QString messagePattern =
#ifdef QT_DEBUG
        "[%{time yyyy-MM-dd h:mm:ss.zzz t} "
        "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}"
        "F%{endif}] %{category} %{threadid} %{file}:%{line} - %{message}";
#else
        "[%{time yyyy-MM-dd h:mm:ss.zzz t} "
        "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] "
        "%{category} %{threadid} - %{message}";
#endif
    qSetMessagePattern(messagePattern);
    QLoggingCategory::setFilterRules("qd.*=true");

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    if (!dir.exists(app.applicationName()) && !dir.mkdir(app.applicationName()))
        qCWarning(qd) << "Cannot make dir:" << dir.absoluteFilePath(app.applicationName());
    else {
        dir.cd(app.applicationName());
        logFile.setFileName(dir.absoluteFilePath("log"));
        if (!logFile.open(QIODevice::Append | QIODevice::Text))
            qCWarning(qd) << "Cannot open file:" << logFile.fileName();
        // else
        //     qInstallMessageHandler(messageHandler);
    }

    QuickDict::createInstance();
    QuickDict::instance()->setUiScale(1.5);

    dir = QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    if (!dir.exists(app.applicationName()) && !dir.mkdir(app.applicationName()))
        qCWarning(qd) << "Cannot make dir:" << dir.absoluteFilePath(app.applicationName());
    dir.cd(app.applicationName());
    ConfigCenter configCenter(dir.absoluteFilePath("settings.ini"));
    QuickDict::instance()->setConfigCenter(&configCenter);

    OcrEngine ocrEngine;
    QuickDict::instance()->setOcrEngine(&ocrEngine);

    MonitorService monitorService;
    QuickDict::instance()->setMonitorService(&monitorService);

    ClipboardMonitor clipboardMonitor;
    monitorService.registerMonitor(&clipboardMonitor);
    QHotkey clipboardMonitorHotkey(QKeySequence("Alt+Q"), true, &app);
    if (!clipboardMonitorHotkey.isRegistered())
        qCWarning(qd) << QString("Register ClipboardMonitor shortcut %1 failed")
                             .arg(clipboardMonitorHotkey.shortcut().toString());
    QObject::connect(&clipboardMonitorHotkey, &QHotkey::activated, qApp, [&clipboardMonitor]() {
        clipboardMonitor.toggle();
        qCInfo(qd) << "ClipboardMonitor: " << clipboardMonitor.isEnabled();
    });
    clipboardMonitor.setEnabled();

    MouseOverMonitor mouseOverMonitor;
    monitorService.registerMonitor(&mouseOverMonitor);
    QHotkey mouseOverMonitorHotkey(QKeySequence("Alt+O"), true, &app);
    if (!mouseOverMonitorHotkey.isRegistered())
        qCWarning(qd) << QString("Register MouseOverMonitor shortcut %1 failed")
                             .arg(mouseOverMonitorHotkey.shortcut().toString());
    QObject::connect(&mouseOverMonitorHotkey, &QHotkey::activated, qApp, [&mouseOverMonitor]() {
        mouseOverMonitor.toggle();
        qCInfo(qd) << "MouseOverMonitor: " << mouseOverMonitor.isEnabled();
    });

    DictService dictService;
    QuickDict::instance()->setDictService(&dictService);

    QObject::connect(&monitorService, &MonitorService::query, &dictService, &DictService::query);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/home.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    // register `qd` in js engine
    engine.rootContext()->setContextProperty("qd", QuickDict::instance());
    // register `setTimeout` in js engine
    engine.rootContext()->setContextObject(QuickDict::instance());
    engine.load(url);
    QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().at(0));
#if ENABLE_KWIN_BLUR
    // window->setProperty("color", "transparent");
    KWindowEffects::enableBlurBehind(window);
#endif
    window->show();

    int ret = app.exec();

    ocrEngine.stop();
    logFile.close();

    return ret;
}
