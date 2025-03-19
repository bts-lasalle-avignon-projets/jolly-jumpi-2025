#ifndef BASE_DE_DONNEES_H
#define BASE_DE_DONNEES_H
#include "statistiques.h"

class BaseDeDonnees
{
  public:
    void enregistrerClassement();
    void enregistrerParticipants();
    void lireHistorique();
};

BaseDeDonnees BDD;

#endif