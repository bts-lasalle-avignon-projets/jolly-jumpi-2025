#ifndef GESTION_PARTIE_H
#define GESTION_PARTIE_H

#include <QObject>
#include <QVector>

class Joueur;
class Communication;

namespace Ui
{
class GestionPartie;
}

class GestionPartie : public QObject
{
    Q_OBJECT
  public:
    GestionPartie(Communication* communication, QObject* parent);
    ~GestionPartie();

  private:
    int                nombreJoueurs;
    int                modeDeJeu;
    int                etat;
    double             duree;
    QVector<Joueur*>   joueurs;
    Ui::GestionPartie* ui;
    Communication*     communication;
    bool               configurationPisteTerminee;

    void creerJoueurs();
    void initialiserJoueur(const int& numeroJoueur);
    void configurerPiste();

  private slots:
    void commencerPartie();
    void gererConfiguration(QString nombreJoueursRecu, QString modeDeJeuRecu);
};

#endif
