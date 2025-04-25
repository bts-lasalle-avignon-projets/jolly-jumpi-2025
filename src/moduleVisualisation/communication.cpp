#include "communication.h"
#include "adressesPeripheries.h"

Communication::Communication()
{
}

Communication::~Communication()
{
}
/***********Etablir connexions***********/

/***********Traiter trames***********/
void Communication::traiterTrame()
{
    const QString trame;

    if(estTrameValide(trame))
    {
        if(estTrameConfiguration(trame))
        {
            gererConfiguration();
        }
        else if(estTrameAssociation(trame))
        {
            gererAssociation();
        }
        else if(estDemandeChangementPage(trame))
        {
            gererChangementPage();
        }
        else
        {
            gererChangementPage();
        }
    }
}

bool Communication::estTrameValide(const QString& trame)
{
    if(trame.startsWith(carDebutTrame) && trame.endsWith(carFinTrame))
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

bool Communication::estTrameAssociation(const QString& trame)
{
    if(verifierTypeTrame(trame, carAssociation))
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

bool Communication::estDemandeChangementPage(const QString& trame)
{
    if(estDemandePageAccueil(trame) || estDemandePageHistorique(trame))
        return true;
    return false;
}

/***********Envoyer trames***********/
QString Communication::construireTrame(const QString& message)
{
    QString trame = carDebutTrame + message + carFinTrame;
    return trame;
}

void Communication::envoyerTrame(const QString& destinataire,
                                 const QString& trame)
{
}

void Communication::demanderAssociation(const QString& destinataire)
{
}

void Communication::confirmerAssociation(const QString& destinataire)
{
}

void Communication::envoyerModeDeJeu(const QString& destinataire,
                                     const QString& information)
{
    envoyerTrame(destinataire, construireTrame(information));
}

void Communication::envoyerDebutDePartie(const QString& destinataire)
{
    envoyerTrame(destinataire, construireTrame(carDebutPartie));
}

void Communication::signalerFinDePartie(const QString& destinataire)
{
    envoyerTrame(destinataire, construireTrame(carFinPartie));
}

/***********Gestion***********/
void Communication::gererConfiguration()
{
}

void Communication::gererAssociation()
{
}

void Communication::gererChangementPage()
{
}
