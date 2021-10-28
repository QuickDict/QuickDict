#include "configcenter.h"
#include "mainwindow.h"
#include "mouseovermonitor.h"
#include "ocrengine.h"

#include <QApplication>
#include <QDir>
#include <QHotkey>
#include <QLoggingCategory>
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
    QApplication a(argc, argv);
    a.setOrganizationDomain("https://github.com/QuickDict/QuickDict");
    a.setApplicationName("QuickDict");

    QLoggingCategory qd("qd.main");

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
    qInstallMessageHandler(messageHandler);

    dir = QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    dir.mkdir(a.applicationName());
    dir.cd(a.applicationName());
    ConfigCenter::setConfigFile(dir.absoluteFilePath("settings.ini"));

    OcrEngine::createInstance();
    OcrEngine::instance()->start();
    MouseOverMonitor monitor;
    auto hotkey = new QHotkey(QKeySequence("Alt+Q"), true, &a);
    qCDebug(qd) << "Is Registered: " << hotkey->isRegistered();
    QObject::connect(hotkey, &QHotkey::activated, qApp, [&]() {
        monitor.toggle();
        qCDebug(qd) << "MouseOverMonitor: " << monitor.isEnabled();
    });

    int ret = a.exec();
    logFile.close();
    return ret;
}
