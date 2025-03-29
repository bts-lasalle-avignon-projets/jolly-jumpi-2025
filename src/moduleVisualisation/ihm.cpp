/**
 * @file ihm.cpp
 *
 * @brief Définition de la classe IHM
 * @author Nicolas Pessina
 * @version 1.0
 */

#include "ihm.h"
#include "gestionPartie.h"
#include "ui_accueil.h"
#include <QDebug>

/**
 * @brief Constructeur de la classe IHM
 *
 * @fn IHM::IHM
 * @param parent L'adresse de l'objet parent, si nullptr IHM sera la
 * fenêtre principale de l'application
 */
IHM::IHM(QWidget* parent) :
    QWidget(parent), gestionPartie(new GestionPartie(this)),
    uiAccueil(new Ui::Accueil)
{
    uiAccueil->setupUi(this);

    qDebug() << Q_FUNC_INFO << this;

    setWindowTitle(QString(NOM_MODULE) + QString(" v") +
                   QString(VERSION_MODULE));

#ifdef RPI
    showFullScreen();
#endif
}

IHM::~IHM()
{
    delete gestionPartie;
    qDebug() << Q_FUNC_INFO << this;
}
