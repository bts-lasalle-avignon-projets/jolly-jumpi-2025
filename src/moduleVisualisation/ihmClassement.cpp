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
      QString::number(gestionPartie->recupererChronometre()) + "s");
}

void IHMClassement::editerLabelPremierJoueurNomJoueur(QString numeroJoueur)
{
    uiClassement->labelPJJoueur->setText("Joueur " + numeroJoueur);
}

void IHMClassement::editerLabelPremierJoueurScore(QString score)
{
    uiClassement->labelPJScore->setText("Score: " + score);
}

void IHMClassement::editerLabelNomJoueur(QString ligne, QString numeroJoueur)
{
    QString nomLabel = QString("labelJ") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(numeroJoueur);
    else
        qDebug() << Q_FUNC_INFO << "Label" << nomLabel << "non trouvé";
}

void IHMClassement::editerLabelScore(QString ligne, QString scoreJoueur)
{
    QString nomLabel = QString("labelS") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(scoreJoueur);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMClassement::editerLabelPlace(QString ligne, QString placeClassement)
{
    QString nomLabel = QString("labelP") + ligne;
    QLabel* label    = this->findChild<QLabel*>(nomLabel);

    if(label)
        label->setText(placeClassement);
    else
        qDebug() << Q_FUNC_INFO << "label" << nomLabel << "non trouvé";
}

void IHMClassement::afficherClassement()
{
    qDebug() << Q_FUNC_INFO;
    std::vector<QList<QString> > classement;
    classement = gestionPartie->genererClassement();
    editerLabelChrono();
    for(int i = 0; i < gestionPartie->recupererNombreJoueurs(); ++i)
    {
        QString        numero          = QString::number(i + 1);
        QList<QString> ligneClassement = classement[i];
        if(i == 0)
        {
            editerLabelPremierJoueurNomJoueur(ligneClassement[0]);
            editerLabelPremierJoueurScore(ligneClassement[1]);
        }

        editerLabelNomJoueur(numero, ligneClassement[0]);
        editerLabelScore(numero, ligneClassement[1]);
        editerLabelPlace(numero, ligneClassement[2]);
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
