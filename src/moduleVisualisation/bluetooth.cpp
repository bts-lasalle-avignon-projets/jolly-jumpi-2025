#include "bluetooth.h"
#include "adressesPeripheries.h"

Bluetooth::Bluetooth()
{
    initialiserInterfaceLocal();
    trouverPeripherique();
}

Bluetooth::~Bluetooth()
{
}

void Bluetooth::initialiserInterfaceLocal()
{
    QBluetoothLocalDevice peripheriqueBluetoothLocal;
    if(peripheriqueBluetoothLocal.isValid())
    {
        peripheriqueBluetoothLocal.powerOn();

        // L'interface a un nom et une adresse MAC
        qDebug() << Q_FUNC_INFO << peripheriqueBluetoothLocal.name()
                 << peripheriqueBluetoothLocal.address().toString();
    }

    QBluetoothSocket* socket = nullptr;

    if(socket == nullptr)
    {
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    }
}

void Bluetooth::trouverPeripherique()
{
    QBluetoothDeviceDiscoveryAgent* agentDecouverteBluetooth = nullptr;

    if(agentDecouverteBluetooth == nullptr)
    {
        agentDecouverteBluetooth = new QBluetoothDeviceDiscoveryAgent(this);
    }

    connect(agentDecouverteBluetooth,
            SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this,
            SLOT(gererPeripherique(QBluetoothDeviceInfo)));

    agentDecouverteBluetooth->start();
}

void Bluetooth::gererPeripherique(QBluetoothDeviceInfo peripherique)
{
    qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
             << peripherique.address();

    // Tester si le périphérique découvert est celui avec lequel on souhaite
    // communiquer Alors on peut conserver le périphérique comme ceci
    peripheriqueDistant = peripherique;
    // Et on pourra alors arrêter la découverte avec
    // agentDecouverteBluetooth->stop();
}

void Bluetooth::connecterPeripherique(QBluetoothSocket* socket)
// faut-il mettre cette déclaration "QBluetoothSocket* socket" dans private
{
    QBluetoothUuid uuid = QBluetoothUuid(QBluetoothUuid::SerialPort);
    socket->connectToService(peripheriqueDistant.address(), uuid);
    socket->open(QIODevice::ReadWrite);
}

void Bluetooth::deconnecterPeripherique(QBluetoothSocket* socket)
{
    if(socket->isOpen())
    {
        socket->close();
    }
}

void Bluetooth::envoyerTrame(QString trame)
{
    if(socket != nullptr && socket->isOpen())
    {
        socket->write(trame.toLocal8Bit());
    }
}
