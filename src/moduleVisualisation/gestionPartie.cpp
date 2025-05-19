#include "gestionPartie.h"
#include "communication.h"
#include "joueur.h"
#include <QDebug>
#include <QTimer>

GestionPartie::GestionPartie(Communication* communication, QObject* parent) :
    QObject(parent), nombreJoueurs(0), modeDeJeu(0), etat(EtatPartie::INCONNU),
    duree(0.), communication(communication)
{
    qDebug() << Q_FUNC_INFO << this;

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
    /**
     * @todo Fin de partie
     */
    connect(communication,
            &Communication::abandonPartie,
            this,
            &GestionPartie::abandonnerPartie);
}

GestionPartie::~GestionPartie()
{
    supprimerJoueurs();
    qDebug() << Q_FUNC_INFO << this;
}

void GestionPartie::gererPartie()
{
    /**
     * @todo Initialiser la partie (état, joueurs, ...)
     */
    supprimerJoueurs();
#ifdef SIMULATION_MODULE_CONFIGURATION
    qDebug() << Q_FUNC_INFO << "SIMULATION_MODULE_CONFIGURATION";
    communication->simulerModuleConfiguration();
#endif
}

void GestionPartie::commencerPartie()
{
    qDebug() << Q_FUNC_INFO;

    if(etat != EtatPartie::CONFIGUREE)
        return;

    etat = EtatPartie::DEBUTEE;
    communication->envoyerDebutDePartie();
#ifdef SIMULATION_MODULE_CONFIGURATION
    QTimer::singleShot(10000,
                       this,
                       [this]()
                       {
                           finirPartie();
                       });
#endif
}

void GestionPartie::gererConfiguration(QString nombreJoueursRecu,
                                       QString modeDeJeuRecu)
{
    qDebug() << Q_FUNC_INFO << "nombreJoueursRecu" << nombreJoueursRecu
             << "modeDeJeuRecu" << modeDeJeuRecu;
    nombreJoueurs = nombreJoueursRecu.toInt();
    modeDeJeu     = modeDeJeuRecu.toInt();
    creerJoueurs();
    etat = EtatPartie::CONFIGUREE;
    configurerPiste();
}

void GestionPartie::creerJoueurs()
{
    qDebug() << Q_FUNC_INFO << "nombreJoueurs" << nombreJoueurs;
    /**
     * @fixme Si associé ou pas ? Attention : 3 joueurs (piste-1, piste-3 et
     * piste-5) !
     */
    for(int i = 0; i < nombreJoueurs; i++)
    {
        joueurs.push_back(new Joueur());
    }
}

void GestionPartie::supprimerJoueurs()
{
    for(int i = 0; i < joueurs.size(); ++i)
    {
        delete joueurs.at(i);
    }
    joueurs.clear();
}

void GestionPartie::configurerPiste()
{
    if(etat == EtatPartie::CONFIGUREE)
    {
        qDebug() << Q_FUNC_INFO;
        communication->envoyerModeDeJeu(modeDeJeu);
    }
}

void GestionPartie::receptionnerTir(const QString& numeroPiste,
                                    const QString& score)
{
    if(etat != EtatPartie::DEBUTEE)
        return;
    qDebug() << Q_FUNC_INFO << "numeroPiste" << numeroPiste << "score" << score;
    int temps = 0; // quand chrono crée, récupérer le temps
    /**
     * @fixme Quel est le lien entre l'index dans joueurs et le numeroPiste ?
     * Exemple : 3 joueurs (piste-1, piste-3 et piste-5) !
     */
    joueurs[numeroPiste.toInt() - 1]->ajouterTir(score.toInt(), temps);
    joueurs[numeroPiste.toInt() - 1]->afficherTirs();
}

void GestionPartie::finirPartie()
{
    qDebug() << Q_FUNC_INFO;
    communication->signalerFinDePartie();
    etat = EtatPartie::FINIE;
}

void GestionPartie::abandonnerPartie()
{
    qDebug() << Q_FUNC_INFO;
    etat = EtatPartie::ABANDONNEE;
}
