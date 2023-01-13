#ifndef MQTT_H
#define MQTT_H

#include <QtMqtt/QtMqtt>
#include <QtMqtt/QMqttClient>
#include <QDebug>
#include <QObject>

/**
 * @brief The Mqtt class
 */
class Mqtt : QObject
{
    Q_OBJECT

public:
    Mqtt(QObject *parent = 0);
    ~Mqtt();

private:
    QMqttClient *client = nullptr;
    QMqttSubscription *topic = nullptr;

private slots:
    void connect();
    void disconnect();
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic);
};

#endif // MQTT_H
