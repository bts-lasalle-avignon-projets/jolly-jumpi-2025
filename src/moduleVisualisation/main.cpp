#include "ihm.h"
#include "bluetooth.h"
#include <QApplication>

/*
 *
 * @file main.cpp
 * @brief Programme principal
 * @details Crée et affiche la fenêtre principale de l'application
 * moduleVisualisation
 * @author Nicolas Pessina
 * @version 1.0
 *
 * @param argc
 * @param argv[]
 * @return int
 *
 */

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    IHM          ihm;

    ihm.show();

    return a.exec();
}
