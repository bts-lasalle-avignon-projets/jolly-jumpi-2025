#include "gestionPartie.h"
#include "communication.h"
#include "joueur.h"
#include <QDebug>

GestionPartie::GestionPartie(Communication* communication, QObject* parent) :
    QObject(parent), nombreJoueurs(0), modeDeJeu(0), etat(0), duree(0.),
    communication(communication)
{
    qDebug() << Q_FUNC_INFO << this;
    /*for(int i = 0; i < nombreJoueurs; ++i)
    {
        joueurs.push_back(new Joueur());
    }*/
    connect(communication,
            &Communication::configurationRecue,
            this,
            &GestionPartie::gererConfiguration);
    connect(communication,
            &Communication::partieDemarree,
            this,
            &GestionPartie::commencerPartie);
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
    qDebug() << Q_FUNC_INFO << "La partie démarre";
}

void GestionPartie::gererConfiguration(QString nombreJoueursRecu,
                                       QString modeDeJeuRecu)
{
    qDebug() << Q_FUNC_INFO << "Nombre joueur" << nombreJoueursRecu
             << "Mode de jeu" << modeDeJeuRecu;
    nombreJoueurs = nombreJoueursRecu.toInt();
    modeDeJeu     = modeDeJeuRecu.toInt();
    creerJoueurs();
    configurerPiste();
}

void GestionPartie::creerJoueurs()
{
    qDebug() << Q_FUNC_INFO << "Joueurs créés";
    for(int i = 0; i < nombreJoueurs; i++)
    {
        joueurs.push_back(new Joueur());
        initialiserJoueur(i);
    }
}

void GestionPartie::initialiserJoueur(const int& numeroJoueur)
{
    qDebug() << Q_FUNC_INFO << "Initialisation joueur";
    /*joueurs[i]->numero = i; //@TODO faire fonction chez Joueur
    joueurs[i]->score  = 0;
    joueurs[i]->tirs.clear();*/

    qDebug() << Q_FUNC_INFO << "Joueur" << numeroJoueur << "initialisé";
}

void GestionPartie::configurerPiste()
{
    communication->envoyerModeDeJeu(modeDeJeu);
}
