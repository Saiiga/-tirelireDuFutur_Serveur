/**
 * \file client.cpp
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe general pour le serveur et du client pour MQTT
 */
#include "client.h"

/**
 * @brief Client::Client
 * @details contructeur par default
 */
Client::Client()
{
    pMqtt = new Mqtt("broker.emqx.io", "testJsonParse", "testJsonParse", this);
    pJson = new Json(pMqtt, this);
    //signal pour un message recu envoie vers le parseJson
    connect(pMqtt,SIGNAL(messageResult(const QByteArray)), pJson ,SLOT(parseJson(const QByteArray)));
}

/**
 * @brief Client::~Client
 * @details destructeur
 */
Client::~Client()
{
    delete pMqtt;
    delete pJson;
}
