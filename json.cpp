/**
 * \file json.cpp
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe du traitement des données json recu
 */
#include "json.h"

/**
 * @brief Json::Json
 * @param myMqtt
 * @param parent
 * @details constructeur par default
 */
Json::Json(Mqtt *myMqtt, QObject *parent) : QObject(parent)
{
    pMqtt = myMqtt;
}

/**
 * @brief Json::~Json
 * @details destructeur
 */
Json::~Json()
{
    delete pMqtt;
}

/**
 * @brief Json::parseJson
 * @param mess
 * @details analyse le json
 */
void Json::parseJson(const QByteArray mess)
{
    // qDebug() << "mess recu : " << mess;
    QJsonParseError parseError;
    QJsonObject json = QJsonDocument::fromJson(mess, &parseError).object();
    if(parseError.error == QJsonParseError::NoError)
    {
        if(json.contains("server"))
        {
            QJsonObject objServer = json["server"].toObject();
            //recupere clé action
            if (objServer.contains("action")) this->differentActions(objServer);
            else qDebug() << "Json::parseJson - " << "there is no action key" ;
        }
        else if(json.contains("mobileApp")){}
        else if(json.contains("esp32")){}
        else qDebug() << "Json::parseJson - " << "key requestHistory unknown";
    }
    else qDebug() << "Json::parseJson - " << "[Error] "
                 << "impossible to interpret the received json because :" << parseError.errorString() ;   
}

/**
 * @brief Json::differentActions
 * @param actionValue
 * @details fonction pour traitrer la valeur de l'action recu
 */
void Json::differentActions(const QJsonObject keyAction)
{
    //different actions
    QStringList lstActionValue;
    lstActionValue << "withdrawn"
                    << "requestHistoryServer"
                    << "requestData"
                    << "answerWithdraw"
                    << "answerHistoryEsp32"
                    << "addMoney";
    switch (lstActionValue.indexOf(keyAction["action"].toString()))
    {
    case 0:
    {
        this->restatedAmountAlgo(keyAction);
        break;
    }
    case 1:
    {
        this->requestHistoryServer(keyAction);
        break;
    }
    case 2:
    {
        this->getAllData(keyAction);
        break;
    }
    case 3:
    {
        this->answerWithdraw(keyAction);
        break;
    }
    case 4:
    {
        qDebug() << "action is : " << keyAction;
        break;
    }
    case 5:
    {
        this->addMoney(keyAction);
        break;
    }
    default:
    {
        qDebug() << "Json::parseJson - " << "key action unknown";
        break;
    }
    }
}

/**
 * @brief Json::addMoney
 * @param addMoney
 * @details ajoute money
 */
