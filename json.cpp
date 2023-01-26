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
        qDebug() << "action is : " << keyAction;
        break;
    }
    case 1:
    {
        qDebug() << "action is : " << keyAction;
        break;
    }
    case 2:
    {
        this->getAllData(keyAction);
        break;
    }
    case 3:
    {
        qDebug() << "action is : " << keyAction;
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
        //ouvrir fichier
        QString docFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
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
                QJsonArray coinsFile = jsonFileData["coins"].toArray();
                //insere les nouvelles données
                for (int i = 0; i < coinsReceved.size(); i++)
                {
                    QJsonObject objCoinsReceved = coinsReceved[i].toObject();
                    int index = 0;
                    foreach (const QJsonValue &valueFile, coinsFile)
                    {
                        QJsonObject objCoinsFile = valueFile.toObject();
                        if (objCoinsReceved["type"].toDouble() == objCoinsFile["type"].toDouble())
                        {
                            QJsonObject newObject;
                            int number = objCoinsReceved["number"].toInt()
                                         + objCoinsFile["number"].toInt();
                            double totalAddition = number * objCoinsReceved["type"].toDouble();
                            //insert dans l'object
                            newObject.insert("type", objCoinsReceved["type"].toDouble());
                            newObject.insert("number", number);
                            newObject.insert("totalAddition", totalAddition);
                            //remplace les valeurs
                            coinsFile.replace(index, newObject);
                        }
                        index++;
                    }
                }
                //calcule total
                double numberTotal = 0.00;
                foreach(const QJsonValue &value, coinsFile) numberTotal += value["totalAddition"].toDouble();
                //ajout nombre total
                jsonFileData.remove("numberTotal");
                jsonFileData.insert("numberTotal", QJsonValue::fromVariant(numberTotal));
                //ajout tableau coins
                jsonFileData.remove("coins");
                jsonFileData.insert("coins", QJsonValue::fromVariant(coinsFile));
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

void Json::getAllData(const QJsonObject requestData)
{
    if (requestData.contains("timestampLastUpdate"))
    {
        double timestamp = requestData["timestampLastUpdate"].toDouble();
        QString docFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
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
