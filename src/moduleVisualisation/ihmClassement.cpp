#include "ihmClassement.h"
#include "ui_classement.h"
#include "gestionPartie.h"
#include <QDebug>

IHMClassement::IHMClassement(QWidget* parent) :
    QWidget{ parent }, uiClassement(new Ui::Classement),
    gestionPartie(new GestionPartie(this))
{
    uiClassement->setupUi(this);

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

IHMClassement::~IHMClassement()
{
    delete gestionPartie;
    delete uiClassement;
    qDebug() << Q_FUNC_INFO << this;
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
}

void IHMClassement::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
}

#ifdef SIMULATION_CLAVIER_PARTIE
void IHMClassement::simulerPartie()
{
    // Touche Q -> fermer
    QAction* quitter = new QAction(this);
    quitter->setShortcut(QKeySequence(QKeySequence(Qt::Key_Q)));
    addAction(quitter);
    connect(quitter, SIGNAL(triggered()), this, SLOT(fermer()));
}
#endif
