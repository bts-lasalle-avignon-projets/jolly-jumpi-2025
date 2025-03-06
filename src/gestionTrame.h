#ifndef GESTION_TRAME_H
#define GESTION_TRAME_H

#include "iostream"

#define ENTETE_TRAME_MODULE_DETECTION "@"
#define ENTETE_TRAME_MODULE_CONFIGURATION "$"
#define ENTETE_TRAME_MODULE_VISUALISATION "$"

#define DEBUT_TRAME_APPAIRAGE "A"
#define DEBUT_TRAME_CONFIGURATION "C"
#define DEBUT_TRAME_DEBUT "D"
#define DEBUT_TRAME_FIN "F"
#define DEBUT_TRAME_STATISTIQUES "S"

#define PIED_TRAME "\n"

#define SEPARATEUR_TRAME ";"

/* En attente du Blutooth
#define ADRESSE_MODULE_DETECTION ""
#define ADRESSE_TRAME_MODULE_CONFIGURATION ""
#define ADRESSE_TRAME_MODULE_VISUALISATION ""*/

void recuperTrame();
std::string transcrireTrame(const std::string& trame); //dans un tableau

bool estTrameModuleDetection(const std::string& trame);
bool estTrameAppairage(const std::string& trame);
bool estTrameScore(const std::string& trame);
bool estTrameModuleConfiguration(const std::string& trame);
bool estTrameConfiguration(const std::string& trame);
bool estTrameStatistiquesGlobales(const std::string& trame);
bool estTramePageAccueil(const std::string& trame);

std::string composerTrame(const std::string& contenue);
void envoyerTrame(const int& destination, const std::string& trame);


#endif