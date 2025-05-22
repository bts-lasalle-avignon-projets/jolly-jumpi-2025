#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QBluetoothServiceInfo>
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothLocalDevice>

class IHMModuleConfiguration;

class Bluetooth : public QObject
{
    Q_OBJECT

  public:
    Bluetooth(IHMModuleConfiguration* ihmModuleConfiguration);
    QBluetoothServer* getServer() const;

  public slots:
    void demarrerServeur();
    void nouvelleConnexion();
    void traiterDonnees();
    void deconnexion();

  private:
    IHMModuleConfiguration*  ihmModuleConfiguration;
    QBluetoothServer*        server;
    QList<QBluetoothSocket*> sockets;
};

#endif // BLUETOOTH_H
