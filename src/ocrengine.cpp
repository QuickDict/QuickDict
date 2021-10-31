#include "ocrengine.h"
#include "configcenter.h"
#include "quickdict.h"
#include <tesseract/baseapi.h>
#include <QMutexLocker>
#include <QThread>

Q_LOGGING_CATEGORY(ocrEngine, "qd.ocr.engine")

OcrEngine::OcrEngine()
    : m_tessApi(new tesseract::TessBaseAPI)
    , m_ocrWorker(new OcrWorker(m_tessApi))
{
    m_ocrWorker->moveToThread(&m_workerThread); // OcrWorker cannot have a parent
    connect(this, &OcrEngine::extractText, m_ocrWorker, &OcrWorker::doExtractText);
    connect(m_ocrWorker, &OcrWorker::extractTextResult, this, &OcrEngine::extractTextResult);
}

OcrEngine::~OcrEngine()
{
    if (isRunning()) {
        m_workerThread.quit();
        m_workerThread.wait(200);
    }
    {
        QMutexLocker locker(&m_mutex);
        m_ocrWorker->deleteLater();
        m_tessApi->End();
        delete m_tessApi;
        m_tessApi = nullptr;
    }
}

void OcrEngine::start()
{
    if (!isRunning()) {
        QMutexLocker locker(&m_mutex);

        QSettings *settings = QuickDict::instance()->configCenter()->settings();
        settings->beginGroup("Tesseract");
        QString dataPath = settings->value("DataPath").toString();
        QString lang = settings->value("Language", "eng").toString();
        settings->endGroup();
        if (m_tessApi->Init(dataPath.isNull() ? nullptr : dataPath.toStdString().c_str(), lang.toStdString().c_str())) {
            qCCritical(ocrEngine) << "Could not initialize tesseract.";
            return;
        }

        m_workerThread.start();
        qCInfo(ocrEngine) << "OcrEngine started...";
        emit started();
    }
}

void OcrEngine::stop()
{
    if (isRunning()) {
        QMutexLocker locker(&m_mutex);

        m_tessApi->End();

        m_workerThread.quit();
        m_workerThread.wait(200);
        qCInfo(ocrEngine) << "OcrEngine stopped...";
        emit stopped();
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
