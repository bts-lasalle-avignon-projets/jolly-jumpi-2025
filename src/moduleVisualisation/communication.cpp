#include "communication.h"
#include "bluetooth.h"
#include "adressesPeripheriques.h"

Communication::Communication(QObject* parent) :
    QObject(parent), bluetooth(new Bluetooth)
{
    qDebug() << Q_FUNC_INFO << this;

    typesMessages << "PC" // CONFIGURATION_PARTIE
                  << "C"  // CONFIGURATION_PISTE
                  << "PD" // DEMARRER_PARTIE (ORDRE)
                  << "PF" // FINIR_PARTIE (ORDRE)
                  << "D"  // DEBUT_PARTIE (SIGNAL)
                  << "F"  // FIN_PARTIE (SIGNAL)
                  << "S0" // PAGE_ACCUEIL
                  << "S1" // PAGE_HISTORIQUE
                  << "A"  // ASSOCIATION
                  << "T"; // TIR
    communiquerModulesConnectes();
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

bool Communication::estMessageValide(const QString& message)
{
    if(message.startsWith(DEBUT_MESSAGE) && message.endsWith(FIN_MESSAGE))
    {
        qDebug() << Q_FUNC_INFO << "true";
        return true;
    }
    qDebug() << Q_FUNC_INFO << "false" << message;
    return false;
}

QString Communication::nettoyerSeparateursMessage(QString& message)
{
    message.remove(DEBUT_MESSAGE);
    message.remove(FIN_MESSAGE);
    return message;
}

void Communication::traiterMessage(QString nom,
                                   QString adresse,
                                   QString messageRecu)
{
    Communication::TypeMessage typeMessage =
      Communication::TypeMessage::INCONNU;

    if(!estMessageValide(messageRecu))
        return;
    QString message = nettoyerSeparateursMessage(messageRecu);
    typeMessage     = identifierTypeMessage(message);
    qDebug() << Q_FUNC_INFO << "nom" << nom << "adresse" << adresse << "message"
             << messageRecu << "typeMessage" << typeMessage;
    QString informations = nettoyerMessage(message, typesMessages[typeMessage]);
    switch(typeMessage)
    {
        case Communication::TypeMessage::CONFIGURATION_PARTIE:
            emit configurationEnCours();
            communiquerConfiguration(informations);
            demanderConfirmationAssociation();
            break;
        case Communication::TypeMessage::DEMARRER_PARTIE:
            emit partieDemarree();
            break;
        case Communication::TypeMessage::FINIR_PARTIE:
            emit abandonPartie();
            break;
        case Communication::TypeMessage::PAGE_ACCUEIL:
        case Communication::TypeMessage::PAGE_HISTORIQUE:
            gererChangementPage();
            break;
        case Communication::TypeMessage::ASSOCIATION:
            gererAssociation(informations);
            break;
        case Communication::TypeMessage::TIR:
            communiquerTirJoueur(informations);
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

void Communication::envoyerMessageGroupe(const QString& message)
{
    qDebug() << Q_FUNC_INFO << "message" << message;
    bluetooth->envoyerMessageGroupe(construireMessage(message));
}

void Communication::demanderConfirmationAssociation()
{
    qDebug() << Q_FUNC_INFO;
    envoyerMessage(bluetooth->recupererAdresseModuleDetectionBalles(),
                   typesMessages.at(Communication::TypeMessage::ASSOCIATION));
}

void Communication::confirmerAssociation(const QString& retourAssociation)
{
    qDebug() << Q_FUNC_INFO << "retourAssociation" << retourAssociation;
    // Signale les pistes au module de configuration
    envoyerMessage(bluetooth->recupererAdresseModuleConfiguration(),
                   typesMessages.at(Communication::TypeMessage::ASSOCIATION) +
                     retourAssociation);
}

void Communication::envoyerConfiguration(const int& modeDeJeu,
                                         const int& nombreJoueur)
{
    QString configuration =
      typesMessages.at(Communication::TypeMessage::CONFIGURATION_PISTE) +
      QString::number(modeDeJeu) + ";" + QString::number(nombreJoueur);

    qDebug() << Q_FUNC_INFO << "modeDeJeu" << modeDeJeu << "nombre joueur"
             << nombreJoueur << "message" << configuration;
    envoyerMessage(bluetooth->recupererAdresseModuleDetectionBalles(),
                   configuration);
}

void Communication::envoyerDebutDePartie()
{
    qDebug() << Q_FUNC_INFO;
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::DEBUT_PARTIE));
}

void Communication::arreterPartie()
{
    qDebug() << Q_FUNC_INFO;
    envoyerMessage(bluetooth->recupererAdresseModuleDetectionBalles(),
                   typesMessages.at(Communication::TypeMessage::FIN_PARTIE));
}

void Communication::signalerFinDePartie()
{
    qDebug() << Q_FUNC_INFO;
    envoyerMessage(bluetooth->recupererAdresseModuleConfiguration(),
                   typesMessages.at(Communication::TypeMessage::FIN_PARTIE));
}

void Communication::communiquerConfiguration(QString informations)
{
    QString nombreJoueursRecu;
    QString modeDeJeuRecu;

    nombreJoueursRecu = extraireElement(informations, NOMBRE_JOUEUR);
    modeDeJeuRecu     = extraireElement(informations, MODE_DE_JEU);

    qDebug() << Q_FUNC_INFO << "informations" << informations
             << "nombreJoueursRecu" << nombreJoueursRecu << "modeDeJeuRecu"
             << modeDeJeuRecu;
    emit configurationRecue(nombreJoueursRecu, modeDeJeuRecu);
}

void Communication::gererAssociation(const QString& message)
{
    qDebug() << Q_FUNC_INFO << message;

    if(!message.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "message" << message;
        confirmerAssociation(message);
        // Génère les numéros de piste
        QString nombrePistes = message.at(NOMBRE_PISTES);
        qDebug() << Q_FUNC_INFO << "nombrePistes" << nombrePistes;
        for(int n = 1; n <= nombrePistes.toInt(); ++n)
            pistes.append(QString::number(n));
    }
}

void Communication::gererChangementPage()
{
    qDebug() << Q_FUNC_INFO;
}

QString Communication::nettoyerMessage(const QString& message,
                                       const QString& caracteres)
{
    QString messageCopie = message;
    return messageCopie.remove(caracteres);
}

QString Communication::extraireElement(const QString& informations,
                                       const int&     element)
{
    QStringList messageDecompose = informations.split(";");
    return messageDecompose[element];
}

void Communication::communiquerTirJoueur(const QString& informations)
{
    qDebug() << Q_FUNC_INFO << informations;
    QString numeroPiste;
    QString scoreTir;

    numeroPiste = extraireElement(informations, NUMERO_PISTE);
    scoreTir    = extraireElement(informations, SCORE_TIR);

    qDebug() << Q_FUNC_INFO << "numeroPiste" << numeroPiste << "scoreTir"
             << scoreTir;
    emit scoreRecu(numeroPiste, scoreTir);
}

QList<QString> Communication::recupererPistes()
{
    qDebug() << Q_FUNC_INFO << "pistes" << pistes;
    return pistes;
}

void Communication::effacerPistes()
{
    pistes.clear();
}

void Communication::communiquerModulesConnectes()
{
    QTimer* minuteur = new QTimer(this);
    connect(minuteur,
            &QTimer::timeout,
            this,
            [this, minuteur]()
            {
                if(bluetooth->getNbConnectes() == NOMBRE_MODULE)
                {
                    minuteur->stop();
                    emit modulesConnectes();
                    return;
                }
            });

    minuteur->start(5000); // toutes les 1 secondes
}

#ifdef SIMULATION_MODULE_CONFIGURATION
void Communication::simulerModuleConfiguration()
{
    qDebug() << Q_FUNC_INFO;
    QTimer::singleShot(
      10000,
      this,
      [this]()
      {
          traiterMessage("jp-config-1", "00:00:00:00:00:00", "C0;2");
          QTimer::singleShot(
            5000,
            this,
            [this]()
            {
                traiterMessage("jp-config-1", "00:00:00:00:00:00", "D");
#ifdef SIMULATION_MODULE_CONFIGURATION_ABANDON
                QTimer::singleShot(
                  5000,
                  this,
                  [this]()
                  {
                      traiterMessage("jp-config-1", "00:00:00:00:00:00", "F");
                      QTimer::singleShot(5000,
                                         this,
                                         [this]()
                                         {
                                             traiterMessage("jp-config-1",
                                                            "00:00:00:00:00:00",
                                                            "C0;2");
                                             QTimer::singleShot(
                                               5000,
                                               this,
                                               [this]()
                                               {
                                                   traiterMessage(
                                                     "jp-config-1",
                                                     "00:00:00:00:00:00",
                                                     "D");
                                               });
                                         });
                  });
#endif
            });
      });
}
#endif
