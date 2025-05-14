#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QStringList>
#include <QString>

#define CAR_CONFIGURATION   "C"
#define CAR_DEBUT_PARTIE    "D"
#define CAR_FIN_PARTIE      "F"
#define CAR_PAGE_ACCUEIL    "S0"
#define CAR_PAGE_HISTORIQUE "S1"
#define CAR_ASSOCIATION     "A"

#define TEST_COMMUNICATION

class Bluetooth;

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
    Communication(QObject* parent = nullptr);
    ~Communication();

    //////Etablir connexions//////

    //////Traiter messages//////
    bool verifierTypeMessage(const QString& message, const QString& caractere);
    bool estMessageConfiguration(const QString& message);
    bool estFinDePartie(const QString& message);
    bool estMessageAssociation(const QString& message);
    bool estDemandePageAccueil(const QString& message);
    bool estDemandePageHistorique(const QString& message);
    bool estDemandeChangementPage(const QString& message);

    //////Envoyer messages//////
    QString construireMessage(const QString& trame);
    void    envoyerMessage(const QString& destinataire, const QString& trame);
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

  private slots:
    void traiterMessage(QString trame);

  private:
    Bluetooth*  bluetooth;
    QStringList messages;
};

#endif // COMMUNICATION_H
