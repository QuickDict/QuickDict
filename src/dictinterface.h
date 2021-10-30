#ifndef DICTINTERFACE_H
#define DICTINTERFACE_H

#include <QObject>
#include <QString>

class DictInterface : public QObject
{
    Q_OBJECT
public:
    explicit DictInterface(QObject *parent = nullptr);
    virtual ~DictInterface();
};

#endif // DICTINTERFACE_H
