#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QLowEnergyController>
#include <QStringList>
#include <QString>

//// adresse du module de configuration////
#define MODULECONFIGURATION = "";

//// adresse des modules de détection de balle////
#define TABLE1 = "";
#define TABLE2 = "";
#define TABLE3 = "";
#define TABLE4 = "";
#define TABLE5 = "";
#define TABLE6 = "";
#define TABLE7 = "";
#define TABLE8 = "";

class Communication : public QObject
{
  public:
    /*enum TypeTrame
    {
        DemandeAssociation,
        ConfirmationAssociation,
        ModeDeJeu,
        DebutPartie,
        FinDePartie,
        AffichagePageAccueil,
        AffichagePageHistorique,
        NbTrames
    };*/

  public:
    Communication();
    ~Communication();

    //////Etablir connexions//////

    //////Traiter trames//////
    void traiterTrame();
    bool estTrameValide(const QString& trame);
    bool verifierTypeTrame(const QString& trame, const QString& caractere);
    bool estTrameConfiguration(const QString& trame);
    bool estTrameFinDePartie(const QString& trame);
    bool estTrameAssociation(const QString& trame);
    bool estDemandePageAccueil(const QString& trame);
    bool estDemandePageHistorique(const QString& trame);
    bool estDemandeChangementPage(const QString& trame);

    //////Envoyer trames//////
    QString construireTrame(const QString& trame);
    void    envoyerTrame(const QString& destinataire, const QString& trame);
    void    demanderAssociation(const QString& destinataire);
    void    confirmerAssociation(const QString& destinataire);
    void    envoyerModeDeJeu(const QString& destinataire,
                             const QString& modeDeJeu);
    void    envoyerDebutDePartie(const QString& destinataire);
    void    signalerFinDePartie(const QString& destinataire);

    //////Gestion//////
    void gererConfiguration();
    void gererAssociation();
    void gererChangementPage();

  private:
    QStringList   trames;
    const QString carDebutTrame     = "$";
    const QString carFinTrame       = "\n";
    const QString carConfiguration  = "C";
    const QString carDebutPartie    = "D";
    const QString carFinPartie      = "F";
    const QString carPageAccueil    = "S0";
    const QString carPageHistorique = "S1";
    const QString carAssociation    = "A";
};

#endif // COMMUNICATION_H
