#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QList>

class IHMModuleConfiguration;

class Bluetooth : public QObject
{
    Q_OBJECT

  public:
    explicit Bluetooth(IHMModuleConfiguration* ihmModuleConfiguration);
    ~Bluetooth();

    void envoyer(const QString& trame);
    void arreterServeur();

  signals:
    void associationReussie();
    void partieDemarree();
    void partieTerminee(); // optionnel

  private slots:
    void clientConnecte();
    void deconnexion();

  private:
    void demarrerServeur();

    QBluetoothServer*        server;
    QList<QBluetoothSocket*> sockets;
    IHMModuleConfiguration*  ihmModuleConfiguration;
};

#endif // BLUETOOTH_H
