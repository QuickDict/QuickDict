#ifndef OCRENGINE_H
#define OCRENGINE_H

#include <QLoggingCategory>
#include <QMutex>
#include <QObject>
#include <QRect>
#include <QThread>

namespace tesseract {
class TessBaseAPI;
};

struct OcrResult
{
    QString text;
    QRect rect;
    QList<QRect> rects;
};

class OcrEngine : public QObject
{
    Q_OBJECT
public:
    OcrEngine();
    ~OcrEngine();

    void doStart();
    void doStop();
    bool isRunning() const;

    void doExtractText(const QImage &image, const QPoint &p, int id = 0);

Q_SIGNALS:
    void start();
    void stop();

    void extractText(const QImage &image, const QPoint &p, int id = 0);
    OcrResult extractTextResult(const OcrResult &result);

private:
    void setImage(const QImage &image);

    tesseract::TessBaseAPI *m_tessApi = nullptr;
    QThread m_workerThread;
    QMutex m_mutex;
};

Q_DECLARE_LOGGING_CATEGORY(ocrEngine)

#endif // OCRENGINE_H
