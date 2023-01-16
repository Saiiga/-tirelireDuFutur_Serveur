/**
 * \file main.cpp
 * \brief serveur Tirelire connectée Qt
 * \author Groupe 6
 * \copyright Copyright (c)
 * \version 1.0
 * \date 16/01/2023
 * \details classe principale
 */
#include <QCoreApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //instancie client
    Client myClient;
    return a.exec();
}
