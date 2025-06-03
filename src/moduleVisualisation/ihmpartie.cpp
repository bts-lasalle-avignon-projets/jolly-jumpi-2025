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

    nomLabels << "LigneArrivee" // LIGNE_ARRIVEE
              << "Numero"       // NUMERO
              << "Cheval";      // CHEVAL

    listePixmaps << QString(":/images/pagePartie/ligneArrivee.jpg")
                 << QString(":/images/pagePartie/chevaux/chevalMarron.png")
                 << QString(":/images/pagePartie/chevaux/chevalJaune.png")
                 << QString(":/images/pagePartie/chevaux/chevalBleuFonce.png")
                 << QString(":/images/pagePartie/chevaux/chevalGris.png")
                 << QString(":/images/pagePartie/chevaux/chevalRouge.png")
                 << QString(":/images/pagePartie/chevaux/chevalBleuClair.png")
                 << QString(":/images/pagePartie/chevaux/chevalViolet.png")
                 << QString(":/images/pagePartie/chevaux/chevalNoir.png");

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
    uiPartie->lcdNumberChonometre->display("-:--");
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

QLabel* IHMPartie::recupererLabel(const QString labelRecherche)
{
    QString nomLabel = "label" + labelRecherche;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);
    if(!label)
    {
        qDebug() << Q_FUNC_INFO << "label" << label << "non trouvé";
        return nullptr;
    }
    return label;
}

void IHMPartie::creerLigneCourse(int nombreJoueurs)
{
    qDebug() << Q_FUNC_INFO << "nombreJoueurs" << nombreJoueurs;
    for(int i = 0; i < NOMBRE_JOUEUR_MAX; i++)
    {
        QString ligne = QString::number(i + 1);
        for(int index = 0; index < NB_LABELS; index++)
        {
            QLabel* label = recupererLabel(nomLabels[index] + ligne);
            if(label)
            {
                if(index == LIGNE_ARRIVEE)
                    editerLabelPixmap(label, listePixmaps[index], ligne);
                else if(index == NUMERO)
                    editerLabelText(label, ligne);
                else
                    editerLabelPixmap(label,
                                      listePixmaps[ligne.toInt()],
                                      ligne);
                initialiserEmplacementLabel(label, index);
            }
        }
    }
}

void IHMPartie::editerLabelPixmap(QLabel*        label,
                                  const QPixmap& pixmap,
                                  const QString& numeroLigne)
{
    qDebug() << Q_FUNC_INFO << "numeroLigne" << numeroLigne;

    if(numeroLigne.toInt() <= gestionPartie->recupererNombreJoueurs())
    {
        label->setPixmap(pixmap);
        label->setScaledContents(true);
    }
    else
        label->clear();
}

void IHMPartie::initialiserEmplacementLabel(QLabel* label, const int& index)
{
    qDebug() << Q_FUNC_INFO;

    int largeurCheval = recupererLabel(nomLabels[CHEVAL] + "1")->width();

    QPoint positionActuelle = label->pos(); // seul Y nous intéresse
    QPoint positionDefaut;
    qDebug() << Q_FUNC_INFO << "largeurCheval" << largeurCheval
             << "taille label" << label->width() << "taille ecran" << width();

    if(index == LIGNE_ARRIVEE)
    {
        QScreen* ecran = this->screen();
        int      largeurEcranUtilise =
          (ecran->size() * ecran->devicePixelRatio()).width();
        positionDefaut = QPoint(
          largeurEcranUtilise - (largeurCheval / (SCORE_MAX / 10)),
          positionActuelle.y()); // s'adapte à tout les écrans et scoreMax
        qDebug() << Q_FUNC_INFO << positionDefaut;
    }
    else if(index == NUMERO)
        positionDefaut = QPoint(10, positionActuelle.y());
    else
        positionDefaut = QPoint(0, positionActuelle.y());

    label->move(positionDefaut);
}

void IHMPartie::mettreAJoursChronometre(int secondes)
{
    qDebug() << Q_FUNC_INFO;
    uiPartie->lcdNumberChonometre->display(
      gestionPartie->convertirTemps(secondes));
}

void IHMPartie::mettreAJoursCourse(QString numero, int scoreTir)
{
    qDebug() << Q_FUNC_INFO << "numero" << numero << "scoreTir" << scoreTir;
    QString nomLabelCheval = nomLabels[CHEVAL] + numero;
    QString nomLabelNumero = nomLabels[NUMERO] + numero;
    QLabel* labelCheval    = recupererLabel(nomLabelCheval);
    QLabel* labelNumero    = recupererLabel(nomLabelNumero);
    if(!labelCheval || !labelNumero)
    {
        qDebug() << Q_FUNC_INFO << "labels" << labelCheval << labelNumero
                 << "non trouvés";
        return;
    }

    int pointManquant = SCORE_MAX - gestionPartie->recupererScoreJoueur(numero);
    int avancement    = 0;
    int segmentEcran  = (width() - labelCheval->width()) / SCORE_MAX;
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

    faireAvancerLabel(labelCheval, avancement, multiplicateurTemps);
    faireAvancerLabel(labelNumero, avancement, multiplicateurTemps);
}

void IHMPartie::faireAvancerLabel(QLabel*    label,
                                  const int& avancement,
                                  const int& multiplicateurTemps)
{
    QPoint posActuelle = label->pos();
    QPoint nouvellePosition =
      QPoint(posActuelle.x() + avancement, posActuelle.y());
    QPropertyAnimation* avancer = new QPropertyAnimation(label, "pos");
    avancer->setDuration(multiplicateurTemps * 400);
    avancer->setStartValue(posActuelle);
    avancer->setEndValue(nouvellePosition);
    avancer->start(QAbstractAnimation::DeleteWhenStopped);
}

void IHMPartie::editerLabelText(QLabel* label, QString ligne)
{
    label->setText(ligne);
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
