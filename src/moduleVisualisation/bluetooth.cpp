#include "bluetooth.h"
#include "adressesPeripheries.h"

Bluetooth::Bluetooth()
{
    qDebug() << Q_FUNC_INFO << this;
    initialiserInterfaceLocal();
    trouverPeripherique();
}

Bluetooth::~Bluetooth()
{
    // @todo libérer les sockets
    qDebug() << Q_FUNC_INFO << this;
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
    connect(this,
            SIGNAL(peripheriqueDistantTrouve(QBluetoothDeviceInfo)),
            this,
            SLOT(connecterPeripherique(QBluetoothDeviceInfo)));

    agentDecouverteBluetooth->start();
}

void Bluetooth::gererPeripherique(QBluetoothDeviceInfo peripherique)
{
    qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
             << peripherique.address();

    // Tester si le périphérique découvert est celui avec lequel on souhaite
    // communiquer Alors on peut conserver le périphérique comme ceci
    if(peripherique.address().toString() == ADRESSE_MODULE_TEST)
    {
        qDebug() << Q_FUNC_INFO << "trouvé";
        // peripheriqueDistant = peripherique;
        emit peripheriqueDistantTrouve(peripherique);
    }
    // Et on pourra alors arrêter la découverte avec
    // agentDecouverteBluetooth->stop();
}

void Bluetooth::connecterPeripherique(QBluetoothDeviceInfo peripherique)
{
    qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
             << peripherique.address();

    if(!sockets.contains(peripherique.address().toString()))
    {
        sockets[peripherique.address().toString()] =
          new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

        // @todo connecter signals/slots de cette socket

        QBluetoothUuid uuid = QBluetoothUuid(QBluetoothUuid::SerialPort);
        sockets[peripherique.address().toString()]->connectToService(
          peripherique.address(),
          uuid);
        sockets[peripherique.address().toString()]->open(QIODevice::ReadWrite);
    }
}

void Bluetooth::deconnecterPeripherique(QBluetoothSocket* socket)
{
    if(socket->isOpen())
    {
        socket->close();
    }
}

void Bluetooth::envoyerTrame(QString adresse, QString trame)
{
    if(sockets[adresse] != nullptr && sockets[adresse]->isOpen())
    {
        sockets[adresse]->write(trame.toLocal8Bit());
    }
}
