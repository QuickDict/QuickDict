#ifndef OCRENGINE_H
#define OCRENGINE_H

#include "ocrworker.h"

#include <QLoggingCategory>
#include <QMutex>
#include <QThread>

namespace tesseract {
class TessBaseAPI;
};

class OcrWorker;

class OcrEngine : public QObject
{
    Q_OBJECT
public:
    explicit OcrEngine(QObject *parent = nullptr);
    ~OcrEngine();

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void toggle();
    Q_INVOKABLE bool isRunning() const;

Q_SIGNALS:
    void started();
    void stopped();

    void extractText(const QImage &image, const QPoint &p, int id = 0);
    void extractTextResult(const OcrResult &result);

private:
    tesseract::TessBaseAPI *m_tessApi = nullptr;
    OcrWorker *m_ocrWorker = nullptr;
    QThread m_workerThread;
};

Q_DECLARE_LOGGING_CATEGORY(ocrEngine)

#endif // OCRENGINE_H
