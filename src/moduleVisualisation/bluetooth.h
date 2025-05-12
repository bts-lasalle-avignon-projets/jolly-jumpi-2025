#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtBluetooth>
#include <QBluetoothLocalDevice>
#include <QMap>

#define DELAI_CONNEXION 10000

#define TEST_ASSOCIATION

static const QString serviceNom(QStringLiteral("jolly-jumpi"));

class Bluetooth : public QObject
{
    Q_OBJECT
  public:
    Bluetooth();
    ~Bluetooth();

    void              demarrerServeur();
    void              arreterServeur();
    int               getNbConnectes();
    void              trouverPeripherique();
    void              arreterRecherche();
    void              envoyerMessage(QString adresse, QString trame);
    void              envoyerMessageGroupe(QString trame);
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

  private slots:
    void gererPeripheriqueDecouvert(QBluetoothDeviceInfo peripherique);
    void connecterPeripheriqueDecouvert(
      QBluetoothDeviceInfo peripherique);  // client -> serveur (slave)
    void connecterPeripheriqueDecouvert(); // serveur <- client (master)
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
};

#endif // BLUETOOTH_H
