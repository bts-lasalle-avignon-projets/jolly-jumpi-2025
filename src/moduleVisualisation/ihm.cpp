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
#include "ui_partie.h"
#include "ui_classement.h"
#include "ui_statistiquesJoueur.h"
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
    uiAccueil(new Ui::Accueil), uiPartie(new Ui::Partie),
    uiClassement(new Ui::Classement),
    uiStatistiquesJoueur(new Ui::StatistiquesJoueur)
{
    // uiAccueil->setupUi(this);
    uiPartie->setupUi(this);
    // uiClassement->setupUi(this);
    // uiStatistiquesJoueur->setupUi(this);

    qDebug() << Q_FUNC_INFO << this;

    setWindowTitle(QString(NOM_MODULE) + QString(" v") +
                   QString(VERSION_MODULE));
    showFullScreen();
#ifdef RPI
    showFullScreen();
#endif
}

IHM::~IHM()
{
    delete gestionPartie;
    delete uiAccueil;
    delete uiPartie;
    delete uiClassement;
    delete uiStatistiquesJoueur;
    qDebug() << Q_FUNC_INFO << this;
}
