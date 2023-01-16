/**
 * \file json.cpp
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe du traitement des données json recu
 */
#include "json.h"

/**
 * @brief Json::Json
 * @param myMqtt
 * @param parent
 * @details constructeur par default
 */
Json::Json(Mqtt *myMqtt, QObject *parent) : QObject(parent)
{
    pMqtt = myMqtt;
}

/**
 * @brief Json::~Json
 * @details destructeur
 */
Json::~Json()
{
    delete pMqtt;
}

/**
 * @brief Json::parseJson
 * @param mess
 * @details analyse le json
 */
void Json::parseJson(const QByteArray mess)
{
    //qDebug() << "mess recu : " << mess;
    QJsonObject json = QJsonDocument::fromJson(mess).object();
    if(!json.isEmpty())
    {
        if(json.contains("server"))
        {
            QJsonObject objServer = json["server"].toObject();
            //recupere clé action
            if (objServer.contains("action")) this->differentActions(objServer["action"].toString());
            else qDebug() << "Json::parseJson - " << "error there is no action key" ;
        }
        else qDebug() << "Json::parseJson - " << "key requestHistory unknown";
    }
    else qDebug() << "Json::parseJson - " << "error is no json";
}

/**
 * @brief Json::differentActions
 * @param actionValue
 * @details fonction pour traitrer la valeur de l'action recu
 */
void Json::differentActions(const QString actionValue)
{
    //different actions
    QStringList lstActionValue;
    lstActionValue << "withdrawn"
                    << "requestHistoryServer"
                    << "requestData"
                    << "answerWithdraw"
                    << "answerHistoryEsp32"
                    << "addMoney";
    switch (lstActionValue.indexOf(actionValue))
    {
    case 0:
    {
        qDebug() << "action is : " << actionValue;
        break;
    }
    case 1:
    {
        qDebug() << "action is : " << actionValue;
        break;
    }
    case 2:
    {
        qDebug() << "action is : " << actionValue;
        break;
    }
    case 3:
    {
        qDebug() << "action is : " << actionValue;
        break;
    }
    case 4:
    {
        qDebug() << "action is : " << actionValue;
        break;
    }
    case 5:
    {
        qDebug() << "action is : " << actionValue;
        break;
    }
    default:
    {
        qDebug() << "Json::parseJson - " << "key action unknown";
        break;
    }
    }
}
