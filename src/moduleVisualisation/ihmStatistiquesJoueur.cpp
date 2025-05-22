#include "ihmStatistiquesJoueur.h"
#include "ui_statistiquesJoueur.h"
#include "gestionPartie.h"
#include <QDebug>

IHMStatistiquesJoueur::IHMStatistiquesJoueur(GestionPartie* gestionPartie,
                                             QWidget*       parent) :
    QWidget{ parent },
    uiStatistiquesJoueur(new Ui::StatistiquesJoueur),
    gestionPartie(gestionPartie)
{
    uiStatistiquesJoueur->setupUi(this);

    setWindowTitle(QString(""));

    qDebug() << Q_FUNC_INFO << this << "gestionPartie" << gestionPartie;

#ifdef RPI
    showFullScreen();
#else
    showMaximized();
#endif
#ifdef SIMULATION_CLAVIER_STATISTIQUESJOUEUR
    simulerStatistiquesJoueur();
#endif
    connect(gestionPartie,
            &GestionPartie::estFinPartie,
            this,
            &IHMStatistiquesJoueur::fermer);
    deroulerStatistiques();
}

IHMStatistiquesJoueur::~IHMStatistiquesJoueur()
{
    delete uiStatistiquesJoueur;
    qDebug() << Q_FUNC_INFO << this << "gestionPartie" << gestionPartie;
}

/**
 * @brief S'exécute à chaque fois que la fenêtre est affichée
 *
 * @fn IHMPartie::showEvent
 *
 */
void IHMStatistiquesJoueur::showEvent(QShowEvent* event)
{
    qDebug() << Q_FUNC_INFO << this;
}

void IHMStatistiquesJoueur::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
}

void IHMStatistiquesJoueur::editerLabelNomJoueur(QString numeroJoueur)
{
    uiStatistiquesJoueur->labelJoueur->setText("Joueur " + numeroJoueur);
}

void IHMStatistiquesJoueur::editerLabelScore(QString scoreJoueur)
{
    uiStatistiquesJoueur->labelScore->setText("Score " + scoreJoueur);
}

void IHMStatistiquesJoueur::editerLabelPlace(QString placeJoueur)
{
    uiStatistiquesJoueur->labelPlace->setText("Place " + placeJoueur);
}

void IHMStatistiquesJoueur::editerLabelNumeroTir(QString ligne, QString numero)
{
    QString nomLabel = QString("labelL") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(numero);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMStatistiquesJoueur::editerLabelScoreTir(QString ligne, QString scoreTir)
{
    QString nomLabel = QString("labelP") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(scoreTir);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMStatistiquesJoueur::editerLabelScoreCumule(QString ligne,
                                                   QString scoreCumule)
{
    QString nomLabel = QString("labelS") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(scoreCumule);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMStatistiquesJoueur::editerLabelTempsTir(QString ligne, QString tempsTir)
{
    QString nomLabel = QString("labelT") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(tempsTir);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMStatistiquesJoueur::editerLabelEcartTir(QString ligne,
                                                QString tempsEntreTir)
{
    QString nomLabel = QString("labelE") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(tempsEntreTir);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMStatistiquesJoueur::editerLabelTrouPrefere(QString ligne,
                                                   QString tourPrefere)
{
    QString nomLabel = QString("labelTPref") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(tourPrefere);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMStatistiquesJoueur::deroulerStatistiques()
{
    qDebug() << Q_FUNC_INFO;
    indexJoueurStatistiques = 0;
    deroulerStatistiquesSuivant();
}

void IHMStatistiquesJoueur::deroulerStatistiquesSuivant()
{
    qDebug() << Q_FUNC_INFO;
    QString numero = QString::number(indexJoueurStatistiques + 1);
    afficherStatistiques(numero);

    indexJoueurStatistiques++;
    if(indexJoueurStatistiques < gestionPartie->recupererNombreJoueurs())
    {
        QTimer::singleShot(TEMPS_AFFICHAGE_JOUEUR * 1000,
                           this,
                           SLOT(deroulerStatistiquesSuivant()));
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "plus de joueur";
    }
}

void IHMStatistiquesJoueur::afficherStatistiques(const QString numero)
{
    qDebug() << Q_FUNC_INFO;
    std::vector<QList<QString> > statistiques =
      gestionPartie->recupererStatistiquesJoueur(numero);
    reitialiserLabel();
    for(int ligne = 0; ligne < statistiques.size(); ++ligne)
    {
        QList<QString> elementsStatistiques = statistiques[ligne];
        QString        numeroLigne          = QString::number(ligne + 1);

        editerLabelNomJoueur(numero);
        editerLabelScore(
          QString::number(gestionPartie->recupererScoreJoueur(numero)));
        editerLabelPlace(
          QString::number(gestionPartie->recupererPlaceJoueur(numero)));
        editerLabelNumeroTir(numeroLigne, elementsStatistiques[0]);
        editerLabelScoreTir(numeroLigne, elementsStatistiques[1]);
        editerLabelScoreCumule(numeroLigne, elementsStatistiques[2]);
        editerLabelTempsTir(numeroLigne, elementsStatistiques[3]);
        editerLabelEcartTir(numeroLigne, elementsStatistiques[4]);
        editerLabelTrouPrefere(numeroLigne, elementsStatistiques[5]);
        qDebug() << Q_FUNC_INFO << "ligne" << numeroLigne
                 << elementsStatistiques;
    }
}

void IHMStatistiquesJoueur::reitialiserLabel()
{
    for(int i = 0; i < NOMBRE_MAX_TIR_POSSIBLE; i++)
    {
        QString numeroLigne = QString::number(i + 1);
        qDebug() << Q_FUNC_INFO << "ligne" << numeroLigne << "réinitialisé";
        editerLabelNumeroTir(numeroLigne, "");
        editerLabelScoreTir(numeroLigne, "");
        editerLabelScoreCumule(numeroLigne, "");
        editerLabelTempsTir(numeroLigne, "");
        editerLabelEcartTir(numeroLigne, "");
        editerLabelTrouPrefere(numeroLigne, "");
    }
}

#ifdef SIMULATION_CLAVIER_STATISTIQUESJOUEUR
void IHMStatistiquesJoueur::simulerStatistiquesJoueur()
{
    // Touche Q -> fermer
    QAction* quitter = new QAction(this);
    quitter->setShortcut(QKeySequence(QKeySequence(Qt::Key_Q)));
    addAction(quitter);
    connect(quitter, SIGNAL(triggered()), this, SLOT(fermer()));
}
#endif
