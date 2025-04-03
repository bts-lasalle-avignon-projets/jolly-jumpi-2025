#include "ihmpartie.h"
#include "ui_partie.h"
#include "gestionPartie.h"
#include <QDebug>

IHMPartie::IHMPartie(QWidget* parent) :
    QWidget{ parent }, uiPartie(new Ui::Partie),
    gestionPartie(new GestionPartie(this))
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
}

IHMPartie::~IHMPartie()
{
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
}

void IHMPartie::fermer()
{
    qDebug() << Q_FUNC_INFO << this;
    this->close();
}

#ifdef SIMULATION_CLAVIER_PARTIE
void IHMPartie::simulerPartie()
{
    // Touche Q -> fermer
    QAction* quitter = new QAction(this);
    quitter->setShortcut(QKeySequence(QKeySequence(Qt::Key_Q)));
    addAction(quitter);
    connect(quitter, SIGNAL(triggered()), this, SLOT(fermer()));
}
#endif
