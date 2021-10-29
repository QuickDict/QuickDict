#ifndef QUICKDICT_H
#define QUICKDICT_H

#include <QObject>

class OcrEngine;
class ConfigCenter;

class QuickDict : public QObject
{
    Q_OBJECT
public:
    explicit QuickDict(QObject *parent = nullptr);
    ~QuickDict();

    static QuickDict *instance() { return _instance; }

    inline OcrEngine *ocrEngine() const { return m_ocrEngine; }
    inline void setOcrEngine(OcrEngine *ocrEngine) { m_ocrEngine = ocrEngine; }
    inline ConfigCenter *configCenter() const { return m_configCenter; }
    inline void setConfigCenter(ConfigCenter *configCenter) { m_configCenter = configCenter; }

private:
    static QuickDict *_instance;
    OcrEngine *m_ocrEngine = nullptr;
    ConfigCenter *m_configCenter = nullptr;
};

#endif // QUICKDICT_H
