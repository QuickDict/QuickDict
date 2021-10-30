#ifndef QUICKDICT_H
#define QUICKDICT_H

#include <QLoggingCategory>
#include <QObject>

class OcrEngine;
class ConfigCenter;

class QuickDict : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *ocrEngine READ ocrEngine);
    Q_PROPERTY(QObject *configCenter READ configCenter);

public:
    explicit QuickDict(QObject *parent = nullptr);
    ~QuickDict();

    static QuickDict *instance() { return _instance; }

    inline OcrEngine *ocrEngine() const { return m_ocrEngine; }
    inline void setOcrEngine(OcrEngine *ocrEngine) { m_ocrEngine = ocrEngine; }
    inline ConfigCenter *configCenter() const { return m_configCenter; }
    inline void setConfigCenter(ConfigCenter *configCenter) { m_configCenter = configCenter; }

    Q_INVOKABLE void setTimeout(const QVariant &function, int delay = 0);

private:
    static QuickDict *_instance;
    OcrEngine *m_ocrEngine = nullptr;
    ConfigCenter *m_configCenter = nullptr;
};

Q_DECLARE_LOGGING_CATEGORY(qd)

#endif // QUICKDICT_H
