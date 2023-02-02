/**
 * \file json.h
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe du traitement des données json recu
 */
#ifndef JSON_H
#define JSON_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "mqtt.h"

/**
 * @brief The Json class
 */
class Json : public QObject
{
    Q_OBJECT

public:
    Json(Mqtt *myMqtt, QObject *parent = 0);
    ~Json();

public slots:
    void parseJson(const QByteArray mess);

private:
    Mqtt *pMqtt = nullptr;
    void differentActions(const QJsonObject keyAction);
    void addMoney(const QJsonObject addMoney);
    void getAllData(const QJsonObject requestData);
    void answerWithdraw(const QJsonObject keyAction);
    void requestHistoryServer(const QJsonObject keyAction);
    //algo retrait
    void restatedAmountAlgo(const QJsonObject withdrawn);
    bool coinsInRack(const QList<double> usedCoins, const QList<double> usedTotalAddition, const double retiredAmount);
    //variable
    const QString docFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    //pieces dispo
    QList<double> coinsPending;
    QList<double> additionByCoinsPending;
};

#endif // JSON_H
