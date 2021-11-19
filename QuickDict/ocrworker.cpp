#include "ocrworker.h"
#include <cmath>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <QBuffer>
#include <QImage>

Q_LOGGING_CATEGORY(ocrWorker, "qd.ocr.worker")

OcrWorker::OcrWorker(tesseract::TessBaseAPI *tessApi, QObject *parent)
    : QObject(parent)
    , m_tessApi(tessApi)
{}

OcrWorker::~OcrWorker() {}

void OcrWorker::doExtractText(const QImage &image, const QPoint &p, int id)
{
    auto psmBackup = m_tessApi->GetPageSegMode();
    setImage(image);
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
        qCDebug(ocrWorker) << "text: " << text << " index: " << index << "char: " << QString(text[index]);
        delete[] result;
    }

    pixaDestroy(&pixa);
    boxaDestroy(&boxa);
    m_tessApi->Clear();
    m_tessApi->SetPageSegMode(psmBackup);

    OcrResult result;
    result.text = text;
    result.rect = rect;
    result.rects = rects;
    emit extractTextResult(result);
}

void OcrWorker::setImage(const QImage &image)
{
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    image.save(&buf, "PNG");

    Pix *img = pixReadMemPng((l_uint8 *) bytes.constData(), bytes.size());
    m_tessApi->SetImage(img);
    pixDestroy(&img);
}
