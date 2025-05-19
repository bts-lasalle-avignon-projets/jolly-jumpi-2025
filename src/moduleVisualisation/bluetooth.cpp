#include "bluetooth.h"
#include "adressesPeripheriques.h"
#include "communication.h"

Bluetooth::Bluetooth() : agentDecouverteBluetooth(nullptr), serveur(nullptr)
{
    qDebug() << Q_FUNC_INFO << this;
    initialiserInterfaceLocal();
    if(ACTIVER_SERVEUR)
        demarrerServeur();
    if(ACTIVER_RECHERCHE)
        trouverPeripherique();
}

Bluetooth::~Bluetooth()
{
    if(ACTIVER_RECHERCHE)
        arreterRecherche();
    libererCommunications();
    arreterInterfaceLocal();
    qDebug() << Q_FUNC_INFO << this;
}

void Bluetooth::initialiserInterfaceLocal()
{
    if(peripheriqueBluetoothLocal.isValid())
    {
        peripheriqueBluetoothLocal.powerOn();

        // L'interface a un nom et une adresse MAC
        qDebug() << Q_FUNC_INFO << peripheriqueBluetoothLocal.name()
                 << peripheriqueBluetoothLocal.address().toString();

        // Nécessaire pour que les clients puissent "découvrir" le serveur
        peripheriqueBluetoothLocal.setHostMode(
          QBluetoothLocalDevice::HostDiscoverable);

        connect(&peripheriqueBluetoothLocal,
                &QBluetoothLocalDevice::hostModeStateChanged,
                [=](QBluetoothLocalDevice::HostMode etat)
                {
                    qDebug() << Q_FUNC_INFO << "hostModeStateChanged"
                             << "etat" << etat;
                });
        connect(&peripheriqueBluetoothLocal,
                &QBluetoothLocalDevice::error,
                [=](QBluetoothLocalDevice::Error erreur)
                {
                    qDebug() << Q_FUNC_INFO << "erreur" << erreur;
                });

        if(GESTION_APPAIRAGE)
            gererAppairage();
    }
}

void Bluetooth::arreterInterfaceLocal()
{
    if(peripheriqueBluetoothLocal.isValid())
    {
        // L'interface a un nom et une adresse MAC
        qDebug() << Q_FUNC_INFO << peripheriqueBluetoothLocal.name()
                 << peripheriqueBluetoothLocal.address().toString();

        peripheriqueBluetoothLocal.setHostMode(
          QBluetoothLocalDevice::HostPoweredOff);
    }
}

