#include "gestionPartie.h"
#include "communication.h"
#include "joueur.h"
#include <QDebug>

GestionPartie::GestionPartie(QObject* parent) :
    QObject(parent), nombreJoueurs(0), modeDeJeu(0), etat(0), duree(0.),
    communication(new Communication)
{
    qDebug() << Q_FUNC_INFO << this;
    /*for(int i = 0; i < nombreJoueurs; ++i)
    {
        joueurs.push_back(new Joueur());
    }*/
    /*connect(communication,
            &Communication::configurationPrete,
            this,
            &GestionPartie::gererConfiguration);*/
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

void GestionPartie::gererConfiguration(QString nombreJoueursRecu,
                                       QString modeDeJeuRecu)
{
    qDebug() << Q_FUNC_INFO << "Nombre joueur" << nombreJoueursRecu
             << "Mode de jeu" << modeDeJeuRecu;
    nombreJoueurs = nombreJoueursRecu.toInt();
    modeDeJeu     = modeDeJeuRecu.toInt();
    for(int i = 0; i < nombreJoueurs; i++)
    {
        creerJoueur();
        configurerPiste(QString::number(i));
    }
}

void GestionPartie::creerJoueur()
{
    qDebug() << Q_FUNC_INFO << "Joueur créé";
    joueurs.push_back(new Joueur());
}

void GestionPartie::initialiserJoueur()
{
    qDebug() << Q_FUNC_INFO << "Initialisation joueur";
    for(int i = 0; i < joueurs.size(); i++)
    {
        /*joueurs[i]->numero = i; //@TODO voir comment touché au private
        joueurs[i]->score  = 0;
        joueurs[i]->tirs.clear();*/
        qDebug() << Q_FUNC_INFO << "Joueur" << i << "initialisé";
    }
}

void GestionPartie::configurerPiste(const QString& numeroPiste)
{
    for(int i = 1; i < numeroPiste.toInt(); i++)
    {
        /*communication->demanderConfirmationAssociation(numeroPiste);
        communication->envoyerModeDeJeu(numeroPiste,
                                        QString::number(modeDeJeu));*/
    }
}
