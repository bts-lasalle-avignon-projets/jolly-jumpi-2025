#include "bluetooth.h"
#include "ihmmoduleconfiguration.h"

#include <QDebug>

Bluetooth::Bluetooth(IHMModuleConfiguration* ihmModuleConfiguration) :
    QObject(ihmModuleConfiguration),
    ihmModuleConfiguration(ihmModuleConfiguration), server(nullptr)
{
    demarrerServeur();
}

Bluetooth::~Bluetooth()
{
    arreterServeur();
}

void Bluetooth::demarrerServeur()
{
    if(server)
    {
        qWarning() << "Bluetooth déjà démarré.";
        return;
    }

    server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);

    connect(server,
            &QBluetoothServer::newConnection,
            this,
            &Bluetooth::clientConnecte);

    if(!server->listen())
    {
        qWarning() << "Impossible de démarrer le Bluetooth.";
        delete server;
        server = nullptr;
    }
    else
    {
        qDebug() << "Bluetooth démarré.";
    }
}

void Bluetooth::clientConnecte()
{
    QBluetoothSocket* nouveauClient = server->nextPendingConnection();
    if(!nouveauClient)
        return;

    qDebug() << "Client Bluetooth connecté.";

    sockets << nouveauClient;

    connect(nouveauClient,
            &QBluetoothSocket::disconnected,
            this,
            &Bluetooth::deconnexion);

    connect(nouveauClient, &QBluetoothSocket::readyRead, this, [=]() {
        while(nouveauClient->canReadLine())
        {
            QString ligne =
              QString::fromUtf8(nouveauClient->readLine()).trimmed();
            qDebug() << "[Bluetooth] Reçu :" << ligne;

            if(ligne == "$Ap")
            {
                emit associationReussie();
            }
            else if(ligne == "$D")
            {
                emit partieDemarree();
            }
            else if(ligne == "$F")
            {
                emit partieTerminee();
            }
        }
    });
};

void Bluetooth::envoyer(const QString& trame)
{
    QByteArray donnees = trame.toUtf8();

    for(QBluetoothSocket* s: sockets)
    {
        if(s->isOpen())
        {
            s->write(donnees);
            qDebug() << "[Bluetooth] Trame envoyée :" << trame;
        }
        else
        {
            qWarning() << "[Bluetooth] Socket non connecté.";
        }
    }
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

void Bluetooth::arreterServeur()
{
    for(QBluetoothSocket* socket: sockets)
    {
        if(socket->isOpen())
            socket->close();
        socket->deleteLater();
    }
    sockets.clear();

    if(server)
    {
        server->close();
        server->deleteLater();
        server = nullptr;
        qDebug() << "Bluetooth arrêté.";
    }
    else
    {
        qWarning() << "aucun Bluetooth à arrêter.";
    }
}
