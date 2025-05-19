#include "gestionPartie.h"
#include "communication.h"
#include "joueur.h"
#include <QDebug>

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
    demarrerChronometre();
#ifdef SIMULATION_MODULE_CONFIGURATION
    QTimer::singleShot(10000,
                       this,
                       [this]()
                       {
                           finirPartie();
                       });
#endif
}

int GestionPartie::recupererChronometre()
{
    return chronometre;
}

void GestionPartie::gererConfiguration(QString nombreJoueursRecu,
                                       QString modeDeJeuRecu)
{
    qDebug() << Q_FUNC_INFO << "nombreJoueursRecu" << nombreJoueursRecu
             << "modeDeJeuRecu" << modeDeJeuRecu;
    nombreJoueurs = nombreJoueursRecu.toInt();
    modeDeJeu     = modeDeJeuRecu.toInt();
    etat          = EtatPartie::CONFIGUREE;
    configurerPiste();
}

void GestionPartie::supprimerJoueurs()
{
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

void GestionPartie::demarrerChronometre()
{
    chronometre      = 0;
    QTimer* minuteur = new QTimer(this);
    connect(minuteur,
            &QTimer::timeout,
            this,
            [this, minuteur]()
            {
                if(etat != EtatPartie::DEBUTEE)
                {
                    minuteur->stop();
                    return;
                }
                chronometre++;
                qDebug() << Q_FUNC_INFO << chronometre << "secondes";
            });

    minuteur->start(1000); // toutes les 1 secondes
}

void GestionPartie::receptionnerTir(const QString& numeroPiste,
                                    const QString& score)
{
    if(etat != EtatPartie::DEBUTEE)
        return;
    qDebug() << Q_FUNC_INFO << "numeroPiste" << numeroPiste << "joueur"
             << joueurs[numeroPiste]->getNumero() << "score" << score;
    joueurs[numeroPiste]->ajouterTir(score.toInt(), chronometre);
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
