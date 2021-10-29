#include "ocrengine.h"
#include "configcenter.h"
#include "quickdict.h"
#include <cmath>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <QBuffer>
#include <QImage>
#include <QMutexLocker>
#include <QThread>
#include <QTime>

Q_LOGGING_CATEGORY(ocrEngine, "qd.ocr.engine")

OcrEngine::OcrEngine()
    : m_tessApi(new tesseract::TessBaseAPI)
{
    connect(this, &OcrEngine::start, this, &OcrEngine::doStart);
    connect(this, &OcrEngine::stop, this, &OcrEngine::doStop);
    connect(this, &OcrEngine::extractText, this, &OcrEngine::doExtractText);
}

OcrEngine::~OcrEngine()
{
    if (isRunning()) {
        moveToThread(m_workerThread.thread());
        m_workerThread.quit();
        m_workerThread.wait(200);
    }
    {
        QMutexLocker locker(&m_mutex);
        m_tessApi->End();
        delete m_tessApi;
        m_tessApi = nullptr;
    }
}

void OcrEngine::doStart()
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
        moveToThread(&m_workerThread); // OcrEngine cannot have a parent
    }
    qCInfo(ocrEngine) << "OcrEngine started...";
}

void OcrEngine::doStop()
{
    if (isRunning()) {
        QMutexLocker locker(&m_mutex);

        m_tessApi->End();

        moveToThread(m_workerThread.thread());
        m_workerThread.quit();
        m_workerThread.wait(200);
    }
    qCInfo(ocrEngine) << "OcrEngine stopped...";
}

bool OcrEngine::isRunning() const
{
    return m_workerThread.isRunning();
}

void OcrEngine::doExtractText(const QImage &image, const QPoint &p, int id)
{
    if (!isRunning())
        return;

    setImage(image);

    auto psmBackup = m_tessApi->GetPageSegMode();
    Pixa *pixa;
    Boxa *boxa = m_tessApi->GetWords(&pixa);
    int count = boxaGetCount(boxa);
    int x = p.x(), y = p.y();
    QString text;
    QRect rect;
    QList<QRect> rects;
    Box *box;
    QRect r;
    for (int i = 0; i < count; ++i) {
        box = boxaGetBox(boxa, i, L_CLONE);
        rects.append(QRect(box->x, box->y, box->w, box->h));
        if (!rect.isValid() && box->x <= x && box->y <= y && box->x + box->w > x && box->y + box->h > y) {
            r = QRect(box->x, box->y, box->w, box->h);
            if (r != image.rect()) {
                rect = r;
            }
        }
        boxDestroy(&box);
    }

    pixaDestroy(&pixa);
    boxaDestroy(&boxa);
    m_tessApi->Clear();
    m_tessApi->SetPageSegMode(psmBackup);

    if (rect.isValid()) {
        m_tessApi->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
        int border = std::ceil((float) rect.height() / 4.0);
        int x = std::max(rect.x() - border, 0);
        int y = std::max(rect.y() - border, 0);
        int w = std::min(rect.width() + 2 * border, image.width());
        int h = std::min(rect.height() + 2 * border, image.height());
        m_tessApi->SetRectangle(x, y, w, h);

        char *result = m_tessApi->GetUTF8Text();
        text = QString::fromUtf8(result);
        text.remove(QRegExp("[ \t\n]"));
        int index = std::round((p.x() - rect.left()) * (text.size() - 1) * 1.0 / (rect.right() - rect.left()));
        qCDebug(ocrEngine) << "text: " << text << " index: " << index << "char: " << QString(text[index]);
        delete[] result;
    }

    OcrResult result;
    result.text = text;
    result.rect = rect;
    result.rects = rects;
}

void OcrEngine::setImage(const QImage &image)
{
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    image.save(&buf, "PNG");

    Pix *img = pixReadMemPng((l_uint8 *) bytes.constData(), bytes.size());
    m_tessApi->SetImage(img);
    pixDestroy(&img);
}
