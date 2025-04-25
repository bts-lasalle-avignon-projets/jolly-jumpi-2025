#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtBluetooth>
#include <QBluetoothLocalDevice>

class Bluetooth : public QObject
{
    Q_OBJECT
  public:
    Bluetooth();
    ~Bluetooth();
    void initialiserInterfaceLocal();
    void trouverPeripherique();
    void connecterPeripherique(QBluetoothSocket* socket);
    void deconnecterPeripherique(QBluetoothSocket* socket);
    void envoyerTrame(QString trame);

  private:
    QBluetoothDeviceInfo peripheriqueDistant; //[nombreTotalPeripherique];
    QBluetoothDeviceDiscoveryAgent* agentDecouverteBluetooth;
    QBluetoothSocket*               socket; //[nombreTotalPeripherique];

  private slots:
    void gererPeripherique(QBluetoothDeviceInfo peripherique);
};

#endif // BLUETOOTH_H
