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
    Mqtt(QObject *parent = 0);
    ~Mqtt();
    void sendMessage(QByteArray mess);

private:
    QMqttClient *client = nullptr;
    QMqttSubscription *topic = nullptr;

private slots:
    void connect();
    void disconnect();
    QByteArray messageReceived(const QByteArray &message, const QMqttTopicName &topic);

signals:
    void messageResult(QByteArray ress);
};

#endif // MQTT_H
