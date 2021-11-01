#include "clipboardmonitor.h"
#include "configcenter.h"
#include "dictinterface.h"
#include "dictservice.h"
#include "mainwindow.h"
#include "monitorservice.h"
#include "mouseovermonitor.h"
#include "ocrengine.h"
#include "quickdict.h"

#include <QApplication>
#include <QDir>
#include <QHotkey>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>

static QFile logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    if (logFile.isOpen()) {
        logFile.write(qFormatLogMessage(type, context, message).toUtf8().append('\n'));
        logFile.flush();
    }
}

int main(int argc, char *argv[])
{
    qRegisterMetaType<OcrResult>("OcrResult");
    qmlRegisterType<DictInterface>("com.quickdict.components", 1, 0, "Dict");

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setOrganizationName("QuickDict");
    app.setOrganizationDomain("https://github.com/QuickDict/QuickDict");
    app.setApplicationName("QuickDict");

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    if (!dir.mkpath(dir.absolutePath()))
        qCWarning(qd) << "Cannot make path:" << dir.absolutePath();
    logFile.setFileName(dir.absoluteFilePath("log"));
    if (!logFile.open(QIODevice::Append | QIODevice::Text))
        qCWarning(qd) << "Cannot open file:" << logFile.fileName();

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
    // qInstallMessageHandler(messageHandler);

    dir = QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    dir.mkdir(app.applicationName());
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
    qCDebug(qd) << "Is Registered: " << clipboardMonitorHotkey.isRegistered();
    QObject::connect(&clipboardMonitorHotkey, &QHotkey::activated, qApp, [&clipboardMonitor]() {
        clipboardMonitor.toggle();
        qCInfo(qd) << "ClipboardMonitor: " << clipboardMonitor.isEnabled();
    });
    clipboardMonitor.setEnabled();

    MouseOverMonitor mouseOverMonitor;
    monitorService.registerMonitor(&mouseOverMonitor);
    QHotkey mouseOverMonitorHotkey(QKeySequence("Alt+O"), true, &app);
    qCDebug(qd) << "Is Registered: " << mouseOverMonitorHotkey.isRegistered();
    QObject::connect(&mouseOverMonitorHotkey, &QHotkey::activated, qApp, [&mouseOverMonitor, &ocrEngine]() {
        mouseOverMonitor.toggle();
        ocrEngine.toggle();
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

    int ret = app.exec();

    ocrEngine.stop();
    logFile.close();

    return ret;
}
