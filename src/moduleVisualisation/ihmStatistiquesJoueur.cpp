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
    nomLabelsEntete << "Joueur" // JOUEUR
                    << "Score"  // SCORE
                    << "Place"; // PLACE

    nomLabels << "L"      // NUMERO_TIR
              << "P"      // SCORE_TIR
              << "S"      // SCORE_CUMULE
              << "T"      // TEMPS_TIR
              << "E"      // ECART_TIR
              << "TPref"; // TROU_PREFERE

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
    redimensionnerLabel();
    deroulerStatistiques();
}

void IHMStatistiquesJoueur::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
}

void IHMStatistiquesJoueur::editerLabelEntete(const QString numero,
                                              int           indexLabelEntete)
{
    QString valeur;
    if(indexLabelEntete == JOUEUR)
        valeur = numero;
    else if(indexLabelEntete == SCORE)
        valeur = QString::number(gestionPartie->recupererScoreJoueur(numero));
    else if(indexLabelEntete == PLACE)
        valeur = QString::number(gestionPartie->recupererPlaceJoueur(numero));
    else
    {
        qDebug() << Q_FUNC_INFO << "indexLabelEntete invalide"
                 << indexLabelEntete;
        return;
    }

    editerLabel(nomLabelsEntete[indexLabelEntete],
                nomLabelsEntete[indexLabelEntete] + " " + valeur);
}

void IHMStatistiquesJoueur::editerLabel(const QString& labelRecherche,
                                        const QString& valeur)
{
    QString nomLabel = "label" + labelRecherche;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
    {
        label->setText(valeur);
    }
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
        QTimer::singleShot(gestionPartie->recupererTempsAffichageFenetre() *
                             1000,
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
    qDebug() << Q_FUNC_INFO << numero;
    std::vector<QList<QString> > statistiques =
      gestionPartie->recupererStatistiquesJoueur(numero);
    for(int indexLabelEntete = 0; indexLabelEntete < NB_LABELS_ENTETE;
        ++indexLabelEntete)
    {
        editerLabelEntete(numero, indexLabelEntete);
    }
    for(int ligne = 0; ligne < gestionPartie->recupererScoreMax(); ++ligne)
    {
        QString numeroLigne = QString::number(ligne + 1);

        for(int indexLabel = 0; indexLabel < NB_LABELS; ++indexLabel)
        {
            if(ligne < statistiques.size())
            {
                QList<QString> elementsStatistiques = statistiques[ligne];
                editerLabel(nomLabels[indexLabel] + numeroLigne,
                            elementsStatistiques[indexLabel]);
            }
            else
                editerLabel(nomLabels[indexLabel] + numeroLigne, "");
            // vide les tirs fantomes
        }
    }
}

void IHMStatistiquesJoueur::appliquerMiseEnForme(QLayout* layout,
                                                 int      taillePolice)
{
    for(int i = 0; i < layout->count(); ++i)
    {
        QLayoutItem* item = layout->itemAt(i);

        if(QWidget* widget = item->widget())
        {
            if(QLabel* label = qobject_cast<QLabel*>(widget))
            {
                QFont police = label->font();
                police.setPointSize(taillePolice);
                label->setFont(police);
            }
        }
        else if(QLayout* sousLayout = item->layout())
        {
            // Appel récursif pour gérer les layouts imbriqués
            appliquerMiseEnForme(sousLayout, taillePolice);
        }
    }
}

void IHMStatistiquesJoueur::redimensionnerLabel()
{
    qDebug() << Q_FUNC_INFO;
    // int          tailleTitre = 30;
    int          tailleTexte = 20;
    QVBoxLayout* vLayoutTableauStatistiques =
      findChild<QVBoxLayout*>("vLayoutTableauStatistiques");

    if(vLayoutTableauStatistiques)
    {
        qDebug() << Q_FUNC_INFO << "Trouvé" << vLayoutTableauStatistiques;
        appliquerMiseEnForme(vLayoutTableauStatistiques, tailleTexte);
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
