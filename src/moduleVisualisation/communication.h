#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QStringList>
#include <QString>

// Simulation du module de configuration
//#define SIMULATION_MODULE_CONFIGURATION
//#define SIMULATION_MODULE_CONFIGURATION_ABANDON

// Protocole
#define DEBUT_MESSAGE "$"
#define FIN_MESSAGE   "\n"

#define PREMIER_ELEMENT  0
#define DEUXIEME_ELEMENT 1

// Pour la configuration
#define MODE_DE_JEU   PREMIER_ELEMENT
#define NOMBRE_JOUEUR DEUXIEME_ELEMENT

// Pour l'association
#define NOMBRE_PISTES DEUXIEME_ELEMENT

// Pour les tirs
#define NUMERO_PISTE PREMIER_ELEMENT
#define SCORE_TIR    DEUXIEME_ELEMENT

class Bluetooth;

class Communication : public QObject
{
    Q_OBJECT
  public:
    enum TypeMessage
    {
        INCONNU = -1,
        CONFIGURATION_PARTIE,
        CONFIGURATION_PISTE,
        DEMARRER_PARTIE,
        FINIR_PARTIE,
        DEBUT_PARTIE,
        FIN_PARTIE,
        PAGE_ACCUEIL,
        PAGE_HISTORIQUE,
        ASSOCIATION,
        TIR,
        NB_MESSAGES
    };

  public:
    Communication(QObject* parent = nullptr);
    ~Communication();

    QString construireMessage(const QString& trame);
    void    envoyerMessage(const QString& destinataire, const QString& trame);
    void    envoyerMessageGroupe(const QString& message);
    void    demanderConfirmationAssociation();
    void    confirmerAssociation(const QString& retourAssociation);
    void    envoyerConfiguration(const int& modeDeJeu, const int& nombreJoueur);
    void    envoyerDebutDePartie();
    void    signalerFinDePartie();

    void    communiquerConfiguration(QString message);
    void    gererAssociation(const QString& message);
    void    gererChangementPage();
    QString nettoyerMessage(const QString& message);
    QString extraireElement(const QString& informations, const int& element);
    void    communiquerTirJoueur(const QString& message);
    QList<QString> recupererPistes();
    void           effacerPistes();
#ifdef SIMULATION_MODULE_CONFIGURATION
    void simulerModuleConfiguration();
#endif

  private slots:
    void traiterMessage(QString nom, QString adresse, QString message);

  private:
    Bluetooth*     bluetooth;
    QStringList    typesMessages; // une QMap ?
    QList<QString> pistes;
    bool           estMessageValide(const QString& message);
    QString        nettoyerSeparateursMessage(QString& message);
    TypeMessage    identifierTypeMessage(const QString& message);
    bool verifierTypeMessage(const QString& message, const QString& caractere);
    bool estMessageConfiguration(const QString& message);
    bool estFinDePartie(const QString& message);
    bool estMessageAssociation(const QString& message);
    bool estDemandePageAccueil(const QString& message);
    bool estDemandePageHistorique(const QString& message);
    bool estDemandeChangementPage(const QString& message);

  signals:
    void moduleConnectes();
    void configurationRecue(QString nombreJoueursRecu, QString modeDeJeuRecu);
    void scoreRecu(QString numeroPiste, QString scoreTir);
    void partieDemarree();
    void estEnAttenteDeConfiguration();
    void abandonPartie();
};

#endif // COMMUNICATION_H
