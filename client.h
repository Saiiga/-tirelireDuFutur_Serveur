#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "mqtt.h"
#include "json.h"

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject *parent = 0);
    ~Client();

private:
    Mqtt *pMqtt = nullptr;
    Json *pJson = nullptr;

};

#endif // CLIENT_H
