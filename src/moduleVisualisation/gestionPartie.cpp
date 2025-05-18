#include "gestionPartie.h"
#include "communication.h"
#include "joueur.h"
#include <QDebug>
#include <QTimer>

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
    connect(communication,
            &Communication::scoreRecu,
            this,
            &GestionPartie::receptionnerTir);
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

    communication->envoyerDebutDePartie();
#ifdef SIMULATION_MODULE_CONFIG
    QTimer::singleShot(10000,
                       this,
                       [this]()
                       {
                           communication->signalerFinDePartie();
                       });
#endif
}

void GestionPartie::gererConfiguration(QString nombreJoueursRecu,
                                       QString modeDeJeuRecu)
{
    qDebug() << Q_FUNC_INFO << "Nombre joueur" << nombreJoueursRecu
             << "Mode de jeu" << modeDeJeuRecu;
    nombreJoueurs = nombreJoueursRecu.toInt();
    modeDeJeu     = modeDeJeuRecu.toInt();
    creerJoueurs();
    QTimer::singleShot(1000,
                       this,
                       [this]()
                       {
                           configurerPiste();
                       });
    // configurerPiste();
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

void GestionPartie::configurerPiste()
{
    communication->envoyerModeDeJeu(modeDeJeu);
}

void GestionPartie::receptionnerTir(const QString& numeroPiste,
                                    const QString& score)
{
    qDebug() << Q_FUNC_INFO << numeroPiste << score;
    int temps = 0; // quand chrono crée, récupérer le temps
    joueurs[numeroPiste.toInt() - 1]->ajouterTir(score.toInt(), temps);
    joueurs[numeroPiste.toInt() - 1]->afficherTirs();
}