void Json::addMoney(const QJsonObject addMoneyObj)
{
    if (addMoneyObj.contains("coins"))
    {
        //ajout fichier money total
        QJsonArray coinsReceved = addMoneyObj["coins"].toArray();
        QFile historyFile(docFolder + "/json/totalPieces.json");
        if (historyFile.open(QIODevice::ReadOnly))
        {
            QByteArray fileData = historyFile.readAll();
            historyFile.close();
            //json
            QJsonParseError parseError;
            QJsonObject jsonFileData = QJsonDocument::fromJson(fileData, &parseError).object();
            if (parseError.error == QJsonParseError::NoError)
            {
                QJsonArray coinsArray = jsonFileData["coins"].toArray();
                //insere les nouvelles données
                for (int i = 0; i < coinsReceved.size(); i++)
                {
                    QJsonObject objCoinsReceved = coinsReceved[i].toObject();
                    int index = 0;
                    foreach (const QJsonValue &valueFile, coinsArray)
                    {
                        QJsonObject objCoins = valueFile.toObject();
                        if (objCoinsReceved["type"].toDouble() == objCoins["type"].toDouble())
                        {
                            QJsonObject newObject;
                            int number = objCoinsReceved["number"].toInt()
                                         + objCoins["number"].toInt();
                            double totalAddition = number * objCoinsReceved["type"].toDouble();
                            //insert dans l'object
                            newObject.insert("type", objCoinsReceved["type"].toDouble());
                            newObject.insert("number", number);
                            newObject.insert("totalAddition", totalAddition);
                            //remplace les valeurs
                            coinsArray.replace(index, newObject);
                        }
                        index++;
                    }
                }
                //calcule total
                double numberTotal = 0.00;
                foreach(const QJsonValue &value, coinsArray) numberTotal += value["totalAddition"].toDouble();
                //ajout nombre total
                jsonFileData.remove("numberTotal");
                jsonFileData.insert("numberTotal", QJsonValue::fromVariant(numberTotal));
                //ajout tableau coins
                jsonFileData.remove("coins");
                jsonFileData.insert("coins", QJsonValue::fromVariant(coinsArray));
                QJsonDocument doc;
                doc.setObject(jsonFileData);
                if (historyFile.open(QIODevice::WriteOnly))
                {
                    historyFile.write(doc.toJson(QJsonDocument::Indented));
                    historyFile.close();
                }
            }
            else qDebug() << "Json::parseJson - " << "[Error] impossible to interpret the received json because : "
                         << parseError.errorString();
        }
        //ajout log
        //ouvrir fichier
        QFile precise(docFolder + "/json/historical/precise.json");
        if(precise.open(QIODevice::ReadOnly))
        {
            QByteArray fileData = precise.readAll();
            precise.close();
            //file json
            QJsonParseError parseError;
            QJsonObject jsonFileData = QJsonDocument::fromJson(fileData, &parseError).object();
            if (parseError.error == QJsonParseError::NoError)
            {
                QJsonArray preciseFile = jsonFileData["precise"].toArray();
                //insere les nouvelles données
                QJsonObject newObject;
                //insert pieces dans l'object
                newObject.insert("coins", coinsReceved);
                //recuperer et insere la date
                if(addMoneyObj.contains("date")) newObject.insert("date", addMoneyObj["date"].toDouble());
                preciseFile.push_back(newObject);
                //remplace tab coins
                jsonFileData.remove("precise");
                jsonFileData.insert("precise", QJsonValue::fromVariant(preciseFile));
                //envoie de le fichier json
                QJsonDocument doc;
                doc.setObject(jsonFileData);
                if(precise.open(QIODevice::WriteOnly))
                {
                    precise.write(doc.toJson(QJsonDocument::Indented));
                    precise.close();
                }

                historyFile.close();
            }
        }
    }
    else qDebug() << "Json::differentActions" << "[Error] No open totalPiece file";
}

/**
 * @brief Json::getAllData
 * @param requestData
 * @details renvoie toutes les donnees disponible a partir d'une date
 */