void Bluetooth::libererCommunications()
{
    qDebug() << Q_FUNC_INFO;
    serviceBluetooth.unregisterService();
    // libérer les sockets
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

void Bluetooth::gererAppairage()
{
    qDebug() << Q_FUNC_INFO;
    connect(&peripheriqueBluetoothLocal,
            &QBluetoothLocalDevice::pairingFinished,
            [=](const QBluetoothAddress&       adresse,
                QBluetoothLocalDevice::Pairing pairing)
            {
                qDebug() << Q_FUNC_INFO << "pairingFinished"
                         << "adresse" << adresse.toString() << "pairing"
                         << pairing;
            });
    connect(&peripheriqueBluetoothLocal,
            &QBluetoothLocalDevice::pairingDisplayPinCode,
            [=](const QBluetoothAddress& adresse, QString pin)
            {
                qDebug() << Q_FUNC_INFO << "pairingDisplayPinCode"
                         << "adresse" << adresse.toString() << "pin" << pin;
                peripheriqueBluetoothLocal.pairingConfirmation(true);
            });
    connect(&peripheriqueBluetoothLocal,
            &QBluetoothLocalDevice::pairingDisplayConfirmation,
            [=](const QBluetoothAddress& adresse, QString pin)
            {
                qDebug() << Q_FUNC_INFO << "pairingDisplayConfirmation"
                         << "adresse" << adresse.toString() << "pin" << pin;
                peripheriqueBluetoothLocal.pairingConfirmation(true);
            });
}

void Bluetooth::afficherAppairagePeripherique(QBluetoothDeviceInfo peripherique)
{
    if(peripherique.name().startsWith(PREFIX_NOM_MOD_CONFIGURATION) ||
       peripherique.name().startsWith(PREFIX_NOM_PISTE))
    {
        QBluetoothLocalDevice::Pairing etatAppairage =
          peripheriqueBluetoothLocal.pairingStatus(peripherique.address());
        if(etatAppairage == QBluetoothLocalDevice::Paired)
        {
            qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
                     << peripherique.address() << "Paired";
        }
        else if(etatAppairage == QBluetoothLocalDevice::AuthorizedPaired)
        {
            qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
                     << peripherique.address() << "AuthorizedPaired";
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
                     << peripherique.address() << "Unpaired";
        }
    }
}

void Bluetooth::appairerPeripherique(QBluetoothDeviceInfo peripherique)
{
    if(!GESTION_APPAIRAGE)
        return;
    if(peripherique.name().startsWith(PREFIX_NOM_MOD_CONFIGURATION) ||
       peripherique.name().startsWith(PREFIX_NOM_PISTE))
    {
        QBluetoothLocalDevice::Pairing etatAppairage =
          peripheriqueBluetoothLocal.pairingStatus(peripherique.address());
        if(etatAppairage == QBluetoothLocalDevice::Paired)
        {
            qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
                     << peripherique.address()
                     << "requestPairing AuthorizedPaired";
            peripheriqueBluetoothLocal.requestPairing(
              peripherique.address(),
              QBluetoothLocalDevice::AuthorizedPaired);
        }
        else if(etatAppairage == QBluetoothLocalDevice::AuthorizedPaired)
        {
            // C'est ok !
            return;
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
                     << peripherique.address()
                     << "requestPairing AuthorizedPaired";
            peripheriqueBluetoothLocal.requestPairing(
              peripherique.address(),
              QBluetoothLocalDevice::AuthorizedPaired);
        }
    }
}

bool Bluetooth::estMessageValide(const QString& message)
{
    if(message.startsWith(DEBUT_MESSAGE) && message.endsWith(FIN_MESSAGE))
    {
        qDebug() << Q_FUNC_INFO << "true";
        return true;
    }
    qDebug() << Q_FUNC_INFO << "false" << message;
    return false;
}

void Bluetooth::nettoyerMessage(QString& message)
{
    message.remove(DEBUT_MESSAGE);
    message.remove(FIN_MESSAGE);
}

void Bluetooth::demarrerServeur()
{
    if(serveur == nullptr)
    {
        qDebug() << Q_FUNC_INFO;
        serveur =
          new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
        connect(serveur,
                SIGNAL(newConnection()),
                this,
                SLOT(connecterPeripheriqueDecouvert()));

        connect(serveur,
                SIGNAL(error(QBluetoothServer::Error)),
                this,
                SLOT(afficherErreurServeur(QBluetoothServer::Error)));

        serviceBluetooth =
          serveur->listen(QBluetoothUuid(QBluetoothUuid::Rfcomm), serviceNom);
    }
}

int Bluetooth::getNbConnectes()
{
    int nombrePeripheriques = 0;

    QList<QBluetoothAddress> listePeripheriquesConnectes =
      peripheriqueBluetoothLocal.connectedDevices();
    qDebug() << Q_FUNC_INFO << "nb peripheriques connectés"
             << listePeripheriquesConnectes.size();
    qDebug() << Q_FUNC_INFO << "listePeripheriquesConnectes"
             << listePeripheriquesConnectes;

    for(const auto& adresse: sockets.keys())
    {
        const QBluetoothSocket* socket = sockets.value(adresse);
        if(socket != nullptr)
        {
            qDebug() << Q_FUNC_INFO << "isOpen" << socket->isOpen() << "state"
                     << socket->state() << "adresse" << adresse;
            if(socket->state() == QBluetoothSocket::ConnectedState)
                nombrePeripheriques++;
        }
    }
    qDebug() << Q_FUNC_INFO << "nombrePeripheriques" << nombrePeripheriques;

    return nombrePeripheriques;
}

void Bluetooth::trouverPeripherique()
{
    if(agentDecouverteBluetooth == nullptr)
    {
        agentDecouverteBluetooth = new QBluetoothDeviceDiscoveryAgent(this);
        agentDecouverteBluetooth->setInquiryType(
          QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);

        connect(agentDecouverteBluetooth,
                SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
                this,
                SLOT(gererPeripheriqueDecouvert(QBluetoothDeviceInfo)));
        connect(this,
                SIGNAL(peripheriqueDistantTrouve(QBluetoothDeviceInfo)),
                this,
                SLOT(connecterPeripheriqueDecouvert(QBluetoothDeviceInfo)));
        connect(agentDecouverteBluetooth,
                &QBluetoothDeviceDiscoveryAgent::finished,
                this,
                [=]()
                {
                    qDebug() << Q_FUNC_INFO << "recherche terminée !";
                    QList<QBluetoothDeviceInfo> listePeripheriques =
                      agentDecouverteBluetooth->discoveredDevices();
                    for(auto peripherique: listePeripheriques)
                    {
                        if(peripherique.name().startsWith(
                             PREFIX_NOM_MOD_CONFIGURATION) ||
                           peripherique.name().startsWith(PREFIX_NOM_PISTE))
                        {
                            qDebug()
                              << Q_FUNC_INFO << "peripherique decouvert"
                              << peripherique.name() << peripherique.address();
                            afficherAppairagePeripherique(peripherique);
                            appairerPeripherique(peripherique);
                        }
                    }
                    emit rechercheTerminee();
                    if(RECHERCHE_CONTINU)
                        agentDecouverteBluetooth->start(
                          QBluetoothDeviceDiscoveryAgent::ClassicMethod);
                });
    }

    // Recherche en cours ?
    if(agentDecouverteBluetooth->isActive())
    {
        return;
    }

    qDebug() << Q_FUNC_INFO << "démarrage recherche";
    agentDecouverteBluetooth->start(
      QBluetoothDeviceDiscoveryAgent::ClassicMethod);
}

void Bluetooth::arreterRecherche()
{
    qDebug() << Q_FUNC_INFO;
    if(agentDecouverteBluetooth != nullptr &&
       agentDecouverteBluetooth->isActive())
        agentDecouverteBluetooth->stop();
}

void Bluetooth::envoyerMessage(QString adresse, QString message)
{
    qDebug() << Q_FUNC_INFO << "adresse" << adresse << "message" << message;
    if(sockets[adresse] != nullptr && sockets[adresse]->isOpen() &&
       sockets[adresse]->state() == QBluetoothSocket::ConnectedState)
    {
        sockets[adresse]->write(message.toLocal8Bit());
    }
    else if(sockets[adresse] != nullptr)
    {
        qDebug() << Q_FUNC_INFO << "isOpen" << sockets[adresse]->isOpen()
                 << "state" << sockets[adresse]->state() << "adresse"
                 << adresse;
    }
}

void Bluetooth::envoyerMessageGroupe(QString message)
{
    for(const auto& adresse: sockets.keys())
    {
        qDebug() << Q_FUNC_INFO << "adresse" << adresse << "message" << message;
        envoyerMessage(adresse, message);
    }
}

QString Bluetooth::recupererAdressePeripherique(QBluetoothSocket* socket)
{
    return sockets.key(socket);
}

QBluetoothSocket* Bluetooth::recupererSocketPeripherique(QString adresse)
{
    return sockets.value(adresse);
}

QString Bluetooth::recupererNomPeripherique(QBluetoothSocket* socket)
{
    return peripheriques.value(sockets.key(socket));
}

QString Bluetooth::recupererNomPeripherique(QString adresse)
{
    return peripheriques.value(adresse);
}

QString Bluetooth::recupererAdresseModuleConfiguration()
{
    return adresseModuleConfiguration;
}

bool Bluetooth::estPeripheriqueConnecte(QString adresse)
{
    QBluetoothSocket* socket = recupererSocketPeripherique(adresse);
    if(socket != nullptr && socket->isOpen() &&
       socket->state() == QBluetoothSocket::ConnectedState)
    {
        return true;
    }
    return false;
}

// Les slots
void Bluetooth::gererPeripheriqueDecouvert(QBluetoothDeviceInfo peripherique)
{
    // Tester si le périphérique découvert est celui avec lequel on souhaite
    // communiquer
    if(peripherique.name().startsWith(PREFIX_NOM_MOD_CONFIGURATION) ||
       peripherique.name().startsWith(PREFIX_NOM_PISTE))
    {
        qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
                 << peripherique.address();
        emit peripheriqueDistantTrouve(peripherique);
    }
}

// Mode client
void Bluetooth::connecterPeripheriqueDecouvert(
  QBluetoothDeviceInfo peripherique)
{
    QString           adresse        = peripherique.address().toString();
    QBluetoothSocket* socket         = nullptr;
    QTimer*           delaiConnexion = nullptr;
    bool              trouve         = false;

#if MODULE_PISTE == CLIENT
    if(peripherique.name().startsWith(PREFIX_NOM_PISTE))
        trouve = true;
#endif

#if MODULE_CONFIGURATION == CLIENT
    if(peripherique.name().startsWith(PREFIX_NOM_MOD_CONFIGURATION))
        trouve = true;
#endif

    if(!trouve)
        return;

    qDebug() << Q_FUNC_INFO << "trouve" << trouve << "nom"
             << peripherique.name() << "adresse"
             << peripherique.address().toString();
    QString numeroPiste = peripherique.name();
    if(peripherique.name().startsWith(PREFIX_NOM_PISTE))
    {
        numeroPiste.replace(QString(PREFIX_NOM_PISTE) + QString("-"),
                            QString(""));
        qDebug() << Q_FUNC_INFO << "numeroPiste" << numeroPiste.toInt();
    }

    // On a déjà eu ce client ?
    if(sockets.contains(adresse))
    {
        // on nettoie
        delete sockets[adresse];
        sockets.remove(adresse);
        peripheriques.remove(adresse);
    }

    // on instancie une socket
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    // on conserve la socket vers le serveur
    sockets[adresse] = socket;

    // et on gère les signaux
    // // Les données entrantes
    connect(socket,
            &QBluetoothSocket::readyRead,
            this,
            [=]()
            {
                QString message = socket->readAll();
                qDebug() << Q_FUNC_INFO << "réception"
                         << "nom" << peripherique.name() << "message"
                         << message;
                traiterMessage(peripherique, message);
            });

    // Les erreurs
    connect(socket,
            SIGNAL(error(QBluetoothSocket::SocketError)),
            this,
            SLOT(afficherErreurSocket(QBluetoothSocket::SocketError)));

    // Les changements d'état
    connect(socket,
            SIGNAL(stateChanged(QBluetoothSocket::SocketState)),
            this,
            SLOT(afficherEtat(QBluetoothSocket::SocketState)));

    // Les déconnexions
    connect(socket,
            &QBluetoothSocket::disconnected,
            this,
            [=]()
            {
                qDebug() << Q_FUNC_INFO << "déconnecté"
                         << "nom" << recupererNomPeripherique(socket)
                         << "adresse" << recupererAdressePeripherique(socket);
                emit peripheriqueDistantDeconnecte(
                  recupererNomPeripherique(socket),
                  recupererAdressePeripherique(socket));
            });

    // Les connexions
    connect(socket,
            &QBluetoothSocket::connected,
            this,
            [=]()
            {
                qDebug() << Q_FUNC_INFO << "connecté"
                         << "nom" << peripherique.name() << "adresse"
                         << peripherique.address().toString();
                if(delaiConnexion != nullptr)
                {
                    delaiConnexion->stop();
                    delaiConnexion->deleteLater();
                }
                peripheriques[adresse] = peripherique.name();
                if(peripherique.name().startsWith(PREFIX_NOM_MOD_CONFIGURATION))
                {
                    adresseModuleConfiguration =
                      recupererAdressePeripherique(socket);
                    qDebug() << Q_FUNC_INFO << "adresseModuleConfiguration"
                             << adresseModuleConfiguration;
                }
                emit peripheriqueDistantConnecte(peripherique);
#ifdef TEST_ASSOCIATION
                envoyerMessage(socket->peerAddress().toString(), "$A\n");
#endif
            });

    // Pas connecté ?
    if(socket->state() == QBluetoothSocket::UnconnectedState)
    {
        // Création du delai de connexion (timeout)
        delaiConnexion = new QTimer(this);
        delaiConnexion->setSingleShot(true);

        connect(delaiConnexion,
                &QTimer::timeout,
                this,
                [=]()
                {
                    if(socket->state() != QBluetoothSocket::ConnectedState)
                    {
                        qDebug() << Q_FUNC_INFO << "délai de connexion dépassé"
                                 << "nom" << peripherique.name() << "adresse"
                                 << peripherique.address().toString();
                        socket->abort();
                    }
                    delaiConnexion->deleteLater();
                });

        qDebug() << Q_FUNC_INFO << "tentative de connexion"
                 << "nom" << peripherique.name() << "adresse"
                 << peripherique.address().toString();
        QBluetoothUuid uuid = QBluetoothUuid(QBluetoothUuid::SerialPort);
        qDebug() << Q_FUNC_INFO << "uuid" << uuid.toString();
        /**
         * @fixme Connexion vers plusieurs clients : qt.bluetooth.bluez: Client
         * profile registration failed: "UUID already registered"
         * Solution temporaire : export BLUETOOTH_FORCE_DBUS_LE_VERSION=5.45
         */
        socket->connectToService(peripherique.address(), uuid);
        delaiConnexion->start(DELAI_CONNEXION);
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "déjà connecté"
                 << "nom" << peripherique.name() << "adresse"
                 << peripherique.address().toString();
    }
}

// Mode serveur
void Bluetooth::connecterPeripheriqueDecouvert()
{
    // la socket du client
    QBluetoothSocket* socket = serveur->nextPendingConnection();
    if(!socket)
        return;

    qDebug() << Q_FUNC_INFO << socket << "nom" << socket->peerName()
             << "adresse" << socket->peerAddress().toString();

#if MODULE_PISTE == SERVEUR
    if(!socket->peerName().startsWith(PREFIX_NOM_PISTE))
        return;
#endif

#if MODULE_CONFIGURATION == SERVEUR
    if(!socket->peerName().startsWith(PREFIX_NOM_MOD_CONFIGURATION))
        return;
#endif

    // On a déjà eu ce client ?
    if(sockets.contains(socket->peerAddress().toString()))
    {
        // on nettoie
        delete sockets[socket->peerAddress().toString()];
        sockets.remove(socket->peerAddress().toString());
        peripheriques.remove(socket->peerAddress().toString());
    }

    // Nouveau périphérique
    qDebug() << Q_FUNC_INFO << "nouveau périphérique"
             << "nom" << socket->peerName() << "adresse"
             << socket->peerAddress().toString();
    // on récupère sa socket
    sockets[socket->peerAddress().toString()] = socket;

    // on enregistre l'adresse du module de configuration
    if(socket->peerName().startsWith(PREFIX_NOM_MOD_CONFIGURATION))
    {
        adresseModuleConfiguration = recupererAdressePeripherique(socket);
        qDebug() << Q_FUNC_INFO << "adresseModuleConfiguration"
                 << adresseModuleConfiguration;
    }

    // et on gère les signaux
    // Les données entrantes
    connect(socket,
            &QBluetoothSocket::readyRead,
            this,
            [=]()
            {
                QString message = socket->readAll();
                qDebug() << Q_FUNC_INFO << "réception"
                         << "nom" << socket->peerName() << "adresse"
                         << socket->peerAddress() << "message" << message;
                traiterMessage(socket->peerName(),
                               socket->peerAddress().toString(),
                               message);
            });

    // Les erreurs
    connect(socket,
            SIGNAL(error(QBluetoothSocket::SocketError)),
            this,
            SLOT(afficherErreurSocket(QBluetoothSocket::SocketError)));

    // Les changements d'état
    connect(socket,
            SIGNAL(stateChanged(QBluetoothSocket::SocketState)),
            this,
            SLOT(afficherEtat(QBluetoothSocket::SocketState)));

    // Les déconnexions
    connect(socket,
            &QBluetoothSocket::disconnected,
            this,
            [=]()
            {
                qDebug() << Q_FUNC_INFO << "déconnecté"
                         << "nom" << recupererNomPeripherique(socket)
                         << "adresse" << recupererAdressePeripherique(socket);
                emit peripheriqueDistantDeconnecte(
                  recupererNomPeripherique(socket),
                  recupererAdressePeripherique(socket));
            });

    // on conserve son nom
    peripheriques[socket->peerAddress().toString()] = socket->peerName();

    // on signale la connexion
    emit peripheriqueDistantConnecte(socket->peerName(),
                                     socket->peerAddress().toString());

#ifdef TEST_ASSOCIATION
    envoyerMessage(socket->peerAddress().toString(), "$A\n");
#endif
}

void Bluetooth::deconnecterPeripherique(QBluetoothDeviceInfo peripherique)
{
    QBluetoothSocket* socket = sockets[peripherique.address().toString()];
    if(socket->isOpen())
    {
        qDebug() << Q_FUNC_INFO << "nom" << peripherique.name() << "adresse"
                 << peripherique.address();
        socket->disconnectFromService();
    }
}

void Bluetooth::traiterMessage(QBluetoothDeviceInfo peripherique,
                               QString              message)
{
    traiterMessage(peripherique.name(),
                   peripherique.address().toString(),
                   message);
}

void Bluetooth::traiterMessage(QString nom, QString adresse, QString message)
{
    qDebug() << Q_FUNC_INFO << "nom" << nom << "adresse" << adresse << "message"
             << message;
    if(estMessageValide(message))
    {
        nettoyerMessage(message);
        emit messageRecu(nom, adresse, message);
    }
}

void Bluetooth::afficherErreurServeur(QBluetoothServer::Error erreur)
{
    qDebug() << Q_FUNC_INFO << "erreur" << erreur;
}

void Bluetooth::afficherErreurSocket(QBluetoothSocket::SocketError erreur)
{
    QBluetoothSocket* socket = qobject_cast<QBluetoothSocket*>(sender());
    QString           messageErreur;

    switch(erreur)
    {
        case QBluetoothSocket::NoSocketError:
            messageErreur = "Pas d'erreur";
            break;
        case QBluetoothSocket::UnknownSocketError:
            messageErreur = "Erreur inconnue (timeout ?)";
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
            messageErreur = QString("Erreur inconnue (code %1)")
                              .arg(static_cast<int>(erreur));
            break;
    }

    /*qDebug() << Q_FUNC_INFO << socket << "nom" << socket->peerName()
             << "adresse" << socket->peerAddress() << messageErreur;*/
    qDebug() << Q_FUNC_INFO << socket << "nom"
             << recupererNomPeripherique(socket) << "adresse"
             << recupererAdressePeripherique(socket) << messageErreur;
}

void Bluetooth::afficherEtat(QBluetoothSocket::SocketState etat)
{
    QBluetoothSocket* socket = qobject_cast<QBluetoothSocket*>(sender());
    QString           messageEtat;

    switch(etat)
    {
        case QBluetoothSocket::UnconnectedState:
            messageEtat = "Socket déconnecté";
            break;
        case QBluetoothSocket::ServiceLookupState:
            messageEtat = "Socket demande les paramètres de connexion";
            break;
        case QBluetoothSocket::ConnectingState:
            messageEtat = "Socket en cours de connexion";
            break;
        case QBluetoothSocket::ConnectedState:
            messageEtat = "Socket connecté";
            break;
        case QBluetoothSocket::BoundState:
            messageEtat = "Socket attaché localement";
            break;
        case QBluetoothSocket::ClosingState:
            messageEtat = "Socket en attente de déconnexion";
            break;
        case QBluetoothSocket::ListeningState:
            messageEtat = "Socket en écoute de demande de connexion";
            break;
        default:
            messageEtat = "État socket inconnu !";
            break;
    }

    /*qDebug() << Q_FUNC_INFO << socket << "nom" << socket->peerName()
             << "adresse" << socket->peerAddress() << messageEtat;*/
    qDebug() << Q_FUNC_INFO << socket << "nom"
             << recupererNomPeripherique(socket) << "adresse"
             << recupererAdressePeripherique(socket) << messageEtat;
}
