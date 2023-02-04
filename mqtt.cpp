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
    //récupere les parametres pour la connexion vers serveur MQTT
    hostName = _hostName;
    topicSubscribe = _topicSubscribe;
    topicPublish = _topicPublish;
    //instancie un client MQTT
    client = new QMqttClient();
    //param client MQTT
    client->setHostname(hostName);
    client->setPort(1883);
    //signaux
    QObject::connect(client, SIGNAL(connected()), this, SLOT(connect()));
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::connect(client, SIGNAL(messageReceived(const QByteArray &, const QMqttTopicName &)), this, SLOT(messageReceive(const QByteArray &, const QMqttTopicName &)));
    //connexion vers le serveur MQTT
    client->connectToHost();
}

/**
 * @brief Mqtt::~Mqtt
 * @details destructeur
 */
Mqtt::~Mqtt()
{
    //Deconnexion du serveur MQTT
    client->disconnectFromHost();
    //supprime les pointeurs
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
        qDebug() << "Mqtt::connect - " << "[Error] " << "Unable to subscribe!";
        return;
    }
}

/**
 * @brief Mqtt::disconnect
 * @details deconnection au MQTT
 */
void Mqtt::disconnect()
{
    qDebug() << "Mqtt::disconnect - " << "Broker disconnection";
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
     qDebug() << QDateTime::currentDateTime().toString() << topic.name() << message;
    // signal quand un message est recu
    emit messageResult(message);
}