void Json::getAllData(const QJsonObject requestData)
{
    if (requestData.contains("timestampLastUpdate"))
    {
        double timestamp = requestData["timestampLastUpdate"].toDouble();
        QFile totalPieceFile(docFolder + "/json/totalPieces.json");
        QFile preciseFile(docFolder + "/json/historical/precise.json");
        QFile meanFile(docFolder + "/json/historical/mean.json");
        QByteArray totalPieceData;
        QByteArray preciseData;
        QByteArray meanData;
        if (totalPieceFile.open(QIODevice::ReadOnly))
        {
            totalPieceData = totalPieceFile.readAll();
            totalPieceFile.close();
        }
        if (preciseFile.open(QIODevice::ReadOnly))
        {
            preciseData = preciseFile.readAll();
            preciseFile.close();
        }
        if (meanFile.open(QIODevice::ReadOnly))
        {
            meanData = meanFile.readAll();
            meanFile.close();
        }
        if ((!totalPieceData.isNull()) || (!preciseData.isNull()) || (!meanData.isNull()))
        {
            QJsonObject newPreciseObject;
            QJsonParseError parseError;
            QJsonObject preciseJson = QJsonDocument::fromJson(preciseData, &parseError).object();
            if (parseError.error == QJsonParseError::NoError)
            {
                QJsonArray preciseArray = preciseJson["precise"].toArray();
                QJsonArray newPreciseArray;
                foreach (const QJsonValue myPrecise, preciseArray)
                {
                    double dateOfPrecise = myPrecise["date"].toDouble();
                    if(timestamp <= dateOfPrecise) newPreciseArray.push_back(myPrecise);
                }
                newPreciseObject.insert("precise", newPreciseArray);
            }
            else qDebug() << "Json::parseJson - " << "[Error] impossible to interpret the received json because : "
                         << parseError.errorString();
            QJsonObject meanJson = QJsonDocument::fromJson(meanData).object();
            QJsonObject totalPieceJson = QJsonDocument::fromJson(totalPieceData).object();
            //appMobile
            QJsonObject mobileAppJson;
            //requete vers l'app :
            QJsonObject historyJson;
            historyJson.insert("mean",meanJson["mean"].toObject());
            historyJson.insert("precise", newPreciseObject["precise"].toArray());
            //ajout tout
            QJsonObject getAllDataJson;
            mobileAppJson.insert("coins",totalPieceJson["coins"].toArray());
            mobileAppJson.insert("history", historyJson);
            getAllDataJson.insert("mobileApp", mobileAppJson);
            //return qbyteArray
            QJsonDocument doc;
            doc.setObject(getAllDataJson);
            //envoie vers le MQTT
            pMqtt->sendMessage(doc.toJson(/*QJsonDocument::Compact*/QJsonDocument::Indented));
        }
        else qDebug() << "Json::parseJson - " << "[Error] file null " ;
    }
}

/**
 * @brief Json::answerWithdraw
 * @param keyAction
 * @details retire la monnaie du json
 */
void Json::answerWithdraw(const QJsonObject keyAction)
{
    if(keyAction.contains("status"))
    {
        bool status = keyAction["status"].toBool();
        if(status && !coinsPending.isEmpty() && !additionByCoinsPending.isEmpty())
        {
            QFile pieceFile(docFolder + "/json/totalPieces.json");
            if (pieceFile.open(QIODevice::ReadOnly))
            {
                QByteArray fileData = pieceFile.readAll();
                pieceFile.close();
                QJsonObject jsonFileData = QJsonDocument::fromJson(fileData).object();
                QJsonArray coinsArray = jsonFileData["coins"].toArray();
                for (int i = 0; i < coinsPending.size(); i++)
                {
                    int index = 0;
                    foreach (const QJsonValue &value, coinsArray)
                    {
                        QJsonObject objCoins = value.toObject();
                        if (coinsPending[i] == objCoins["type"].toDouble())
                        {
                            QJsonObject newObject;
                            int number = objCoins["number"].toInt() - additionByCoinsPending[i];
                            double totalAddition = number * coinsPending[i];
                            //insert dans l'object
                            newObject.insert("type", coinsPending[i]);
                            newObject.insert("number", number);
                            newObject.insert("totalAddition", totalAddition);
                            //remplace les valeurs
                            coinsArray.replace(index, newObject);
                        }
                        index++;
                    }
                }
                //calcule total
                double numberTotal = 0.00;
                foreach(const QJsonValue &value, coinsArray) numberTotal += value["totalAddition"].toDouble();
                //ajout nombre total
                jsonFileData.remove("numberTotal");
                jsonFileData.insert("numberTotal", QJsonValue::fromVariant(numberTotal));
                //ajout tableau coins
                jsonFileData.remove("coins");
                jsonFileData.insert("coins", QJsonValue::fromVariant(coinsArray));
                QJsonDocument doc;
                doc.setObject(jsonFileData);
                if (pieceFile.open(QIODevice::WriteOnly))
                {
                    pieceFile.write(doc.toJson(QJsonDocument::Indented));
                    pieceFile.close();
                }

            }
        }
        else qDebug() << "Json::answerWithdraw - " << "[Error] the piggy bank failed to withdraw the requested money !";
        additionByCoinsPending.clear();
        coinsPending.clear();
    }
    else qDebug() << "Json::answerWithdraw - " << "key status unknown";

}

