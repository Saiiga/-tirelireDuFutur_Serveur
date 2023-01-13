#ifndef JSON_H
#define JSON_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class Json : public QObject
{
    Q_OBJECT

public:
    Json(QObject *parent = 0);
    ~Json();

public slots:
    void parseJson(QByteArray mess);
};

#endif // JSON_H
