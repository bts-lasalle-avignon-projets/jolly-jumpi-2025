#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QStringList>
#include <QString>

class Communication
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
    void    envoyerTrame(const QString& destinataire, const QString& trame);
    void    traiterTrame();
    QString construireTrame(const QString& trame);
    void    demanderAssociation(const QString& destinataire);
    void    confirmerAssociation(const QString& destinataire);
    void    envoyerModeDeJeu(const QString& destinataire,
                             const QString& modeDeJeu);
    void    envoyerDebutDePartie(const QString& destinataire);
    void    signalerFinDePartie(const QString& destinataire);
    bool    estTrameConfiguration(const QString& trame);
    bool    estTrameFinDePartie(const QString& trame);
    bool    estDemandePageAccueil(const QString& trame);
    bool    estDemandePageHistorique(const QString& trame);
    bool    estTrameAssociation(const QString& trame);
    bool    verifierTypeTrame(const QString& trame, const QString& caractere);
    bool    estTrameValide(const QString& trame);
    bool    estDemandeChangementPage(const QString& trame);
    void    gererConfiguration();
    void    gererAssociation();

  private:
    QStringList       trames;
    const QString     carDebutTrame      = "$";
    const QString     carFinTrame        = "\n";
    const QString     carConfiguration   = "C";
    const QString     carDebutPartie     = "D";
    const QString     carFinPartie       = "F";
    const QString     carPageAccueil     = "S0";
    const QString     carPageHistorique  = "S1";
    const QString     carAssociation     = "A";
    const QStringList moduleDetection[8] = {
        "", "", "", "", "", "", "", ""
    }; // adresse des modules de détection de balle
    const QString moduleConfiguration = ; // adresse du module de configuration
};

#endif // COMMUNICATION_H
