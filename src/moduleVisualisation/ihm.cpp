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
#include "communication.h"
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
    communication(new Communication(this)), minuteurDefilement(new QTimer),
    numeroMessage(0)
{
    uiAccueil->setupUi(this);

    qDebug() << Q_FUNC_INFO << this;

    setWindowTitle(QString(NOM_MODULE) + QString(" v") +
                   QString(VERSION_MODULE));

#ifdef RPI
    QRect ecran   = QApplication::desktop()->screenGeometry();
    int   largeur = ecran.width();
    int   hauteur = ecran.height();
    qDebug() << "largeur" << largeur; // Largeur
    qDebug() << "hauteur" << hauteur; // Hauteur
    setFixedSize(largeur, hauteur);
    showFullScreen();
#else
    showMaximized();
#endif
#ifdef SIMULATION_CLAVIER_ACCUEIL
    simulerAffichageFenetre();
#endif
#ifdef TELEVISION
    redimentionnerElements();
#endif

    listeMessages << "BTS CIEL IR"
                  << "LaSalle Avignon"
                  << "Nicolas Pessina";
    listeTitres << "BIENVENUE!"
                << "Attente connexion";

    connect(minuteurDefilement, SIGNAL(timeout()), this, SLOT(defilerTexte()));
    connect(minuteurDefilement, SIGNAL(timeout()), this, SLOT(defilerTitre()));
    uiAccueil->labelDefilementTexte->setText(
      listeMessages.at(numeroMessage++ % listeMessages.count()));
    uiAccueil->labelDefilementTitre->setText(
      listeTitres.at(numeroTitre++ % listeTitres.count()));
    minuteurDefilement->start(PERIODE_DEFILEMENT);

    connect(communication,
            &Communication::modulesConnectes,
            this,
            &IHM::mettreAJourListeTitres);

    connect(communication,
            &Communication::configurationEnCours,
            this,
            &IHM::afficherPartie);
    //** @todo faire de même pour la page historiques*/
}

IHM::~IHM()
{
    if(ihmPartie != nullptr)
        delete ihmPartie;
    delete uiAccueil;
    qDebug() << Q_FUNC_INFO << this;
}

void IHM::redimentionnerElements()
{
    QLabel* labelFond = findChild<QLabel*>("labelFond");
    labelFond->setGeometry(0, 0, width(), height());
    QVBoxLayout* verticalLayoutPrincipal =
      findChild<QVBoxLayout*>("verticalLayoutPrincipal");
    this->setLayout(verticalLayoutPrincipal);
}

void IHM::defilerTexte()
{
    uiAccueil->labelDefilementTexte->setText(
      listeMessages.at(numeroMessage++ % listeMessages.count()));
}

void IHM::defilerTitre()
{
    uiAccueil->labelDefilementTitre->setText(
      listeTitres.at(numeroTitre++ % listeTitres.count()));
}

void IHM::mettreAJourListeTitres()
{
    listeTitres[1] = "attente configuration";
}

#ifdef SIMULATION_CLAVIER_ACCUEIL
/**
 * @fn IHM::simulerAffichageFenetre
 * @brief méthode pour créer les raccourcis clavier (pour les tests
 * seulement)
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
}
#endif

void IHM::fermer()
{
    this->close();
}

void IHM::afficherPartie()
{
    qDebug() << Q_FUNC_INFO;
    if(ihmPartie == nullptr)
    {
        ihmPartie = new IHMPartie(communication);
    }
    else
    {
        ihmPartie->show();
    }
}

void IHM::estPartieEnConfiguration()
{
    /** @todo faire défiler le texte pour signaler "En config"*/
}
