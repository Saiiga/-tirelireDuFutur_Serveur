#include "mqtt.h"

Mqtt::Mqtt(QObject *parent) : QObject(parent)
{

    client = new QMqttClient();
    client->setHostname("broker.emqx.io");
    client->setPort(1883);
    //signaux
    QObject::connect(client, SIGNAL(connected()), this, SLOT(connect()));
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(disconnect()));
    QObject::connect(client, SIGNAL(messageReceived(const QByteArray &, const QMqttTopicName &)), this, SLOT(messageReceived(const QByteArray &, const QMqttTopicName &)));

    client->connectToHost();
}

Mqtt::~Mqtt()
{
    client->disconnectFromHost();
}

void Mqtt::sendMessage(QByteArray mess)
{
    quint8 qos = 1;
    client->publish(QLatin1String("prout"), mess, qos);
}

void Mqtt::connect()
{
    topic = client->subscribe(QLatin1String("esp32lotfihihi"));
    if (!topic)
    {
        qDebug() << "Erreur", "Impossible de s'abonner !";
        return;
    }
}

void Mqtt::disconnect()
{
    qDebug() << "Déconnexion du broker";
}

QByteArray Mqtt::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug() << QDateTime::currentDateTime().toString() << topic.name() << message;
    emit messageResult(message);
}