void Json::requestHistoryServer(const QJsonObject keyAction)
{
    if (keyAction.contains("timestampLastUpdate"))
    {
        double timestamp = keyAction["timestampLastUpdate"].toDouble();
        QFile preciseFile(docFolder + "/json/historical/precise.json");
        QFile meanFile(docFolder + "/json/historical/mean.json");
        QByteArray preciseData;
        QByteArray meanData;
        if (preciseFile.open(QIODevice::ReadOnly))
        {
            preciseData = preciseFile.readAll();
            preciseFile.close();
        }
        if (meanFile.open(QIODevice::ReadOnly))
        {
            meanData = meanFile.readAll();
            meanFile.close();
        }
        if ((!preciseData.isNull()) || (!meanData.isNull()))
        {
            QJsonObject newPreciseObject;
            QJsonParseError parseError;
            QJsonObject preciseJson = QJsonDocument::fromJson(preciseData, &parseError).object();
            if (parseError.error == QJsonParseError::NoError)
            {
                QJsonArray preciseArray = preciseJson["precise"].toArray();
                QJsonArray newPreciseArray;
                foreach (const QJsonValue myPrecise, preciseArray)
                {
                    double dateOfPrecise = myPrecise["date"].toDouble();
                    if(timestamp <= dateOfPrecise) newPreciseArray.push_back(myPrecise);
                }
                newPreciseObject.insert("precise", newPreciseArray);
            }
            else qDebug() << "Json::parseJson - " << "[Error] impossible to interpret the received json because : "
                         << parseError.errorString();
            QJsonObject meanJson = QJsonDocument::fromJson(meanData).object();
            //appMobile
            QJsonObject mobileAppJson;
            //requete vers l'app :
            QJsonObject historyJson;
            historyJson.insert("mean",meanJson["mean"].toObject());
            historyJson.insert("precise", newPreciseObject["precise"].toArray());
            //ajout tout
            QJsonObject getAllDataJson;
            mobileAppJson.insert("history", historyJson);
            getAllDataJson.insert("mobileApp", mobileAppJson);
            //return qbyteArray
            QJsonDocument doc;
            doc.setObject(getAllDataJson);
            //envoie vers le MQTT
            pMqtt->sendMessage(doc.toJson(/*QJsonDocument::Compact*/QJsonDocument::Indented));
        }
        else qDebug() << "Json::parseJson - " << "[Error] file null " ;
    }
}

/**
 * @brief Json::restatedAmountAlgo
 * @param withdrawn
 * @details algor retirer des pieces
 */
