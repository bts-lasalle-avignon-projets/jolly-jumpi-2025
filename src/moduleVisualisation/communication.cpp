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
        case Communication::TypeMessage::TIR:
            communiquerTirJoueur(message);
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
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::ASSOCIATION));
}

void Communication::confirmerAssociation(const QString& retourAssociation)
{
    QString adresseModuleConfiguration =
      bluetooth->recupererAdresseModuleConfiguration();
    qDebug() << Q_FUNC_INFO << "adresseModuleConfiguration"
             << adresseModuleConfiguration << "retourAssociation"
             << retourAssociation;
    // Signale la piste au module de configuration
    envoyerMessage(adresseModuleConfiguration, retourAssociation);
}

void Communication::envoyerConfiguration(const int& modeDeJeu,
                                         const int& nombreJoueur)
{
    QString configuration =
      typesMessages.at(Communication::TypeMessage::CONFIGURATION) +
      QString::number(modeDeJeu) + ";" + QString::number(nombreJoueur);

    qDebug() << Q_FUNC_INFO << "modeDeJeu" << modeDeJeu << "nombre joueur"
             << nombreJoueur << "message" << configuration;
    envoyerMessageGroupe(configuration);
}

void Communication::envoyerDebutDePartie()
{
    qDebug() << Q_FUNC_INFO;
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::DEBUT_PARTIE));
}

void Communication::signalerFinDePartie()
{
    qDebug() << Q_FUNC_INFO;
    envoyerMessageGroupe(
      typesMessages.at(Communication::TypeMessage::FIN_PARTIE));
}

void Communication::communiquerConfiguration(QString message)
{
    QString informations = nettoyerMessage(message);
    QString nombreJoueursRecu;
    QString modeDeJeuRecu;

    nombreJoueursRecu = extraireElement(informations, NOMBRE_JOUEUR);
    modeDeJeuRecu     = extraireElement(informations, MODE_DE_JEU);

    qDebug() << Q_FUNC_INFO << "message" << message << "nombreJoueursRecu"
             << nombreJoueursRecu << "modeDeJeuRecu" << modeDeJeuRecu;
    emit configurationRecue(nombreJoueursRecu, modeDeJeuRecu);
}

void Communication::gererAssociation(const QString& message)
{
    qDebug() << Q_FUNC_INFO << message;

    if(!nettoyerMessage(message).isEmpty())
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

QString Communication::nettoyerMessage(const QString& message)
{
    QString messageCopie = message;
    return messageCopie.remove(0, 1);
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
    QString informations = nettoyerMessage(message);
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

#ifdef SIMULATION_MODULE_CONFIGURATION
void Communication::simulerModuleConfiguration()
{
    qDebug() << Q_FUNC_INFO;
    QTimer::singleShot(
      2000,
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
