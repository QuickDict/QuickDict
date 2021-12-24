#ifndef DICTSERVICE_H
#define DICTSERVICE_H

#include "service.h"
#include <QJsonObject>
#include <QLoggingCategory>
#include <QQmlParserStatus>

class QQmlComponent;

class DictService : public Service, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
public:
    explicit DictService(QObject *parent = nullptr);
    virtual ~DictService();

    QQmlComponent *delegate() const { return m_delegate; }
    void setDelegate(QQmlComponent *delegate);

    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void delegateChanged();
    void query(const QString &text);
    void queryResult(const QJsonObject &result);

private:
    QQmlComponent *m_delegate = nullptr;
};

Q_DECLARE_LOGGING_CATEGORY(qdDict)

#endif // DICTSERVICE_H