void Json::restatedAmountAlgo(const QJsonObject withdrawn)
{
    static int attempt = 0;
    if(coinsPending.isEmpty() && additionByCoinsPending.isEmpty())
    {
        attempt = 0;
        QFile totalPieceFile(docFolder + "/json/totalPieces.json");
        QByteArray totalPieceData;
        if (totalPieceFile.open(QIODevice::ReadOnly))
        {
            totalPieceData = totalPieceFile.readAll();
            totalPieceFile.close();
        }
        if ((!totalPieceData.isNull()))
        {
            QJsonObject totalPieceObject = QJsonDocument::fromJson(totalPieceData).object();
            if (withdrawn.contains("withdrawn"))
            {
                double retiredAmount = withdrawn["withdrawn"].toDouble();
                //pieces dispo
                QList<double> coins;
                QList<double> additionByCoins;
                //recupe fichier json
                double totalAddition = totalPieceObject["numberTotal"].toDouble();
                QJsonArray coinsArray = totalPieceObject["coins"].toArray();
                for (int i = 0; i < coinsArray.size(); i++)
                {
                    QJsonObject myCoins = coinsArray[i].toObject();
                    additionByCoins.push_back(myCoins["totalAddition"].toDouble());
                    coins.push_back(myCoins["type"].toDouble());
                }
                double totalCurrentRackAmount = 0;
                QList<double> usedCoins;
                QList<double> usedAdditionByCoins;
                bool coinsInRackOk = false;

                if (totalAddition >= retiredAmount)
                {
                    for (int i = 0; i < coins.size() ; i++)
                    {
                        if (totalCurrentRackAmount < retiredAmount || !coinsInRackOk)
                        {
                            totalCurrentRackAmount += additionByCoins[i];
                            usedAdditionByCoins.push_back(additionByCoins[i]);
                            usedCoins.push_back(coins[i]);
                            //TODO : a modifier
                            coinsInRackOk = this->coinsInRack(usedCoins, usedAdditionByCoins, retiredAmount);
                        }
                    }
                }
                else qDebug() << "Json::restatedAmountAlgo - " << "not enough parts";
            }
            if (withdrawn.contains("withdrawnDetail"))
            {
                QJsonArray withdrawnDetailArray = withdrawn["withdrawnDetail"].toArray();
                //recupe fichier json
                QJsonArray coinsArray = totalPieceObject["coins"].toArray();
                QList<double> typePieces;
                QList<double> listPieceRetire;
                for (int i = 0; i < withdrawnDetailArray.size(); i++)
                {
                    QJsonObject withdrawnDetailObj = withdrawnDetailArray[i].toObject();
                    QStringList key = withdrawnDetailObj.keys();
                    for (int var = 0; var < coinsArray.size() ; var++)
                    {
                        QString myType = key[0];
                        QJsonObject myCoins = coinsArray[var].toObject();

                        if(myType.toDouble() == myCoins["type"].toDouble())
                        {
                            if(myCoins["number"].toInt() >= myCoins[myType].toInt())
                            {
                                typePieces.push_back(myType.toDouble());
                                listPieceRetire.push_back(withdrawnDetailObj[myType].toDouble());
                                QJsonObject esp32;
                                esp32.insert("action", "requestWithdraw");
                                esp32.insert("type", myType.toDouble());
                                esp32.insert("number", withdrawnDetailObj[myType].toDouble());
                                //requete vers l'esp32 :
                                QJsonObject requestWithdraw;
                                requestWithdraw.insert("esp32", esp32);
                                //return qbyteArray
                                QJsonDocument doc;
                                doc.setObject(requestWithdraw);
                                //envoie vers le MQTT
                                pMqtt->sendMessage(doc.toJson(/*QJsonDocument::Compact*/QJsonDocument::Indented));
                            }
                        }
                    }
                }
                if(!typePieces.isEmpty() && !listPieceRetire.isEmpty())
                {
                    QFile pieceFile(docFolder + "/json/totalPieces.json");
                    if (pieceFile.open(QIODevice::ReadOnly))
                    {
                        QByteArray fileData = pieceFile.readAll();
                        pieceFile.close();
                        QJsonObject jsonFileData = QJsonDocument::fromJson(fileData).object();
                        QJsonArray coinsArray = jsonFileData["coins"].toArray();
                        for (int i = 0; i < typePieces.size(); i++)
                        {
                            int index = 0;
                            foreach (const QJsonValue &value, coinsArray)
                            {
                                QJsonObject objCoins = value.toObject();
                                if (typePieces[i] == objCoins["type"].toDouble())
                                {
                                    QJsonObject newObject;
                                    int number = objCoins["number"].toInt() - listPieceRetire[i];
                                    double totalAddition = number * typePieces[i];
                                    //insert dans l'object
                                    newObject.insert("type", typePieces[i]);
                                    newObject.insert("number", number);
                                    newObject.insert("totalAddition", totalAddition);
                                    //remplace les valeurs
                                    coinsArray.replace(index, newObject);
                                }
                                index++;
                            }
                        }
                        //calcule total
                        double numberTotal = 0.00;
                        foreach(const QJsonValue &value, coinsArray) numberTotal += value["totalAddition"].toDouble();
                        //ajout nombre total
                        jsonFileData.remove("numberTotal");
                        jsonFileData.insert("numberTotal", QJsonValue::fromVariant(numberTotal));
                        //ajout tableau coins
                        jsonFileData.remove("coins");
                        jsonFileData.insert("coins", QJsonValue::fromVariant(coinsArray));
                        QJsonDocument doc;
                        doc.setObject(jsonFileData);
                        if (pieceFile.open(QIODevice::WriteOnly))
                        {
                            pieceFile.write(doc.toJson(QJsonDocument::Indented));
                            pieceFile.close();
                        }

                    }
                }
            }
            if(!withdrawn.contains("withdrawn") && !withdrawn.contains("withdrawnDetail"))
                qDebug() << "Json::restatedAmountAlgo - " << "[Error] action withdrawn";

        }
    }
    else
    {
        if(attempt < 3)
        {
            attempt++;
            QTimer::singleShot(500,this, [this,withdrawn](){this->restatedAmountAlgo(withdrawn);} );
        }
        else
        {
            attempt=0;
            additionByCoinsPending.clear();
            coinsPending.clear();
        }
    }
}

