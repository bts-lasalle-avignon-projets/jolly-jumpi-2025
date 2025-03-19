#ifndef STATISTIQUES_H
#define STATISTIQUES_H
#include "joueur.h"
#include <vector>
#include <iostream>

class Communication
{
  private:
    std::string trameDemanderAssociation;
    std::string trameConfirmerAssociation;
    std::string trameModeDeJeu;
    std::string trameDebuterPartie;
    std::string trameFinDePartie;
    std::string trameAfficherPageAccueil;
    std::string trameAfficherPageHistorique;

  public:
    void envoyerTrame();
    void traiterTrame();
    void demanderAssociation();
    void confirmerAssociation();
    void envoyerModeDeJeu();
    void envoyerDebutDePartie();
    void signalerFinDePartie();
    bool estTrameFinDePartie();
    bool estDemandePageAccueil();
    bool estDemandePageHistorique();
};

Communication communication;

#endif