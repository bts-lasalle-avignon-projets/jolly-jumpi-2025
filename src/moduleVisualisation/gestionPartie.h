#ifndef GESTION_PARTIE_H
#define GESTION_PARTIE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QLabel>

#define RPI

#define NOMBRE_JOUEUR_MAX       8
#define SCORE_MAX               10
#define TEMPS_AFFICHAGE_FENETRE 3   // en seconde
#define TEMPS_ANIMATION         400 // mini seconde
#define SCORE_TIR_MAX           5

class Joueur;
class Communication;
class Joueur;

namespace Ui
{
class GestionPartie;
}

class GestionPartie : public QObject
{
    Q_OBJECT
  public:
    enum EtatPartie
    {
        INCONNU = -1,
        CONFIGUREE,
        DEBUTEE,
        FINIE,
        ABANDONNEE,
        NB_ETATS
    };

  public:
    GestionPartie(Communication* communication, QObject* parent);
    ~GestionPartie();

    void                         gererPartie();
    std::vector<QList<QString> > genererClassement();
    std::vector<QList<QString> > recupererStatistiquesJoueur(
      const QString& numeroJoueur);
    int     recupererNombreJoueurs();
    int     recupererChronometre();
    int     recupererScoreJoueur(QString numero);
    int     recupererPlaceJoueur(QString numero);
    QString convertirTemps(const int& secondes);
    int     recupererScoreMax();
    int     recupererTempsAffichageFenetre();

  private:
    int                    nombreJoueurs;
    int                    modeDeJeu;
    EtatPartie             etat;
    double                 duree;
    Ui::GestionPartie*     ui;
    Communication*         communication;
    bool                   configurationPisteTerminee;
    QMap<QString, Joueur*> joueurs;
    int                    chronometre;
    int                    tempsAttenteFinAnimation;

    void supprimerJoueurs();
    void configurerPiste();
    void relierPistesEtJoueurs();
    void chronometrer();
    int  calculerScoreJoueur(const QString& numeroPiste);
    bool estScoreMax(const int& score);

  private slots:
    void commencerPartie();
    void gererConfiguration(QString nombreJoueursRecu, QString modeDeJeuRecu);
    void receptionnerTir(const QString& numeroPiste, const QString& score);
    void finirPartie();
    void abandonnerPartie();

  signals:
    // void changementEtatPartie(EtatPartie etatPartie);
    void tirRecu(QString numeroJoueur, int scoreTir);
    void demandeClassement();
    void demandeStatistiquesJoueur();
    void estFinPartie();
    void creerChevaux(int nombreJoueurs);
    void avancementChronometre(int);
};

#endif
