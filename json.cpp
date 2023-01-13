#include "json.h"

Json::Json(QObject *parent) : QObject(parent)
{

}

Json::~Json()
{

}

void Json::parseJson(QByteArray mess)
{
    qDebug() << "message recu : " << mess;
    QJsonObject json = QJsonDocument::fromJson(mess).object();
    qDebug() << "json : " << json;
    if(json.contains("server"))
    {
        qDebug()<<"il y a action ";
    }
    else qDebug() << "Json::parseJson - " << "key requestHistory unknown";
}
