#ifndef GESTION_PARTIE_H
#define GESTION_PARTIE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QMap>

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

    void gererPartie();

  private:
    int                    nombreJoueurs;
    int                    modeDeJeu;
    EtatPartie             etat;
    double                 duree;
    Ui::GestionPartie*     ui;
    Communication*         communication;
    bool                   configurationPisteTerminee;
    QMap<QString, Joueur*> joueurs;

    void creerJoueurs();
    void supprimerJoueurs();
    void configurerPiste();
    void relierPistesEtJoueurs();

  private slots:
    void commencerPartie();
    void gererConfiguration(QString nombreJoueursRecu, QString modeDeJeuRecu);
    void receptionnerTir(const QString& numeroPiste, const QString& score);
    void finirPartie();
    void abandonnerPartie();
};

#endif
