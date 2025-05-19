#include "ihmpartie.h"
#include "ui_partie.h"
#include "ihmClassement.h"
#include "ihmStatistiquesJoueur.h"
#include "communication.h"
#include "gestionPartie.h"
#include <QDebug>

IHMPartie::IHMPartie(Communication* communication, QWidget* parent) :
    QWidget(parent), uiPartie(new Ui::Partie), ihmClassement(nullptr),
    ihmStatistiquesJoueur(nullptr), communication(communication),
    gestionPartie(new GestionPartie(communication, this))

{
    uiPartie->setupUi(this);

    setWindowTitle(QString(""));

    qDebug() << Q_FUNC_INFO << this;

#ifdef RPI
    showFullScreen();
#else
    showMaximized();
#endif
#ifdef SIMULATION_CLAVIER_PARTIE
    simulerPartie();
#endif
}

IHMPartie::~IHMPartie()
{
    if(ihmClassement != nullptr)
        delete ihmClassement;
    if(ihmStatistiquesJoueur != nullptr)
        delete ihmStatistiquesJoueur;
    delete gestionPartie;
    delete uiPartie;
    qDebug() << Q_FUNC_INFO << this;
}

/**
 * @brief S'exécute à chaque fois que la fenêtre est affichée
 *
 * @fn IHMPartie::showEvent
 *
 */
void IHMPartie::showEvent(QShowEvent* event)
{
    qDebug() << Q_FUNC_INFO << this;
    gestionPartie->gererPartie();
}

void IHMPartie::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
}

#ifdef SIMULATION_CLAVIER_PARTIE
void IHMPartie::simulerPartie()
{
    // Touche Q -> fermer
    QAction* quitter = new QAction(this);
    quitter->setShortcut(QKeySequence(QKeySequence(Qt::Key_Q)));
    addAction(quitter);
    connect(quitter, SIGNAL(triggered()), this, SLOT(fermer()));

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
            SLOT(afficherStatistiquesJoueur()));
}

void IHMPartie::afficherClassement()
{
    qDebug() << Q_FUNC_INFO;
    if(ihmClassement == nullptr)
    {
        ihmClassement = new IHMClassement(gestionPartie);
    }
    else
    {
        ihmClassement->show();
    }
}

void IHMPartie::afficherStatistiquesJoueur()
{
    qDebug() << Q_FUNC_INFO;
    if(ihmStatistiquesJoueur == nullptr)
    {
        ihmStatistiquesJoueur = new IHMStatistiquesJoueur(gestionPartie);
    }
    else
    {
        ihmStatistiquesJoueur->show();
    }
}
#endif
