#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QStringList>
#include <QString>

class Communication
{
  public:
    enum TypeTrame
    {
        DemandeAssociation,
        ConfirmationAssociation,
        ModeDeJeu,
        DebutPartie,
        FinDePartie,
        AffichagePageAccueil,
        AffichagePageHistorique,
        NbTrames
    };

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
    bool estTrameFinDePartie();
    bool estDemandePageAccueil();
    bool estDemandePageHistorique();

  private:
    QStringList trames;
};

#endif // COMMUNICATION_H
