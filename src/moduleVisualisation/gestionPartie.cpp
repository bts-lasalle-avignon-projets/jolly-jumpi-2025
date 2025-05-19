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
    relierPistesEtJoueurs();
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
    relierPistesEtJoueurs();
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
    // creerJoueurs();
    etat = EtatPartie::CONFIGUREE;
    configurerPiste();
}

/*void GestionPartie::creerJoueurs()
{
    qDebug() << Q_FUNC_INFO << "nombreJoueurs" << nombreJoueurs;
    for(int i = 0; i < nombreJoueurs; i++)
    {
        joueurs.push_back(new Joueur());
    }
    // joueur relié à une piste dans relierPistesJoueurs()
}*/

void GestionPartie::supprimerJoueurs()
{
    /*for(int i = 0; i < joueurs.size(); ++i)
    {
        delete joueurs.at(i);
    }*/
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

void GestionPartie::relierPistesEtJoueurs()
{
    QList<QString> pistes = communication->recupererPistes();
    for(const QString& piste: pistes)
    {
        qDebug() << Q_FUNC_INFO << "Piste" << piste;
        joueurs[piste] = new Joueur(piste.toInt());
    }
}

void GestionPartie::receptionnerTir(const QString& numeroPiste,
                                    const QString& score)
{
    if(etat != EtatPartie::DEBUTEE)
        return;
    qDebug() << Q_FUNC_INFO << "numeroPiste" << numeroPiste << "joueur"
             << joueurs[numeroPiste]->getNumero() << "score" << score;
    int temps = 0; // quand chrono crée, récupérer le temps
    joueurs[numeroPiste]->ajouterTir(score.toInt(), temps);
    joueurs[numeroPiste]->afficherTirs();
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
