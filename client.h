#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "mqtt.h"

class Client : QObject
{
    Q_OBJECT

public:
    Client();

private:
    Mqtt *pMqtt = nullptr;
};

#endif // CLIENT_H
