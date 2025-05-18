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
#ifdef SIMULATION_MODULE_CONFIG
    QTimer::singleShot(10000,
                       this,
                       [this]()
                       {
                           traiterMessage("jp-config-X", "simulation", "C1;0");
                           QTimer::singleShot(5000,
                                              this,
                                              [this]()
                                              {
                                                  traiterMessage("jp-config-X",
                                                                 "simulation",
                                                                 "D");
                                              });
                       });
#endif
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
            communiquerConfiguration(message);
            demanderConfirmationAssociation();
            break;
        case Communication::TypeMessage::DEBUT_PARTIE:
            emit partieDemarree();
            break;
        case Communication::TypeMessage::FIN_PARTIE:
            emit abandonPartie();
            break;
        case Communication::TypeMessage::PAGE_ACCUEIL:
        case Communication::TypeMessage::PAGE_HISTORIQUE:
            gererChangementPage();
            break;
        case Communication::TypeMessage::ASSOCIATION:
            gererAssociation(message);
            break;
        // Attendre la mise à jour du simulateur car manque le "T"
        /*case Communication::TypeMessage::TIR:
            communiquerTirJoueur(message);
            break;*/
        default:
            // qDebug() << Q_FUNC_INFO << "typeMessage inconnu !";
            communiquerTirJoueur(message);
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

void Communication::envoyerMessageGroupe(const QString& message)
{
    qDebug() << Q_FUNC_INFO << "Message groupé, message" << message;
    bluetooth->envoyerMessageGroupe(construireMessage(message));
}

void Communication::demanderConfirmationAssociation()
{
    qDebug() << Q_FUNC_INFO << "Message groupé, confirmation d'association";
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::ASSOCIATION));
}

void Communication::confirmerAssociation(const QString& retourAssociation)
// Signale la piste au module de config
{
    QString adresseModuleConfiguration =
      bluetooth->recupererAdresseModuleConfiguration();
    qDebug() << Q_FUNC_INFO << "Adresse du module configuration"
             << adresseModuleConfiguration << "message" << retourAssociation;
    envoyerMessage(adresseModuleConfiguration, retourAssociation);
}

void Communication::envoyerModeDeJeu(const int& information)
{
    qDebug() << Q_FUNC_INFO << "Message groupé, information" << information;
    qDebug() << "mixture"
             << typesMessages.at(Communication::TypeMessage::CONFIGURATION) +
                  QString::number(information);
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::CONFIGURATION) +
      QString::number(information));
}

void Communication::envoyerDebutDePartie()
{
    qDebug() << Q_FUNC_INFO << "Message groupé, Debut de partie";
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::DEBUT_PARTIE));
}

void Communication::signalerFinDePartie()
{
    qDebug() << Q_FUNC_INFO << "Message groupé, Fin de partie";
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::FIN_PARTIE));
}

void Communication::communiquerConfiguration(QString message)
{
    qDebug() << Q_FUNC_INFO << message;
    QString informations = nettoyerMessage(message);
    QString nombreJoueursRecu;
    QString modeDeJeuRecu;

    nombreJoueursRecu = extraireElement(informations, NOMBRE_JOUEUR);
    modeDeJeuRecu     = extraireElement(informations, MODE_DE_JEU);

    qDebug() << Q_FUNC_INFO << "Nombre joueur" << nombreJoueursRecu
             << "Mode de jeu" << modeDeJeuRecu;
    emit configurationRecue(nombreJoueursRecu, modeDeJeuRecu);
}

void Communication::gererAssociation(const QString& message)
{
    qDebug() << Q_FUNC_INFO << message;
#ifdef TEST_ASSOCIATION
    // envoyerMessageGroupe("C0");
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
    if(!nettoyerMessage(message).isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "Confirmation d'association" << message;
        confirmerAssociation(message);
    }
}

void Communication::gererChangementPage()
{
    qDebug() << Q_FUNC_INFO;
}

QString Communication::nettoyerMessage(const QString& message)
{
    QString messageCopie = message;
    return messageCopie.remove(0, 1);
    qDebug() << Q_FUNC_INFO << messageCopie;
}

QString Communication::extraireElement(const QString& informations,
                                       const int&     element)
{
    QStringList messageDecompose = informations.split(";");
    return messageDecompose[element];
}

void Communication::communiquerTirJoueur(const QString& message)
{
    qDebug() << Q_FUNC_INFO << message;
    // QString informations = nettoyerMessage(message); //att maj simulateur
    QString informations = message; // A supprimer après maj simulateur
    QString numeroPiste;
    QString scoreTir;

    numeroPiste = extraireElement(informations, NUMERO_PISTE);
    scoreTir    = extraireElement(informations, SCORE_TIR);

    qDebug() << Q_FUNC_INFO << "Piste" << numeroPiste << "Score du tir"
             << scoreTir;
    emit scoreRecu(numeroPiste, scoreTir);
}
