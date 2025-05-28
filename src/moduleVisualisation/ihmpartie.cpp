#include "ihmpartie.h"
#include "ui_partie.h"
#include "ihmClassement.h"
#include "ihmStatistiquesJoueur.h"
#include "communication.h"
#include "gestionPartie.h"
#include <QDebug>
#include <QPropertyAnimation>

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
    connect(gestionPartie,
            &GestionPartie::estFinPartie,
            this,
            &IHMPartie::fermer);
    connect(gestionPartie,
            &GestionPartie::demandeClassement,
            this,
            &IHMPartie::afficherClassement);
    connect(gestionPartie,
            &GestionPartie::demandeStatistiquesJoueur,
            this,
            &IHMPartie::afficherStatistiquesJoueur);
    connect(gestionPartie,
            &GestionPartie::tirRecu,
            this,
            &IHMPartie::mettreAJoursCourse);
    connect(gestionPartie,
            &GestionPartie::creerChevaux,
            this,
            &IHMPartie::creerLigneCourse);
    connect(gestionPartie,
            &GestionPartie::avancementChronometre,
            this,
            &IHMPartie::mettreAJoursChronometre);
    deplacerLigneArrivee();
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
    initialiserEmplacementLabel();
    uiPartie->lcdNumberChonometre->display("0:00");
}

void IHMPartie::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
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

void IHMPartie::creerLigneCourse(int nombreJoueurs)
{
    qDebug() << Q_FUNC_INFO << "nombreJoueurs" << nombreJoueurs;
    for(int i = 0; i < NOMBRE_JOUEUR_MAX; i++)
    {
        QString ligne    = QString::number(i + 1);
        QString nomLabel = QString("labelCheval") + ligne;
        QLabel* label    = this->findChild<QLabel*>(nomLabel);
        QPixmap pixmap(":/partie/cheval.png");
        creerLigneArrivee(ligne);

        if(label)
        {
            if(i < nombreJoueurs)
            {
                label->setPixmap(pixmap);
                label->setScaledContents(true);
            }
            else
            {
                label->clear();
            }
        }
        else
            qDebug() << Q_FUNC_INFO << "Label" << nomLabel << "non trouvé";
    }
}

void IHMPartie::mettreAJoursCourse(QString numero, int scoreTir)
{
    qDebug() << Q_FUNC_INFO << "numero" << numero << "scoreTir" << scoreTir;
    QString nomLabel = "labelCheval" + numero;
    QLabel* label    = findChild<QLabel*>(nomLabel);
    if(!label)
    {
        qDebug() << Q_FUNC_INFO << "Label" << nomLabel << "non trouvé";
        return;
    }

    QWidget* parentWidget = label->parentWidget();
    if(!parentWidget)
    {
        qDebug() << Q_FUNC_INFO << "parentWidget non trouvé";
        return;
    }

    int pointManquant = SCORE_MAX - gestionPartie->recupererScoreJoueur(numero);
    int avancement    = 0;
    int segmentEcran  = (parentWidget->width() - label->width()) / SCORE_MAX;
    int multiplicateurTemps = 1;

    if(scoreTir <= pointManquant)
    {
        avancement          = segmentEcran * scoreTir;
        multiplicateurTemps = multiplicateurTemps * scoreTir;
        qDebug() << Q_FUNC_INFO << "pointManquant <= au scorTir";
    }
    else
    {
        avancement          = segmentEcran * pointManquant;
        multiplicateurTemps = multiplicateurTemps * pointManquant;
        qDebug() << Q_FUNC_INFO << "pointManquant > au scorTir";
    }

    qDebug() << Q_FUNC_INFO << "numero" << numero << "segmentEcran"
             << segmentEcran << "pointManquant" << pointManquant << "avancement"
             << avancement << "multiplicateurTemps" << multiplicateurTemps;

    QPoint posActuelle = label->pos();
    QPoint nouvellePosition =
      QPoint(posActuelle.x() + avancement, posActuelle.y());
    QPropertyAnimation* avancer = new QPropertyAnimation(label, "pos");
    avancer->setDuration(multiplicateurTemps * 400);
    avancer->setStartValue(posActuelle);
    avancer->setEndValue(nouvellePosition);
    avancer->start(QAbstractAnimation::DeleteWhenStopped);
}

void IHMPartie::initialiserEmplacementLabel()
{
    qDebug() << Q_FUNC_INFO;
    for(int i = 0; i < NOMBRE_JOUEUR_MAX; i++)
    {
        QString numero   = QString::number(i + 1);
        QString nomLabel = "labelCheval" + numero;
        QLabel* label    = findChild<QLabel*>(nomLabel);
        if(!label)
        {
            qDebug() << Q_FUNC_INFO << "Label" << nomLabel << "non trouvé";
            return;
        }

        QWidget* parentWidget = label->parentWidget();
        if(!parentWidget)
        {
            qDebug() << Q_FUNC_INFO << "parentWidget non trouvé";
            return;
        }
        QPoint positionY        = label->pos();
        QPoint positionInitiale = QPoint(0, positionY.y());
        label->move(positionInitiale);
    }
}

void IHMPartie::mettreAJoursChronometre(int secondes)
{
    qDebug() << Q_FUNC_INFO;
    uiPartie->lcdNumberChonometre->display(
      gestionPartie->convertirTemps(secondes));
}

void IHMPartie::deplacerLigneArrivee()
{
    qDebug() << Q_FUNC_INFO;
    for(int i = 0; i < NOMBRE_JOUEUR_MAX; i++)
    {
        QString ligne       = QString::number(i + 1);
        QString nomLabel    = "labelLigneArrivee" + ligne;
        QString nomLabelRef = "labelCheval1";
        QLabel* label       = findChild<QLabel*>(nomLabel);
        QLabel* labelRef    = findChild<QLabel*>(nomLabelRef);
        if(!label)
        {
            qDebug() << Q_FUNC_INFO << "Label" << nomLabel << "non trouvé";
            return;
        }

        int segmentEcran    = (width() - labelRef->width()) / SCORE_MAX;
        int positionDesiree = segmentEcran * 7 - label->width() * 3;
        qDebug() << Q_FUNC_INFO << "position" << positionDesiree;
        QPoint posActuelle = label->pos();
        QPoint nouvellePosition =
          QPoint(posActuelle.x() + positionDesiree, posActuelle.y());
        label->move(nouvellePosition);
    }
}

void IHMPartie::creerLigneArrivee(QString ligne)
{
    qDebug() << Q_FUNC_INFO;

    QString nomLabel = QString("labelLigneArrivee") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);
    QPixmap pixmap(":/partie/ligneArrivee.jpg");

    if(label)
    {
        if(ligne.toInt() <= gestionPartie->recupererNombreJoueurs())
        {
            label->setPixmap(pixmap);
            label->setScaledContents(true);
        }
        else
        {
            label->clear();
        }
    }
    else
        qDebug() << Q_FUNC_INFO << "Label" << nomLabel << "non trouvé";
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
#endif
