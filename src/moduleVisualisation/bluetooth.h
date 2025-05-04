#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtBluetooth>
#include <QBluetoothLocalDevice>
#include <QMap>

class Bluetooth : public QObject
{
    Q_OBJECT
  public:
    Bluetooth();
    ~Bluetooth();
    void initialiserAdressePeripherique();
    void initialiserInterfaceLocal();
    void trouverPeripherique();
    void envoyerMessage(QString adresse, QString trame);
    bool estToutConnecte();
    void arreterRecherche();
    void envoyerMessageGroupe(QString trame);

  private:
    QBluetoothDeviceInfo peripheriqueDistant; //[NOMBRE_TOTAL_PERIPHERIQUE];
    QBluetoothDeviceDiscoveryAgent* agentDecouverteBluetooth;
    QList<QString>                   listeAdressePistes;
    QMap<QString, QBluetoothSocket*> sockets;

  private slots:
    void gererPeripherique(QBluetoothDeviceInfo peripherique);
    void connecterPeripherique(QBluetoothDeviceInfo peripherique);
    void deconnecterPeripherique(QBluetoothDeviceInfo peripherique);
    void traiterMessage(QBluetoothDeviceInfo peripherique, QString message);
    void afficherErreurSocket(QBluetoothSocket::SocketError erreur);

  signals:
    void peripheriqueDistantTrouve(QBluetoothDeviceInfo peripherique);
    //void peripheriqueDistantDeconnecte(QBluetoothDeviceInfo peripherique);
    //void donneesReceptionnees(QBluetoothDeviceInfo peripherique, QString message);
};

#endif // BLUETOOTH_H
