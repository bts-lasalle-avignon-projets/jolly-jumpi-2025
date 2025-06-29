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
    chronometrer();
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
    emit creerChevaux(nombreJoueurs);
}

void GestionPartie::supprimerJoueurs()
{
    joueurs.clear();
    communication->effacerPistes();
}

void GestionPartie::configurerPiste()
{
    if(etat == EtatPartie::CONFIGUREE)
    {
        qDebug() << Q_FUNC_INFO;
        communication->envoyerConfiguration(modeDeJeu, nombreJoueurs);
    }
}

void GestionPartie::relierPistesEtJoueurs()
{
    QList<QString> pistes = communication->recupererPistes();
    for(const QString& piste: pistes)
    {
        qDebug() << Q_FUNC_INFO << "piste" << piste;
        joueurs[piste] = new Joueur(piste.toInt());
    }
}

void GestionPartie::chronometrer()
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
                emit avancementChronometre(chronometre);
                if(chronometre % 5 == 0)
                    qDebug() << Q_FUNC_INFO << chronometre << "secondes";
            });

    minuteur->start(1000); // toutes les 1 secondes
}

QString GestionPartie::convertirTemps(const int& secondes)
{
    QString temps;
    QString minute  = QString::number(secondes / 60);
    QString seconde = QString::number(secondes % 60);
    if(seconde.toInt() < 10)
        temps = minute + ":0" + seconde;
    else
        temps = minute + ":" + seconde;
    return temps;
}

void GestionPartie::receptionnerTir(const QString& numeroPiste,
                                    const QString& score)
{
    int scoreTir = score.toInt();
    if(etat != EtatPartie::DEBUTEE)
        return;
    qDebug() << Q_FUNC_INFO << "numeroPiste" << numeroPiste << "joueur"
             << joueurs[numeroPiste]->recupererNumero() << "scoreTir"
             << scoreTir;
    if(etat != EtatPartie::FINIE || etat != EtatPartie::ABANDONNEE)
    {
        int pointManquant  = SCORE_MAX - joueurs[numeroPiste]->recupererScore();
        int scoreRecevable = 0;

        if(scoreTir <= pointManquant)
        {
            scoreRecevable = scoreTir;
            qDebug() << Q_FUNC_INFO << "pointManquant <= au scorTir";
            tempsAttenteFinAnimation = (SCORE_TIR_MAX + 1) * TEMPS_ANIMATION;
        }
        else
        {
            scoreRecevable           = pointManquant;
            tempsAttenteFinAnimation = (pointManquant + 1) * TEMPS_ANIMATION;
            qDebug() << Q_FUNC_INFO << "pointManquant > au scorTir";
        }
        emit tirRecu(QString::number(joueurs[numeroPiste]->recupererNumero()),
                     scoreRecevable);
    }
    joueurs[numeroPiste]->ajouterTir(scoreTir, chronometre);
    joueurs[numeroPiste]->afficherTirs();
    joueurs[numeroPiste]->definirScore(scoreTir);

    if(estScoreMax(joueurs[numeroPiste]->recupererScore()))
    {
        finirPartie();
    }
}

void GestionPartie::finirPartie()
{
    communication->arreterPartie();
    // Le timer permet à IHMPartie de recevoir le dernier tir
    QTimer::singleShot(
      tempsAttenteFinAnimation,
      this,
      [this]()
      {
          qDebug() << Q_FUNC_INFO << "partie fini, reception score fermee";
          etat = EtatPartie::FINIE;

          emit demandeClassement();
          QTimer::singleShot(
            TEMPS_AFFICHAGE_FENETRE * 1000,
            this,
            [this]()
            {
                qDebug() << Q_FUNC_INFO << "afficher statistique";
                emit demandeStatistiquesJoueur();
                QTimer::singleShot(
                  TEMPS_AFFICHAGE_FENETRE * nombreJoueurs * 1000,
                  this,
                  [this]()
                  {
                      qDebug() << Q_FUNC_INFO << "afficher statistique";
                      emit estFinPartie();
                      communication->signalerFinDePartie();
                  });
            });
      });
}

void GestionPartie::abandonnerPartie()
{
    qDebug() << Q_FUNC_INFO;
    supprimerJoueurs();
    etat = EtatPartie::ABANDONNEE;
    emit estFinPartie();
    communication->arreterPartie();
    communication->signalerFinDePartie();
}

int GestionPartie::calculerScoreJoueur(const QString& numeroPiste)
{
    int score = 0;
    for(const Tir& tir: joueurs[numeroPiste]->recupererTirs())
    {
        score += tir.recupererScore();
    }
    qDebug() << Q_FUNC_INFO << "joueur"
             << joueurs[numeroPiste]->recupererNumero() << "score" << score;
    return score;
}

bool GestionPartie::estScoreMax(const int& score)
{
    if(score >= SCORE_MAX)
        return true;
    return false;
}

std::vector<QList<QString> > GestionPartie::genererClassement()
{
    QList<Joueur*> listeJoueurs = joueurs.values();
    std::sort(listeJoueurs.begin(),
              listeJoueurs.end(),
              [](Joueur* a, Joueur* b)
              {
                  return a->recupererScore() > b->recupererScore();
              });

    std::vector<QList<QString> > classement;
    int                          placeClassement = 1;
    for(Joueur* joueur: listeJoueurs)
    {
        joueur->definirPlace(placeClassement);
        QList<QString> ligneClassement;
        ligneClassement << QString::number(joueur->recupererPlace());
        ligneClassement << QString::number(joueur->recupererNumero());
        ligneClassement << QString::number(joueur->recupererScore());
        classement.push_back(ligneClassement);
        placeClassement++;
        qDebug() << Q_FUNC_INFO << ligneClassement;
    }
    return classement;
}

std::vector<QList<QString> > GestionPartie::recupererStatistiquesJoueur(
  const QString& numeroJoueur)
{
    qDebug() << Q_FUNC_INFO << numeroJoueur;
    return joueurs[numeroJoueur]->recupererStatistiquesJoueur();
}

int GestionPartie::recupererNombreJoueurs()
{
    return nombreJoueurs;
}

int GestionPartie::recupererScoreJoueur(QString numero)
{
    return joueurs[numero]->recupererScore();
}

int GestionPartie::recupererPlaceJoueur(QString numero)
{
    return joueurs[numero]->recupererPlace();
}

int GestionPartie::recupererScoreMax()
{
    return SCORE_MAX;
}

int GestionPartie::recupererTempsAffichageFenetre()
{
    return TEMPS_AFFICHAGE_FENETRE;
}
