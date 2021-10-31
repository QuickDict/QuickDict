#ifndef QUICKDICT_H
#define QUICKDICT_H

#include <QLoggingCategory>
#include <QObject>

class OcrEngine;
class ConfigCenter;
class MonitorService;
class DictService;

class QuickDict : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *ocrEngine READ ocrEngine CONSTANT);
    Q_PROPERTY(QObject *configCenter READ configCenter CONSTANT);
    Q_PROPERTY(QObject *monitorService READ monitorService CONSTANT);
    Q_PROPERTY(QObject *dictService READ dictService CONSTANT);

public:
    explicit QuickDict(QObject *parent = nullptr);
    ~QuickDict();

    static QuickDict *instance() { return _instance; }

    inline OcrEngine *ocrEngine() const { return m_ocrEngine; }
    inline void setOcrEngine(OcrEngine *ocrEngine) { m_ocrEngine = ocrEngine; }
    inline ConfigCenter *configCenter() const { return m_configCenter; }
    inline void setConfigCenter(ConfigCenter *configCenter) { m_configCenter = configCenter; }
    inline MonitorService *monitorService() const { return m_monitorService; }
    inline void setMonitorService(MonitorService *monitorService) { m_monitorService = monitorService; }
    inline DictService *dictService() const { return m_dictService; }
    inline void setDictService(DictService *dictService) { m_dictService = dictService; }

    Q_INVOKABLE void setTimeout(const QVariant &function, int delay = 0);

private:
    static QuickDict *_instance;
    OcrEngine *m_ocrEngine = nullptr;
    ConfigCenter *m_configCenter = nullptr;
    MonitorService *m_monitorService = nullptr;
    DictService *m_dictService = nullptr;
};

Q_DECLARE_LOGGING_CATEGORY(qd)

#endif // QUICKDICT_H
