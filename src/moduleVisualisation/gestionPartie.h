#ifndef GESTION_PARTIE_H
#define GESTION_PARTIE_H

#include <QObject>
#include <QVector>

class Joueur;

namespace Ui
{
class GestionPartie;
}

class GestionPartie : public QObject
{
    Q_OBJECT
  public:
    GestionPartie(QObject* parent);
    ~GestionPartie();
    void commencerPartie();

  private:
    int                nombreJoueurs;
    int                modeDeJeu;
    int                etat;
    double             duree;
    QVector<Joueur*>   joueurs;
    Ui::GestionPartie* ui;
};

#endif
