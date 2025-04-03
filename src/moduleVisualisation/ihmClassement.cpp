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
}

void IHMClassement::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
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
