#ifndef GESTION_PARTIE_H
#define GESTION_PARTIE_H
#include "joueur.h"

class GestionPartie
{
  public:
    int    nombreJoueur, modeDeJeu, etat;
    double duree;
    Joueur tableauDesJoueur[];
    // mettre nombreJoueur dans le tableau cause une erreur si pas défini

    void commencerPartie();
};

GestionPartie partie;

#endif