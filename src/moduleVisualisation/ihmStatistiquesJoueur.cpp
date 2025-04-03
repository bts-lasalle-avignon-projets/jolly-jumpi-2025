#include "ihmStatistiquesJoueur.h"
#include "ui_statistiquesJoueur.h"
#include "gestionPartie.h"
#include <QDebug>

IHMStatistiquesJoueur::IHMStatistiquesJoueur(QWidget* parent) :
    QWidget{ parent }, uiStatistiquesJoueur(new Ui::StatistiquesJoueur),
    gestionPartie(new GestionPartie(this))
{
    uiStatistiquesJoueur->setupUi(this);

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

IHMStatistiquesJoueur::~IHMStatistiquesJoueur()
{
    delete gestionPartie;
    delete uiStatistiquesJoueur;
    qDebug() << Q_FUNC_INFO << this;
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

#ifdef SIMULATION_CLAVIER_PARTIE
void IHMStatistiquesJoueur::simulerPartie()
{
    // Touche Q -> fermer
    QAction* quitter = new QAction(this);
    quitter->setShortcut(QKeySequence(QKeySequence(Qt::Key_Q)));
    addAction(quitter);
    connect(quitter, SIGNAL(triggered()), this, SLOT(fermer()));
}
#endif
