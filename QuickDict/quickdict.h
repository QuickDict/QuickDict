#ifndef QUICKDICT_H
#define QUICKDICT_H

#include "service.h"
#include <QJsonObject>
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
    Q_PROPERTY(QList<QObject *> monitors READ monitors NOTIFY monitorsChanged);
    Q_PROPERTY(QList<QObject *> dicts READ dicts NOTIFY dictsChanged);

    // convience properties
    Q_PROPERTY(QString sourceLanguage READ sourceLanguage WRITE setSourceLanguage NOTIFY sourceLanguageChanged);
    Q_PROPERTY(QString targetLanguage READ targetLanguage WRITE setTargetLanguage NOTIFY targetLanguageChanged);

    Q_PROPERTY(qreal dpScale READ dpScale WRITE setDpScale NOTIFY dpScaleChanged);
    Q_PROPERTY(qreal spScale READ spScale WRITE setSpScale NOTIFY spScaleChanged);
    Q_PROPERTY(qreal uiScale READ uiScale WRITE setUiScale NOTIFY uiScaleChanged);

public:
    explicit QuickDict(QObject *parent = nullptr);
    ~QuickDict();

    static void createInstance();
    static QuickDict *instance() { return _instance; }

    inline OcrEngine *ocrEngine() const { return m_ocrEngine; }
    inline void setOcrEngine(OcrEngine *ocrEngine) { m_ocrEngine = ocrEngine; }
    inline ConfigCenter *configCenter() const { return m_configCenter; }
    void setConfigCenter(ConfigCenter *configCenter);
    QString sourceLanguage() const;
    void setSourceLanguage(const QString &sourceLang);
    Q_SIGNAL void sourceLanguageChanged(const QString &sourceLang);
    QString targetLanguage() const;
    void setTargetLanguage(const QString &targetLang);
    Q_SIGNAL void targetLanguageChanged(const QString &targetLang);

    void loadConfig();

    Q_INVOKABLE void setTimeout(const QVariant &function, int delay = 0);
    Q_INVOKABLE qreal dp(qreal value) const;
    Q_INVOKABLE qreal sp(qreal value) const;

    qreal dpScale() const { return m_dpScale; }
    void setDpScale(qreal dpScale);
    Q_SIGNAL void dpScaleChanged();
    qreal spScale() const { return m_spScale; }
    void setSpScale(qreal spScale);
    Q_SIGNAL void spScaleChanged();
    qreal uiScale() const { return m_uiScale; }
    void setUiScale(qreal uiScale);
    Q_SIGNAL void uiScaleChanged();

    Q_INVOKABLE void registerMonitor(MonitorService *monitor);
    QList<QObject *> monitors() const;
    Q_INVOKABLE MonitorService *monitor(const QString &name) const;
    Q_INVOKABLE void registerDict(DictService *dict);
    QList<QObject *> dicts() const;
    Q_INVOKABLE DictService *dict(const QString &name) const;
    Q_INVOKABLE QStringList availableLocales() const;
    Q_INVOKABLE QObject *findChild(const QString &name, QObject *parent = nullptr) const;
    Q_INVOKABLE QRect textBoundingRect(const QFont &font, const QString &text) const;

Q_SIGNALS:
    void query(const QString &text);
    void queryResult(const QJsonObject &result);
    void monitorsChanged();
    void dictsChanged();

private Q_SLOTS:
    void onMonitorEnabledChanged(bool enabled);
    void onDictEnabledChanged(bool enabled);
    void onConfigChanged(const QString &key, const QVariant &value);

private:
    void handleMonitor(MonitorService *monitor, bool enabled);
    void handleDict(DictService *dict, bool enabled);

    static QuickDict *_instance;
    OcrEngine *m_ocrEngine = nullptr;
    ConfigCenter *m_configCenter = nullptr;
    MonitorService *m_monitorService = nullptr;
    DictService *m_dictService = nullptr;

    QList<MonitorService *> m_monitors;
    QList<DictService *> m_dicts;

    qreal m_dpScale = 1.0;
    qreal m_spScale = 1.0;
    qreal m_uiScale = 1.0;
    qreal m_pixelScale;
};

Q_DECLARE_LOGGING_CATEGORY(qd)

#endif // QUICKDICT_H
