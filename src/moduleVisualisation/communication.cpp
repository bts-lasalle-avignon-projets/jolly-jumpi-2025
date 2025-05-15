#include "communication.h"
#include "bluetooth.h"
#include "adressesPeripheriques.h"

Communication::Communication(QObject* parent) :
    QObject(parent), bluetooth(new Bluetooth)
{
    qDebug() << Q_FUNC_INFO << this;

    typesMessages << "C"  // CONFIGURATION
                  << "D"  // DEBUT_PARTIE
                  << "F"  // FIN_PARTIE
                  << "S0" // PAGE_ACCUEIL
                  << "S1" // PAGE_HISTORIQUE
                  << "A"  // ASSOCIATION
                  << "T"; // TIR

    connect(bluetooth,
            &Bluetooth::messageRecu,
            this,
            &Communication::traiterMessage);
}

Communication::~Communication()
{
    delete bluetooth;
    qDebug() << Q_FUNC_INFO << this;
}

void Communication::traiterMessage(QString nom,
                                   QString adresse,
                                   QString message)
{
    Communication::TypeMessage typeMessage =
      Communication::TypeMessage::INCONNU;

    typeMessage = identifierTypeMessage(message);
    qDebug() << Q_FUNC_INFO << "nom" << nom << "adresse" << adresse << "message"
             << message << "typeMessage" << typeMessage;
    switch(typeMessage)
    {
        case Communication::TypeMessage::CONFIGURATION:
            gererConfiguration();
            break;
        case Communication::TypeMessage::ASSOCIATION:
            gererAssociation(adresse);
            break;
        case Communication::TypeMessage::PAGE_ACCUEIL:
        case Communication::TypeMessage::PAGE_HISTORIQUE:
            gererChangementPage();
            break;
        default:
            qDebug() << Q_FUNC_INFO << "typeMessage inconnu !";
    }
}

Communication::TypeMessage Communication::identifierTypeMessage(
  const QString& message)
{
    for(int i = 0; i < typesMessages.size(); ++i)
    {
        if(verifierTypeMessage(message, typesMessages.at(i)))
            return Communication::TypeMessage(i);
    }
    return Communication::TypeMessage::INCONNU;
}

bool Communication::verifierTypeMessage(const QString& message,
                                        const QString& type)
{
    if(message.startsWith(type))
    {
        return true;
    }
    return false;
}

QString Communication::construireMessage(const QString& message)
{
    return DEBUT_MESSAGE + message + FIN_MESSAGE;
}

void Communication::envoyerMessage(const QString& destinataire,
                                   const QString& message)
{
    qDebug() << Q_FUNC_INFO << "destinataire" << destinataire << "message"
             << message;
    bluetooth->envoyerMessage(destinataire, construireMessage(message));
}

void Communication::demanderAssociation(const QString& destinataire)
{
    qDebug() << Q_FUNC_INFO << "destinataire" << destinataire;
    envoyerMessage(destinataire,
                   typesMessages.at(Communication::TypeMessage::ASSOCIATION));
}

void Communication::confirmerAssociation(const QString& destinataire)
{
    qDebug() << Q_FUNC_INFO << "destinataire" << destinataire;
    envoyerMessage(destinataire,
                   typesMessages.at(Communication::TypeMessage::ASSOCIATION));
}

void Communication::envoyerModeDeJeu(const QString& destinataire,
                                     const QString& information)
{
    qDebug() << Q_FUNC_INFO << "destinataire" << destinataire << "information"
             << information;
    envoyerMessage(destinataire, information);
}

void Communication::envoyerDebutDePartie(const QString& destinataire)
{
    qDebug() << Q_FUNC_INFO << "destinataire" << destinataire;
    envoyerMessage(destinataire,
                   typesMessages.at(Communication::TypeMessage::DEBUT_PARTIE));
}

void Communication::signalerFinDePartie(const QString& destinataire)
{
    qDebug() << Q_FUNC_INFO << "destinataire" << destinataire;
    envoyerMessage(destinataire,
                   typesMessages.at(Communication::TypeMessage::FIN_PARTIE));
}

void Communication::gererConfiguration()
{
    qDebug() << Q_FUNC_INFO;
}

void Communication::gererAssociation(const QString& adresse)
{
    qDebug() << Q_FUNC_INFO;
#ifdef TEST_ASSOCIATION
    envoyerMessage(adresse, "C0");
    QTimer::singleShot(2000,
                       this,
                       [this]()
                       {
                           bluetooth->envoyerMessageGroupe("$D\n");
                           QTimer::singleShot(
                             10000,
                             this,
                             [this]()
                             {
                                 bluetooth->envoyerMessageGroupe("$F\n");
                             });
                       });
#endif
}

void Communication::gererChangementPage()
{
    qDebug() << Q_FUNC_INFO;
}
