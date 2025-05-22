#include "bluetooth.h"
#include "ihmmoduleconfiguration.h"
#include "ui_ihmmoduleconfiguration.h"

Bluetooth::Bluetooth(IHMModuleConfiguration* ihmModuleConfiguration) :
    ihmModuleConfiguration(ihmModuleConfiguration), server(nullptr)
{
}

void Bluetooth::demarrerServeur()
{
    if(server)
    {
        qWarning() << "Serveur déjà démarré.";
        return;
    }

    server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);

    if(!server->listen())
    {
        qWarning() << "Impossible de démarrer le serveur Bluetooth.";
        delete server;
        server = nullptr;
    }
    else
    {
        ihmModuleConfiguration->afficherConnexionFait();

        qDebug() << "Serveur Bluetooth démarré.";
    }
    connect(server,
            &QBluetoothServer::newConnection,
            this,
            &Bluetooth::nouvelleConnexion);
}

void Bluetooth::nouvelleConnexion()
{
    QBluetoothSocket* socket = server->nextPendingConnection();
    if(!socket)
        return;

    sockets.append(socket);

    connect(socket,
            &QBluetoothSocket::readyRead,
            this,
            &Bluetooth::traiterDonnees);
    connect(socket,
            &QBluetoothSocket::disconnected,
            this,
            &Bluetooth::deconnexion);

    qDebug() << "Nouvelle connexion Bluetooth de"
             << socket->peerAddress().toString();

    socket->write("Connectée à la table virtuelle\r\n");
}

void Bluetooth::traiterDonnees()
{
    QBluetoothSocket* socket = qobject_cast<QBluetoothSocket*>(sender());
    if(!socket)
        return;

    QByteArray data = socket->readAll();
    qDebug() << "Données reçues:" << data;
}

void Bluetooth::deconnexion()
{
    QBluetoothSocket* socket = qobject_cast<QBluetoothSocket*>(sender());
    if(!socket)
        return;

    sockets.removeAll(socket);
    socket->deleteLater();
    qDebug() << "Client Bluetooth déconnecté.";
}
