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
    void envoyerTrame();
    void traiterTrame();
    void demanderAssociation();
    void confirmerAssociation();
    void envoyerModeDeJeu();
    void envoyerDebutDePartie();
    void signalerFinDePartie();
    bool estTrameConfiguration(const QString& trame);
    bool estTrameFinDePartie(const QString& trame);
    bool estDemandePageAccueil(const QString& trame);
    bool estDemandePageHistorique(const QString& trame);
    bool estTrameAssociation(const QString& trame);

  private:
    QStringList   trames;
    const QString carConfiguration  = "C";
    const QString carFinDePartie    = "F";
    const QString carPageAccueil    = "S0";
    const QString carPageHistorique = "S1";
    const QString carAssociation    = "A";
};

#endif // COMMUNICATION_H
