#ifndef GESTION_PARTIE_H
#define GESTION_PARTIE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QMap>
#include <QTimer>
#include <QLabel>

#define SCORE_MAX 10

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

    void supprimerJoueurs();
    void configurerPiste();
    void relierPistesEtJoueurs();
    void chronometrer();
    int  recupererChronometre();
    int  calculerScoreJoueur(const QString& numeroPiste);
    bool estScoreMax(const int& score);

  private slots:
    void commencerPartie();
    void gererConfiguration(QString nombreJoueursRecu, QString modeDeJeuRecu);
    void receptionnerTir(const QString& numeroPiste, const QString& score);
    void finirPartie();
    void abandonnerPartie();

  signals:
    void changementEtatPartie(EtatPartie etatPartie);
    void tirRecu(int joueur, int scoreTir);
};

#endif