/**
 * @brief Json::coinsInRack
 * @param usedCoins
 * @param usedTotalAddition
 * @param retiredAmount
 * @return bool
 * @details si fonctionne alors retirer
 */
bool Json::coinsInRack(const QList<double> usedCoins, const QList<double> usedTotalAddition, const double retiredAmount)
{
    double remainingAmount = retiredAmount*10;
    QList<double> listPieceRetire;
    QList<double> typePieces;
    for (int i = usedCoins.size() - 1; i >= 0; i--)
    {
        if ((usedTotalAddition[i]*10 > remainingAmount) && (remainingAmount > 0))
        {
            int dividende = (int)(retiredAmount*10);
            int diviseur = (int)(usedCoins[i]*10);
            int moduloRest = dividende % diviseur;
            int montantPieceRetire = (remainingAmount - moduloRest);
            int nbrePiecesRetire = montantPieceRetire / (usedCoins[i] * 10);
            remainingAmount = moduloRest;
            listPieceRetire.push_back(nbrePiecesRetire);
            typePieces.push_back(usedCoins[i]);
        }
        else if ((usedTotalAddition[i]*10 <= remainingAmount)  && (remainingAmount > 0))
        {
            remainingAmount -= usedTotalAddition[i] * 10;
            listPieceRetire.push_back(usedTotalAddition[i]/usedCoins[i]);
            typePieces.push_back(usedCoins[i]);

        }
    }
    double total;
    for (int i = 0; i < listPieceRetire.size(); i++) total += typePieces[i] * listPieceRetire[i];
    if (total == retiredAmount)
    {
        for (int i = 0; i < listPieceRetire.size(); i++)
        {
            //requete vers l'esp32 :
            QJsonObject requestWithdraw;
            QJsonObject esp32;
            esp32.insert("action", "requestWithdraw");
            esp32.insert("type", typePieces[i]);
            esp32.insert("number", listPieceRetire[i]);
            requestWithdraw.insert("esp32", esp32);
            //return qbyteArray
            QJsonDocument doc;
            doc.setObject(requestWithdraw);
            //envoie vers le MQTT
            pMqtt->sendMessage(doc.toJson(/*QJsonDocument::Compact*/QJsonDocument::Indented));
            //save piece for json
            coinsPending.push_back(typePieces[i]);
            additionByCoinsPending.push_back(listPieceRetire[i]);
        }
        return true;
    }
    else return false;
}
