#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtBluetooth>
#include <QBluetoothLocalDevice>
#include <QMap>

#define ADRESSE_MODULE_TEST "18:67:B0:5F:8D:5B"

class Bluetooth : public QObject
{
    Q_OBJECT
  public:
    Bluetooth();
    ~Bluetooth();

    void initialiserInterfaceLocal();
    void trouverPeripherique();
    void deconnecterPeripherique(QBluetoothSocket* socket);
    void envoyerTrame(QString adresse, QString trame);

  private:
    QBluetoothDeviceInfo peripheriqueDistant; //[nombreTotalPeripherique];
    QBluetoothDeviceDiscoveryAgent* agentDecouverteBluetooth;

    QMap<QString, QBluetoothSocket*> sockets;

  private slots:
    void gererPeripherique(QBluetoothDeviceInfo peripherique);
    void connecterPeripherique(QBluetoothDeviceInfo peripherique);

  signals:
    void peripheriqueDistantTrouve(QBluetoothDeviceInfo peripherique);
};

#endif // BLUETOOTH_H
