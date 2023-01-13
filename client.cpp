#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    pMqtt = new Mqtt();
    pJson = new Json();
    connect(pMqtt,SIGNAL(messageResult(QByteArray)), pJson ,SLOT(parseJson(QByteArray)));

}

Client::~Client()
{

}
