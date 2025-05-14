#include "communication.h"
#include "bluetooth.h"
#include "adressesPeripheries.h"

Communication::Communication(QObject* parent) :
    QObject(parent), bluetooth(new Bluetooth)
{
    qDebug() << Q_FUNC_INFO << this;
    connect(bluetooth,
            &Bluetooth::messageRecue,
            this,
            &Communication::traiterMessage);

#ifdef TEST_COMMUNICATION
    QTimer::singleShot(20000, this, [this]() {
        envoyerMessage(ADR_MAC_PISTE_2, "A");
    });
#endif
}

Communication::~Communication()
{
    delete bluetooth;
    qDebug() << Q_FUNC_INFO << this;
}

void Communication::traiterMessage(QString message)
{
    if(estMessageConfiguration(message))
    {
        gererConfiguration();
    }
    else if(estMessageAssociation(message))
    {
        gererAssociation();
    }
    else if(estDemandeChangementPage(message))
    {
        gererChangementPage();
    }
    else
    {
        gererChangementPage();
    }
}

bool Communication::verifierTypeMessage(const QString& message,
                                        const QString& caractere)
{
    if(message.startsWith(caractere))
    {
        qDebug() << Q_FUNC_INFO << "Le message" << message << "contient"
                 << caractere;
        return true;
    }
    qDebug() << Q_FUNC_INFO << "Le message" << message << "ne contient pas"
             << caractere;
    return false;
}

bool Communication::estMessageConfiguration(const QString& message)
{
    if(verifierTypeMessage(message, CAR_CONFIGURATION))
        return true;
    return false;
}

bool Communication::estFinDePartie(const QString& message)
{
    if(verifierTypeMessage(message, CAR_FIN_PARTIE))
        return true;
    return false;
}

bool Communication::estMessageAssociation(const QString& message)
{
    if(verifierTypeMessage(message, CAR_ASSOCIATION))
        return true;
    return false;
}

bool Communication::estDemandePageAccueil(const QString& message)
{
    if(verifierTypeMessage(message, CAR_PAGE_ACCUEIL))
        return true;
    return false;
}

bool Communication::estDemandePageHistorique(const QString& message)
{
    if(verifierTypeMessage(message, CAR_PAGE_HISTORIQUE))
        return true;
    return false;
}

bool Communication::estDemandeChangementPage(const QString& message)
{
    if(estDemandePageAccueil(message) || estDemandePageHistorique(message))
        return true;
    return false;
}

/***********Envoyer trames***********/
QString Communication::construireMessage(const QString& message)
{
    return CAR_DEBUT_TRAME + message + CAR_FIN_TRAME;
}

void Communication::envoyerMessage(const QString& destinataire,
                                   const QString& message)
{
    qDebug() << Q_FUNC_INFO << "Envoi" << destinataire << message;
    bluetooth->envoyerMessage(destinataire, construireMessage(message));
}

void Communication::demanderAssociation(const QString& destinataire)
{
    envoyerMessage(destinataire, CAR_ASSOCIATION);
}

void Communication::confirmerAssociation(const QString& destinataire)
{
    envoyerMessage(destinataire, CAR_ASSOCIATION);
}

void Communication::envoyerModeDeJeu(const QString& destinataire,
                                     const QString& information)
{
    envoyerMessage(destinataire, information);
}

void Communication::envoyerDebutDePartie(const QString& destinataire)
{
    envoyerMessage(destinataire, CAR_DEBUT_PARTIE);
}

void Communication::signalerFinDePartie(const QString& destinataire)
{
    envoyerMessage(destinataire, CAR_FIN_PARTIE);
}

/***********Gestion***********/
void Communication::gererConfiguration()
{
}

void Communication::gererAssociation()
{
}

void Communication::gererChangementPage()
{
}
