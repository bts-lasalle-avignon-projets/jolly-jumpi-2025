#include "gestionPartie.h"
#include "joueur.h"
#include <QDebug>

GestionPartie::GestionPartie(QObject* parent) :
    QObject(parent), nombreJoueurs(0), modeDeJeu(0), etat(0), duree(0.)
{
    qDebug() << Q_FUNC_INFO << this;
    for(int i = 0; i < nombreJoueurs; ++i)
    {
        joueurs.push_back(new Joueur());
    }
}

GestionPartie::~GestionPartie()
{
    for(int i = 0; i < nombreJoueurs; ++i)
    {
        delete joueurs.at(i);
    }
    qDebug() << Q_FUNC_INFO << this;
}

void GestionPartie::commencerPartie()
{
    qDebug() << Q_FUNC_INFO;
}
