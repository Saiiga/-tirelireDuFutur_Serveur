/**
 * \file mqtt.h
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe du traitement des parametres MQTT
 */
#ifndef MQTT_H
#define MQTT_H

#include <QtMqtt/QtMqtt>
#include <QtMqtt/QMqttClient>
#include <QDebug>
#include <QObject>

/**
 * @brief The Mqtt class
 */
class Mqtt : public QObject
{
    Q_OBJECT

public:
    Mqtt(const QString _hostName, const QByteArray _topicSubscribe, const QByteArray _topicPublish, QObject *parent = 0);
    ~Mqtt();
    void sendMessage(const QByteArray mess);

private slots:
    void connect();
    void disconnect();
    void messageReceive(const QByteArray &message, const QMqttTopicName &topic);

signals:
    void messageResult(const QByteArray ress);

private:
    QMqttClient *client = nullptr;
    QMqttSubscription *topic = nullptr;
    QString hostName;
    QByteArray topicSubscribe;
    QByteArray topicPublish;
};

#endif // MQTT_H
