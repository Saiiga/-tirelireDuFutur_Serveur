/**
 * \file mqtt.h
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe du traitement des parametres MQTT
 */
#include "mqtt.h"

/**
 * @brief Mqtt::Mqtt
 * @param parent
 * @details constructeur par default
 */
Mqtt::Mqtt(const QString _hostName, const QByteArray _topicSubscribe, const QByteArray _topicPublish, QObject *parent) : QObject(parent)
{
    hostName = _hostName;
    topicSubscribe = _topicSubscribe;
    topicPublish = _topicPublish;
    client = new QMqttClient();
    //param MQTT
    client->setHostname(hostName);
    client->setPort(1883);
    //signaux
    QObject::connect(client, SIGNAL(connected()), this, SLOT(connect()));
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::connect(client, SIGNAL(messageReceived(const QByteArray &, const QMqttTopicName &)), this, SLOT(messageReceive(const QByteArray &, const QMqttTopicName &)));
    client->connectToHost();
}

/**
 * @brief Mqtt::~Mqtt
 * @details destructeur
 */
Mqtt::~Mqtt()
{
    client->disconnectFromHost();
    delete client;
    delete topic;
}

/**
 * @brief Mqtt::connect
 * @details connexion au MQTT
 */
void Mqtt::connect()
{
    topic = client->subscribe(QLatin1String(topicSubscribe));
    if (!topic)
    {
        qDebug() << "Erreur" << "Impossible de s'abonner !";
        return;
    }
}

/**
 * @brief Mqtt::disconnect
 * @details deconnection au MQTT
 */
void Mqtt::disconnect()
{
    qDebug() << "Déconnexion du broker";
}

/**
 * @brief Mqtt::sendMessage
 * @param mess
 * @details envoie message vers le serveur MQTT
 */
void Mqtt::sendMessage(const QByteArray mess)
{
    quint8 qos = 1;
    client->publish(QLatin1String(topicPublish), mess, qos);
}

/**
 * @brief Mqtt::messageReceive
 * @param message
 * @param topic
 * @details message recu du serveur MQTT
 */
void Mqtt::messageReceive(const QByteArray &message, const QMqttTopicName &topic)
{
    // qDebug() << QDateTime::currentDateTime().toString() << topic.name() << message;
    emit messageResult(message);
}
