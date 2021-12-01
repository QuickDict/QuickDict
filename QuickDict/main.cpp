#include "clipboardmonitor.h"
#include "configcenter.h"
#include "dictservice.h"
#include "hotkey.h"
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
#include <QTimer>
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
    qmlRegisterType<MonitorService>("com.quickdict.components", 1, 0, "Monitor");
    qmlRegisterType<DictService>("com.quickdict.components", 1, 0, "Dict");
    qmlRegisterType<Hotkey>("com.quickdict.components", 1, 0, "Hotkey");

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
    QuickDict *quickDict = QuickDict::instance();
    quickDict->setUiScale(1.5);

    dir = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
    if (!dir.exists(app.applicationName()) && !dir.mkdir(app.applicationName()))
        qCWarning(qd) << "Cannot make dir:" << dir.absoluteFilePath(app.applicationName());
    dir.cd(app.applicationName());
    ConfigCenter configCenter(dir.absoluteFilePath("settings.ini"));
    quickDict->setConfigCenter(&configCenter);

    OcrEngine ocrEngine;
    quickDict->setOcrEngine(&ocrEngine);

    ClipboardMonitor clipboardMonitor(quickDict);
    quickDict->registerMonitor(&clipboardMonitor);

    MouseOverMonitor mouseOverMonitor(quickDict);
    quickDict->registerMonitor(&mouseOverMonitor);

    QQmlApplicationEngine engine(quickDict);
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
    engine.rootContext()->setContextProperty("qd", quickDict);
    // register `setTimeout` in js engine
    engine.rootContext()->setContextObject(quickDict);
    engine.load(url);

    QTimer::singleShot(200, quickDict, [quickDict]() { quickDict->loadConfig(); });

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
