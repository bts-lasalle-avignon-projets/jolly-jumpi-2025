#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtBluetooth>
#include <QBluetoothLocalDevice>
#include <QMap>

// Configuration Bluetooth
#define ACTIVER_SERVEUR   true
#define ACTIVER_RECHERCHE false
#define RECHERCHE_CONTINU false
#define GESTION_APPAIRAGE false

// Définition des modes de connexion (client/serveur) pour les modules
#define SERVEUR 1 // Mode serveur (Pi) <- client (ESP32 master)
#define CLIENT  0 // Mode client (Pi) -> serveur (ESP32 slave)
// Choix du mode de connexion (client/serveur) pour les modules
#define MODULE_PISTE         SERVEUR
#define MODULE_CONFIGURATION CLIENT

// Pour le mode client (timeout)
#define DELAI_CONNEXION 10000

// Protocole
#define DEBUT_MESSAGE "$"
#define FIN_MESSAGE   "\n"

// Pour les tests
#define TEST_ASSOCIATION

// Nom du service
static const QString serviceNom(QStringLiteral("jolly-jumpi"));

class Bluetooth : public QObject
{
    Q_OBJECT
  public:
    Bluetooth();
    ~Bluetooth();

    void              demarrerServeur();
    int               getNbConnectes();
    void              trouverPeripherique();
    void              arreterRecherche();
    void              envoyerMessage(QString adresse, QString message);
    void              envoyerMessageGroupe(QString message);
    QString           recupererAdressePeripherique(QBluetoothSocket* socket);
    QBluetoothSocket* recupererSocketPeripherique(QString adresse);
    QString           recupererNomPeripherique(QBluetoothSocket* socket);
    QString           recupererNomPeripherique(QString adresse);
    bool              estPeripheriqueConnecte(QString adresse);

  private:
    QBluetoothLocalDevice            peripheriqueBluetoothLocal;
    QBluetoothDeviceDiscoveryAgent*  agentDecouverteBluetooth;
    QBluetoothServiceInfo            serviceBluetooth;
    QBluetoothServer*                serveur;
    QMap<QString, QBluetoothSocket*> sockets;
    QMap<QString, QString>           peripheriques;

    void initialiserInterfaceLocal();
    void arreterInterfaceLocal();
    void libererCommunications();
    void gererAppairage();
    void afficherAppairagePeripherique(QBluetoothDeviceInfo peripherique);
    void appairerPeripherique(QBluetoothDeviceInfo peripherique);
    bool estMessageValide(const QString& message);
    void nettoyerMessage(QString& message);

  private slots:
    void gererPeripheriqueDecouvert(QBluetoothDeviceInfo peripherique);
    void connecterPeripheriqueDecouvert(
      QBluetoothDeviceInfo
        peripherique); // Mode client -> serveur (ESP32 slave)
    void
    connecterPeripheriqueDecouvert(); // Mode serveur <- client (ESP32 master)
    void deconnecterPeripherique(QBluetoothDeviceInfo peripherique);
    void traiterMessage(QBluetoothDeviceInfo peripherique, QString message);
    void traiterMessage(QString nom, QString adresse, QString message);
    void afficherErreurServeur(QBluetoothServer::Error erreur);
    void afficherErreurSocket(QBluetoothSocket::SocketError erreur);
    void afficherEtat(QBluetoothSocket::SocketState etat);

  signals:
    void peripheriqueDistantTrouve(QBluetoothDeviceInfo peripherique);
    void peripheriqueDistantConnecte(QBluetoothDeviceInfo peripherique);
    void peripheriqueDistantDeconnecte(QBluetoothDeviceInfo peripherique);
    void peripheriqueDistantConnecte(QString nom, QString adresse);
    void peripheriqueDistantDeconnecte(QString nom, QString adresse);
    void rechercheTerminee();
    void messageRecu(QString nom, QString adresse, QString message);
};

#endif // BLUETOOTH_H
