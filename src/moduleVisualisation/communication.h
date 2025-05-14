#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QStringList>
#include <QString>

#define TEST_COMMUNICATION

class Bluetooth;

class Communication : public QObject
{
  public:
    enum TypeMessage
    {
        INCONNU = -1,
        CONFIGURATION,
        DEBUT_PARTIE,
        FIN_PARTIE,
        PAGE_ACCUEIL,
        PAGE_HISTORIQUE,
        ASSOCIATION,
        NB_MESSAGES
    };

  public:
    Communication(QObject* parent = nullptr);
    ~Communication();

    QString construireMessage(const QString& trame);
    void    envoyerMessage(const QString& destinataire, const QString& trame);
    void    demanderAssociation(const QString& destinataire);
    void    confirmerAssociation(const QString& destinataire);
    void    envoyerModeDeJeu(const QString& destinataire,
                             const QString& modeDeJeu);
    void    envoyerDebutDePartie(const QString& destinataire);
    void    signalerFinDePartie(const QString& destinataire);

    void gererConfiguration();
    void gererAssociation(const QString& adresse);
    void gererChangementPage();

  private slots:
    void traiterMessage(QString nom, QString adresse, QString message);

  private:
    Bluetooth*  bluetooth;
    QStringList typesMessages; // une QMap ?

    TypeMessage identifierTypeMessage(const QString& message);
    bool verifierTypeMessage(const QString& message, const QString& caractere);
    bool estMessageConfiguration(const QString& message);
    bool estFinDePartie(const QString& message);
    bool estMessageAssociation(const QString& message);
    bool estDemandePageAccueil(const QString& message);
    bool estDemandePageHistorique(const QString& message);
    bool estDemandeChangementPage(const QString& message);
};

#endif // COMMUNICATION_H
