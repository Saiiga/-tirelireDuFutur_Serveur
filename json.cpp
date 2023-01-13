#include "json.h"

Json::Json(QObject *parent) : QObject(parent)
{

}

Json::~Json()
{

}

void Json::parseJson(QByteArray mess)
{
    qDebug() << "mess : " << mess;
    //QJsonObject json = QJsonDocument::fromJson(mess);
}
