#include "ocrengine.h"
#include "configcenter.h"
#include "quickdict.h"
#include <tesseract/baseapi.h>
#include <QDir>
#include <QMutexLocker>
#include <QThread>

Q_LOGGING_CATEGORY(qdOcrEngine, "qd.ocr.engine")

OcrEngine::OcrEngine(QObject *parent)
    : QObject(parent)
    , m_tessApi(new tesseract::TessBaseAPI)
    , m_ocrWorker(new OcrWorker(m_tessApi))
{
    m_ocrWorker->moveToThread(&m_workerThread); // OcrWorker cannot have a parent
    connect(this, &OcrEngine::extractText, m_ocrWorker, &OcrWorker::doExtractText);
    connect(m_ocrWorker, &OcrWorker::extractTextResult, this, &OcrEngine::extractTextResult);
    connect(&m_workerThread, &QThread::started, this, [this]() {
        qCInfo(qdOcrEngine) << "OcrEngine started...";
        emit started();
    });
    connect(&m_workerThread, &QThread::finished, this, [this]() {
        m_tessApi->End();
        qCInfo(qdOcrEngine) << "OcrEngine stopped...";
        emit stopped();
    });
}

OcrEngine::~OcrEngine()
{
    if (isRunning()) {
        m_workerThread.quit();
        m_workerThread.wait(200);
        m_workerThread.terminate();
    }
    delete m_ocrWorker;
    m_tessApi->End();
    delete m_tessApi;
    m_tessApi = nullptr;
}

void OcrEngine::start()
{
    if (!isRunning()) {
        QString dataPath = QDir(QuickDict::dataDirPath()).filePath("tessdata");
        QSettings *settings = QuickDict::instance()->configCenter()->settings();
        settings->beginGroup("Tesseract");
        QString lang = settings->value("Language", "eng").toString();
        settings->endGroup();
        if (m_tessApi->Init(dataPath.isNull() ? nullptr : dataPath.toStdString().c_str(), lang.toStdString().c_str())) {
            qCCritical(qdOcrEngine) << "Could not initialize tesseract.";
            return;
        }

        m_workerThread.start();
    }
}

void OcrEngine::stop()
{
    if (isRunning()) {
        m_workerThread.quit();
    }
}

void OcrEngine::toggle()
{
    if (isRunning())
        stop();
    else
        start();
}

bool OcrEngine::isRunning() const
{
    return m_workerThread.isRunning();
}
