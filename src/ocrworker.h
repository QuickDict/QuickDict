#ifndef OCRWORKER_H
#define OCRWORKER_H

#include <QLoggingCategory>
#include <QObject>
#include <QRect>

namespace tesseract {
class TessBaseAPI;
};

struct OcrResult
{
    QString text;
    QRect rect;
    QList<QRect> rects;
};

class OcrWorker : public QObject
{
    Q_OBJECT
public:
    explicit OcrWorker(tesseract::TessBaseAPI *tessApi, QObject *parent = nullptr);
    ~OcrWorker();

public Q_SLOTS:
    void doExtractText(const QImage &image, const QPoint &p, int id = 0);

Q_SIGNALS:
    void extractTextResult(const OcrResult &result);

private:
    void setImage(const QImage &image);

    tesseract::TessBaseAPI *m_tessApi;
};

Q_DECLARE_LOGGING_CATEGORY(ocrWorker)

#endif // OCRWORKER_H
