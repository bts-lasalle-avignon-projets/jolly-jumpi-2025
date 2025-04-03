/**
 * @file ihm.cpp
 *
 * @brief Définition de la classe IHM
 * @author Nicolas Pessina
 * @version 1.0
 */

#include "ihm.h"
#include "ui_accueil.h"
#include "ihmpartie.h"
#include <QAction>
#include <QDebug>

/**
 * @brief Constructeur de la classe IHM
 *
 * @fn IHM::IHM
 * @param parent L'adresse de l'objet parent, si nullptr IHM sera la
 * fenêtre principale de l'application
 */
IHM::IHM(QWidget* parent) :
    QWidget(parent), uiAccueil(new Ui::Accueil), ihmPartie(nullptr),
    minuteurDefilement(new QTimer), numeroMessage(0)
{
    uiAccueil->setupUi(this);

    qDebug() << Q_FUNC_INFO << this;

    setWindowTitle(QString(NOM_MODULE) + QString(" v") +
                   QString(VERSION_MODULE));

#ifdef RPI
    showFullScreen();
#else
    showMaximized();
#endif
#ifdef SIMULATION_CLAVIER_ACCUEIL
    simulerAffichageFenetre();
#endif

    listeMessages << "BTS CIEL IR"
                  << "LaSalle Avignon"
                  << "Nicolas Pessina";
    connect(minuteurDefilement, SIGNAL(timeout()), this, SLOT(defilerTexte()));
    minuteurDefilement->start(PERIODE_DEFILEMENT);
}

IHM::~IHM()
{
    if(ihmPartie != nullptr)
        delete ihmPartie;
    delete uiAccueil;
    qDebug() << Q_FUNC_INFO << this;
}

void IHM::defilerTexte()
{
    uiAccueil->labelDefilementTexte->setText(
      listeMessages.at(numeroMessage++ % listeMessages.count()));
}

#ifdef SIMULATION_CLAVIER_ACCUEIL
/**
 * @fn IHM::simulerAffichageFenetre
 * @brief méthode pour créer les raccourcis clavier (pour les tests seulement)
 */
void IHM::simulerAffichageFenetre()
{
    // les touches pour "changer" de fenêtre avec le clavier
    // Touche Q -> fermer
    QAction* quitter = new QAction(this);
    quitter->setShortcut(QKeySequence(QKeySequence(Qt::Key_Q)));
    addAction(quitter);
    connect(quitter, SIGNAL(triggered()), this, SLOT(fermer()));

    // Touche P -> affichage Partie
    QAction* affichagePartie = new QAction(this);
    affichagePartie->setShortcut(QKeySequence(Qt::Key_P));
    addAction(affichagePartie);
    connect(affichagePartie, SIGNAL(triggered()), this, SLOT(afficherPartie()));

    // Touche C -> affichage Classement
    QAction* affichageClassement = new QAction(this);
    affichageClassement->setShortcut(QKeySequence(Qt::Key_C));
    addAction(affichageClassement);
    connect(affichageClassement,
            SIGNAL(triggered()),
            this,
            SLOT(afficherClassement()));

    // Touche S -> affichage Statistiques
    QAction* affichageStatistiques = new QAction(this);
    affichageStatistiques->setShortcut(QKeySequence(Qt::Key_S));
    addAction(affichageStatistiques);
    connect(affichageStatistiques,
            SIGNAL(triggered()),
            this,
            SLOT(afficherStatistiques()));
}
#endif

void IHM::fermer()
{
    this->close();
}

#ifdef SIMULATION_CLAVIER_ACCUEIL
void IHM::afficherPartie()
{
    qDebug() << Q_FUNC_INFO;
    if(ihmPartie == nullptr)
    {
        ihmPartie = new IHMPartie();
    }
    else
    {
        ihmPartie->show();
    }
}

void IHM::afficherClassement()
{
    qDebug() << Q_FUNC_INFO;
}

void IHM::afficherStatistiques()
{
    qDebug() << Q_FUNC_INFO;
}
#endif
