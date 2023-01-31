/**
 * \file client.h
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe general pour le serveur et du client pour MQTT
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include "mqtt.h"
#include "json.h"

/**
 * @brief The Client class
 */
class Client : public QObject
{
    Q_OBJECT

public:
    Client();
    ~Client();

private:
    Mqtt *pMqtt = nullptr;
    Json *pJson = nullptr;
};

#endif // CLIENT_H
