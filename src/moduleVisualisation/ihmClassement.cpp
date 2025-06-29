#include "ihmClassement.h"
#include "ui_classement.h"
#include "gestionPartie.h"
#include <QDebug>

IHMClassement::IHMClassement(GestionPartie* gestionPartie, QWidget* parent) :
    QWidget{ parent }, uiClassement(new Ui::Classement),
    gestionPartie(gestionPartie)
{
    uiClassement->setupUi(this);

    setWindowTitle(QString(""));

    qDebug() << Q_FUNC_INFO << this << "gestionPartie" << gestionPartie;

    nomLabels << "P"  // PLACE
              << "J"  // JOUEUR
              << "S"; // SCORE

#ifdef RPI
    showFullScreen();
#else
    showMaximized();
#endif
#ifdef SIMULATION_CLAVIER_CLASSEMENT
    simulerClassement();
#endif
    connect(gestionPartie,
            &GestionPartie::estFinPartie,
            this,
            &IHMClassement::fermer);
}

IHMClassement::~IHMClassement()
{
    delete uiClassement;
    qDebug() << Q_FUNC_INFO << this << "gestionPartie" << gestionPartie;
}

/**
 * @brief S'exécute à chaque fois que la fenêtre est affichée
 *
 * @fn IHMPartie::showEvent
 *
 */
void IHMClassement::showEvent(QShowEvent* event)
{
    qDebug() << Q_FUNC_INFO << this;
    redimensionnerLabel();
    afficherClassement();
}

void IHMClassement::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
}

void IHMClassement::editerLabelChrono()
{
    uiClassement->labelTemps->setText(
      gestionPartie->convertirTemps(gestionPartie->recupererChronometre()));
}

void IHMClassement::editerLabelPremierJoueurNomJoueur(QString numeroJoueur)
{
    uiClassement->labelPJJoueur->setText("Joueur " + numeroJoueur);
}

void IHMClassement::editerLabelPremierJoueurScore(QString score)
{
    uiClassement->labelPJScore->setText("Score: " + score);
}

void IHMClassement::editerLabel(const QString& labelRecherche,
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

void IHMClassement::afficherClassement()
{
    qDebug() << Q_FUNC_INFO;
    std::vector<QList<QString> > classement;
    classement = gestionPartie->genererClassement();
    editerLabelChrono();

    for(int i = 0; i < NOMBRE_JOUEUR_MAX; ++i)
    {
        QString               numeroLigne = QString::number(i + 1);
        const QList<QString>& ligne       = classement[i];
        if(i == 0)
        {
            editerLabelPremierJoueurNomJoueur(ligne[JOUEUR]);
            editerLabelPremierJoueurScore(ligne[SCORE]);
        }
        for(int indexLabel = 0; indexLabel < NB_LABELS; ++indexLabel)
        {
            if(i < gestionPartie->recupererNombreJoueurs())
                editerLabel(nomLabels[indexLabel] + numeroLigne,
                            ligne[indexLabel]);
            else
                editerLabel(nomLabels[indexLabel] + numeroLigne, "");
            // vide les joueurs fantomes
        }
    }
}

void IHMClassement::appliquerMiseEnForme(QLayout* layout, int taillePolice)
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

void IHMClassement::redimensionnerLabel()
{
    int          tailleTitre = 30;
    int          tailleTexte = 20;
    QHBoxLayout* hLayoutPJ   = findChild<QHBoxLayout*>("hLayoutPJ");
    QVBoxLayout* vLayoutTableauClassement =
      findChild<QVBoxLayout*>("vLayoutTableauClassement");
    if(hLayoutPJ)
    {
        qDebug() << Q_FUNC_INFO << "Trouvé" << hLayoutPJ;
        appliquerMiseEnForme(hLayoutPJ, tailleTitre);
    }

    if(vLayoutTableauClassement)
    {
        qDebug() << Q_FUNC_INFO << "Trouvé" << vLayoutTableauClassement;
        appliquerMiseEnForme(vLayoutTableauClassement, tailleTexte);
    }
}

#ifdef SIMULATION_CLAVIER_CLASSEMENT
void IHMClassement::simulerClassement()
{
    // Touche Q -> fermer
    QAction* quitter = new QAction(this);
    quitter->setShortcut(QKeySequence(QKeySequence(Qt::Key_Q)));
    addAction(quitter);
    connect(quitter, SIGNAL(triggered()), this, SLOT(fermer()));
}
#endif
