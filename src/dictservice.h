#ifndef DICTSERVICE_H
#define DICTSERVICE_H

#include <QObject>

class DictInterface;

class DictService : public QObject
{
    Q_OBJECT
public:
    explicit DictService(QObject *parent = nullptr);
    virtual ~DictService();

    void registerDict(DictInterface *dict);

private:
    QList<DictInterface *> m_dicts;
};

#endif // DICTSERVICE_H
