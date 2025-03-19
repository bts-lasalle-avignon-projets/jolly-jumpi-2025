#ifndef STATISTIQUES_H
#define STATISTIQUES_H
#include "joueur.h"
#include <vector>

class Statistiques
{
  public:
    std::vector<Joueur> classement;
    std::vector<Joueur> participants;
    Joueur              tableauDesJoueur[];
    // mettre nombreJoueur dans le tableau cause une erreur si pas défini

    void calculerEcartTempsEntreTir();
};

Statistiques statistiques;

#endif