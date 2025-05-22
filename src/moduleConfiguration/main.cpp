/**
 * @file main.cpp
 *
 * @brief Programme principal
 * @author Axel Ariati
 * @version 1.0
 */

#include "ihmmoduleconfiguration.h"
#include "bluetooth.h"
#include "communication.h"
#include "gestionPartie.h"
#include <QApplication>

/**
 * @brief Programme principal
 * @details Crée et affiche la fenêtre principale de l'application
 *
 * @fn main
 * @param argc
 * @param argv[]
 * @return int
 *
 */

int main(int argc, char* argv[])
{
    QApplication           a(argc, argv);
    IHMModuleConfiguration ihmModuleConfiguration;
    Bluetooth              server(&ihmModuleConfiguration);
    ihmModuleConfiguration.show();

    return a.exec();
}
