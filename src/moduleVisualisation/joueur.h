#ifndef JOUEUR_H
#define JOUEUR_H

#include <QList>
#include "tir.h"

#define JOUEUR_INCONNU -1
#define AUCUN          0

class Joueur
{
  public:
    Joueur();
    ~Joueur();

  private:
    int        numero;
    int        score;
    int        trouPrefere;
    QList<Tir> tirs;
};

#endif
