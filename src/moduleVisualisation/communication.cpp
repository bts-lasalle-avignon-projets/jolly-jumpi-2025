#include "communication.h"

Communication::Communication()
{
}

Communication::~Communication()
{
}

void Communication::envoyerTrame()
{
}

void Communication::traiterTrame()
{
}

void Communication::demanderAssociation()
{
}

void Communication::confirmerAssociation()
{
}

void Communication::envoyerModeDeJeu()
{
}

void Communication::envoyerDebutDePartie()
{
}

void Communication::signalerFinDePartie()
{
}

bool Communication::estTrameConfiguration(const QString& trame)
{
    if(verifierTypeTrame(trame, carConfiguration))
        return true;
    return false;
}

bool Communication::estTrameFinDePartie(const QString& trame)
{
    if(verifierTypeTrame(trame, carFinPartie))
        return true;
    return false;
}

bool Communication::estDemandePageAccueil(const QString& trame)
{
    if(verifierTypeTrame(trame, carPageAccueil))
        return true;
    return false;
}

bool Communication::estDemandePageHistorique(const QString& trame)
{
    if(verifierTypeTrame(trame, carPageHistorique))
        return true;
    return false;
}

bool Communication::estTrameAssociation(const QString& trame)
{
    if(verifierTypeTrame(trame, carAssociation))
        return true;
    return false;
}

bool Communication::verifierTypeTrame(const QString& trame,
                                      const QString& caractere)
{
    if(trame.contains(caractere))
        return true;
    return false;
}
