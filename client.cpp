#include "client.h"

Client::Client()
{
    pMqtt = new Mqtt(this);
    pJson = new Json(this);
    connect(pMqtt,SIGNAL(messageResult(QByteArray)), pJson ,SLOT(parseJson(QByteArray)));

}

Client::~Client()
{
    delete pMqtt;
    delete pJson;
}
