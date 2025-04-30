#include "bluetooth.h"
#include "adressesPeripheries.h"

Bluetooth::Bluetooth()
{
    qDebug() << Q_FUNC_INFO << this;

    initialiserInterfaceLocal();
    trouverPeripherique();
    estToutConnecte();
}

Bluetooth::~Bluetooth()
{
    // @todo libérer les sockets
    for(auto socket: sockets)
    {
        if(socket)
        {
            socket->disconnectFromService();
            socket->deleteLater(); // ou delete socket;
        }
    }
    sockets.clear();
    qDebug() << Q_FUNC_INFO << this;
}

/*void Bluetooth::initialiserAdressePeripherique()
{
    listeAdressePistes{ ADR_MAC_PISTE_1,          ADR_MAC_PISTE_2,
                        ADR_MAC_PISTE_3,          ADR_MAC_PISTE_4,
                        ADR_MAC_PISTE_5,          ADR_MAC_PISTE_6,
                        ADR_MAC_PISTE_7,          ADR_MAC_PISTE_8,
                        ADR_MAC_MOD_CONFIGURATION };
    for(int i = 0; i < NOMBRE_TOTAL_PERIPHERIQUE; i++)
    {
        sockets.insert(listeAdressePistes[i], nullptr);
    }
}*/

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
    agentDecouverteBluetooth = nullptr;

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
    if(peripherique.name().startsWith(PREFIX_NOM_MOD_CONFIGURATION) ||
       peripherique.name().startsWith(PREFIX_NOM_PISTE))
    {
        qDebug() << Q_FUNC_INFO << "trouvé";
        // peripheriqueDistant = peripherique;
        emit peripheriqueDistantTrouve(peripherique);
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Aucune correspondance";
    }
}

void Bluetooth::connecterPeripherique(QBluetoothDeviceInfo peripherique)
{
    qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
             << peripherique.address();

    if(!sockets.contains(peripherique.address().toString()))
    {
        sockets[peripherique.address().toString()] =
          new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

        QBluetoothSocket* socket = sockets[peripherique.address().toString()];

        // Signale quand connecté
        connect(socket, &QBluetoothSocket::connected, this, [=]() {
            qDebug() << "Connecté à:" << socket->peerName()
                     << socket->peerAddress().toString();
        });

        // Signale quand erreur
        connect(socket,
                SIGNAL(error(QBluetoothSocket::SocketError)),
                this,
                SLOT(gererErreurSocket(QBluetoothSocket::SocketError)));

        // @todo connecter signals/slots de cette socket
        connect(this,
                SIGNAL(peripheriqueDistantDeconnecte(QBluetoothDeviceInfo)),
                this,
                SLOT(deconnecterPeripherique(QBluetoothDeviceInfo)));
        connect(this,
                SIGNAL(donneesReceptionnees(QBluetoothDeviceInfo)),
                this,
                SLOT(traiterMessage(QBluetoothDeviceInfo)));

        QBluetoothUuid uuid = QBluetoothUuid(QBluetoothUuid::SerialPort);
        sockets[peripherique.address().toString()]->connectToService(
          peripherique.address(),
          uuid);
        sockets[peripherique.address().toString()]->open(QIODevice::ReadWrite);
    }
}

void Bluetooth::gererErreurSocket(QBluetoothSocket::SocketError erreur)
{
    qDebug() << Q_FUNC_INFO << "Erreur Bluetooth : " << erreur;
}

void Bluetooth::deconnecterPeripherique(QBluetoothDeviceInfo peripherique)
{
    QBluetoothSocket* socket = sockets[peripherique.address().toString()];
    if(socket->isOpen())
    {
        socket->close();
    }
}

void Bluetooth::envoyerMessage(QString adresse, QString trame)
{
    if(sockets[adresse] != nullptr && sockets[adresse]->isOpen())
    {
        sockets[adresse]->write(trame.toLocal8Bit());
    }
}

bool Bluetooth::estToutConnecte()
{
    int nombrePeripherique = 0;
    for(const auto& key: sockets.keys())
    {
        const QBluetoothSocket* socket = sockets.value(key);
        if(!socket)
        {
            qDebug() << "Socket nulle pour la clé:" << key;
            return false;
        }
        else
        {
            qDebug() << "Socket valide pour la clé:" << key << ":" << socket;
        }

        nombrePeripherique++;
    }
    if(nombrePeripherique != NOMBRE_TOTAL_PERIPHERIQUE)
    {
        qDebug() << "Nombre de périphérique manquant :"
                 << NOMBRE_TOTAL_PERIPHERIQUE - nombrePeripherique;
        return false;
    }
    else
        qDebug() << "Tout les périphériques sont connectés";
    return true;
}

void Bluetooth::arreterRecherche()
{
    // Et on pourra alors arrêter la découverte avec
    agentDecouverteBluetooth->stop();
}

void Bluetooth::receptionnerMessage(QBluetoothDeviceInfo peripherique)
{
    if(sockets[peripherique.address().toString()])
    {
        QString readSocket =
          sockets[peripherique.address().toString()]->readAll();
        qDebug() << Q_FUNC_INFO
                 << "Données reçues:" + readSocket + peripherique.name();
        traiterMessage(peripherique);
    }
}

void Bluetooth::traiterMessage(QBluetoothDeviceInfo peripherique)
{
    qDebug() << Q_FUNC_INFO << "Message traité avec succès";
}
