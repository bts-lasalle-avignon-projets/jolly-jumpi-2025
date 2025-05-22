#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <iostream>

class communication
{
  public:
    void lancerAppairage();
    void configurerPartie();
    void demanderAffichageStatistique();
    // void afficherStatistique();
    void affichageAccueil();
    void interromprePartie();
    void signalerAssociation();
    void signalerDemarrage();
    void signalerFin();
};

#endif
