#include "bluetooth.h"
#include "adressesPeripheries.h"

Bluetooth::Bluetooth()
{
    qDebug() << Q_FUNC_INFO << this;
    initialiserInterfaceLocal();
    trouverPeripherique();
    initialiserAdressePeripherique();
    if(estToutConnecte()){
    QTimer::singleShot(10000, this, [this]() {  // Attendre un peu pour laisser le temps à la découverte + connexions et simuler les envois
            arreterRecherche();
            envoyerMessageGroupe("$A\n");
            envoyerMessageGroupe("$F\n"); // Si déconnexion le serveur laisse la partie active
            QTimer::singleShot(5000, this, [this]() {
                    envoyerMessageGroupe("$D\n");
                    QTimer::singleShot(10000, this, [this]() {
                            envoyerMessageGroupe("$F\n");
                    });
            });
    });
    }
}

Bluetooth::~Bluetooth()
{
    qDebug() << Q_FUNC_INFO << this;

    // @todo libérer les sockets
    for(auto socket: sockets)
    {
        if(socket)
        {
            socket->disconnectFromService();
            socket->deleteLater();
        }
    }
    sockets.clear();
}

void Bluetooth::initialiserAdressePeripherique()
{
    /*listeAdressePistes{ ADR_MAC_PISTE_1,          ADR_MAC_PISTE_2,
                        ADR_MAC_PISTE_3,          ADR_MAC_PISTE_4,
                        ADR_MAC_PISTE_5,          ADR_MAC_PISTE_6,
                        ADR_MAC_PISTE_7,          ADR_MAC_PISTE_8,
                        ADR_MAC_MOD_CONFIGURATION };*/
    listeAdressePistes = sockets.keys();
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

        // Lecture des données entrantes
        connect(socket, &QBluetoothSocket::readyRead, this, [=]() {
                    QString message = socket->readAll();
                    qDebug() << "Trame reçue de" << peripherique.name() << message;
                    traiterMessage(peripherique, message);
                });

        // Signale quand connecté
        connect(socket, &QBluetoothSocket::connected, this, [=]() {
                    qDebug() << "Connecté à:" << peripherique.name()
                     << peripherique.address().toString();
        });

        // Signale quand erreur
        connect(socket,
                SIGNAL(error(QBluetoothSocket::SocketError)),
                this,
                SLOT(afficherErreurSocket(QBluetoothSocket::SocketError)));

        // Signale quand déconnecté
        connect(socket, &QBluetoothSocket::disconnected, this, [=]() {
                    qDebug() << "Déconnecté de:" << peripherique.name();
                });

        QBluetoothUuid uuid = QBluetoothUuid(QBluetoothUuid::SerialPort);

        // Gestion propre de la connexion avec une vérification d'erreur
        if (socket->state() == QBluetoothSocket::UnconnectedState)
        {
            socket->connectToService(peripherique.address(), uuid);
            socket->open(QIODevice::ReadWrite);
        }
        else
        {
            qDebug() << "Le socket est déjà connecté, tentative de déconnexion.";
            socket->disconnectFromService();
        }
    }
}

void Bluetooth::afficherErreurSocket(QBluetoothSocket::SocketError erreur)
{
    QString messageErreur;
    switch (erreur) {
        case QBluetoothSocket::NoSocketError:
            messageErreur = "Pas d'erreur";
            break;
        case QBluetoothSocket::UnknownSocketError:
            messageErreur = "Erreur inconnue";
            break;
        case QBluetoothSocket::HostNotFoundError:
            messageErreur = "Hôte non trouvé";
            break;
        case QBluetoothSocket::ServiceNotFoundError:
            messageErreur = "Service non trouvé";
            break;
        case QBluetoothSocket::NetworkError:
            messageErreur = "Erreur réseau";
            break;
        case QBluetoothSocket::UnsupportedProtocolError:
            messageErreur = "Protocole non supporté";
            break;
        case QBluetoothSocket::OperationError:
            messageErreur = "Erreur d'opération Bluetooth";
            break;
        default:
            messageErreur = QString("Erreur inconnue (code %1)").arg(static_cast<int>(erreur));
            break;
    }

    qDebug() << Q_FUNC_INFO << "Erreur Bluetooth : " << messageErreur;
}

void Bluetooth::deconnecterPeripherique(QBluetoothDeviceInfo peripherique)
{
    QBluetoothSocket* socket = sockets[peripherique.address().toString()];
    if(socket->isOpen())
    {
        socket->close();
        qDebug() << Q_FUNC_INFO << "Socket fermée";
    }
    else
        qDebug() << Q_FUNC_INFO << "Socket déjà fermée";
}

void Bluetooth::envoyerMessage(QString adresse, QString trame)
{
    if(sockets[adresse] != nullptr && sockets[adresse]->isOpen() && sockets[adresse]->state() == QBluetoothSocket::ConnectedState)
    {
        sockets[adresse]->write(trame.toLocal8Bit());
        qDebug() << "Trame envoyée : " << trame << "Destinataire : " << adresse;
    } else
        qDebug() << "Echec de l'envoi; Destinataire :" << adresse;
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
    qDebug() << Q_FUNC_INFO << "Arret de la recherche";
    agentDecouverteBluetooth->stop();
}

void Bluetooth::traiterMessage(QBluetoothDeviceInfo peripherique, QString message)
{
    qDebug() << Q_FUNC_INFO << "Expéditeur :" << peripherique.name() << "Message :" << message;
}

void Bluetooth::envoyerMessageGroupe(QString trame){
    for(const auto& adresse: sockets.keys())
    {
        envoyerMessage(adresse, trame);
    }
}
