/**
 * @file src/main.cpp
 * @brief Programme principal Jolly Jumpi 2025
 * @author Thierry Vaira
 * @version 1.0
 */
#include <Arduino.h>
#include <afficheur.h>
#include <bluetooth.h>

// Configuration table Jolly Jumpi
#define PRECISION_TIR 80 //!< Précision du tir (en %)
#define NB_TROUS      6
//#define NUMERO_PISTE 1 //!< Numéro de la piste (cf. platformio.ini)

// Configuration Bluetooth
//#define BLUETOOTH_SLAVE //!< esclave (attendra la connexion d'un client)
#define BLUETOOTH_MASTER //!< maître (se connectera au serveur)
#ifdef BLUETOOTH_MASTER
#define USE_NAME_SERVER                                                        \
    false //!< Utiliser le nom du serveur sinon l'adresse MAC
#define ENABLE_DISCOVERY true //!< Activer la recherche de périphériques
#define RECHERCHE_ASYNCHRONE                                                   \
    true                //!< Activer la recherche asynchrone sinon synchrone
#define ENABLE_SSP true //!< Activer le Secure Simple Pairing
//#define PREFIXE_NOM_SERVEUR "sedatech" //!< Le préfixe à rechercher
//#define NOM_SERVEUR         "sedatech" //!< Le nom du serveur par défaut
#define PREFIXE_NOM_SERVEUR "jp-visu" //!< Le préfixe à rechercher
#define NOM_SERVEUR         "jp-visu" //!< Le nom du serveur par défaut
#define CODE_PIN            "1234"    //!< Code PIN pour l'appairage
// Les timeout en ms
#define ATTENTE_CONNEXION    10000 //!< Temps d'attente de connexion au serveur
#define TEMPS_RECHERCHE      10000 //!< Temps d'attente de recherche de périphériques
#define LONGUEUR_ADRESSE_MAC 6 //!< Longueur de l'adresse MAC
// Si USE_NAME_SERVER à false, on utilise l'adresse MAC (plus rapide)
uint8_t adresseMACServeur[LONGUEUR_ADRESSE_MAC] = { 0x00, 0x1A, 0x7D,
                                                    0xDA, 0x71, 0x0A };
/*uint8_t adresseMACServeurDefaut[LONGUEUR_ADRESSE_MAC] = {
    0x3C, 0xE9, 0xF7, 0x61, 0x82, 0x20
};    // pour sedatech*/
#else // BLUETOOTH_SLAVE
#endif

// Brochages
#define GPIO_LED_ROUGE   5    //!< Partie finie
#define GPIO_LED_ORANGE  17   //!< Prêt à jouer
#define GPIO_LED_VERTE   16   //!< Partie en cours
#define GPIO_SW1         12   //!< Pour simuler un tir
#define ADRESSE_I2C_OLED 0x3c //!< Adresse I2C de l'OLED
#define BROCHE_I2C_SDA   21   //!< Broche SDA
#define BROCHE_I2C_SCL   22   //!< Broche SCL

// Protocole (cf. Google Drive)
#define EN_TETE_TRAME    "$"
#define DELIMITEUR_CHAMP ";"
#define DELIMITEURS_FIN  "\n"
#define DELIMITEUR_DATAS ';'
#define DELIMITEUR_FIN   '\n'

// Divers
#define BUFFER_TRAME 64 //!< Buffer pour les trames
#define LG_STR       18 //!< Longueur des messages texte (pour affichage)

/**
 * @enum TypeTrame
 * @brief Les differents types de trame
 */
enum TypeTrame
{
    Inconnu     = -1,
    ASSOCIATION = 0,
    MODE_JEU,
    DEBUT_PARTIE,
    FIN_PARTIE,
    TIR,
    NB_TRAMES
};

/**
 * @enum EtatPartie
 * @brief Les differents états d'une partie
 */
enum EtatPartie
{
    Finie = 0,
    EnCours,
    Terminee
};

/**
 * @enum ModeJeu
 * @brief Les differents modes de jeu
 */
enum ModeJeu
{
    Standard = 0,
    Aleatoire,
    Chrono,
    NbModes
};

const String nomsTrame[TypeTrame::NB_TRAMES] = {
    "A",
    "C",
    "D",
    "F",
    ""
}; //!< nom des trames dans le protocole

// variables globales
BluetoothSerial ESPBluetooth;
String          prefixeNomServeur =
  String(PREFIXE_NOM_SERVEUR);            //!< Le préfixe à rechercher
String  nomServeur = String(NOM_SERVEUR); //!< Le nom du serveur découvert
uint8_t adresseMACServeur[LONGUEUR_ADRESSE_MAC] = {
    0
};                                //!< Adresse MAC du serveur découvert
bool       connecte      = false; //!< connecté au serveur
bool       etatConnexion = false; //!< état de la connexion au serveur
bool       serveurTrouve = false; //!< serveur trouvé
bool       demandeConfirmationAppairage = false;
bool       appairageReussi              = false; //!< Appairage réussi
EtatPartie etatPartie                   = Finie; //!< l'état de la partie
ModeJeu    modeJeu                      = ModeJeu::Standard; //!< le mode de jeu
bool      estAssocie = false; //!< le module est associé pour joueur une partie
bool      tirEncours = false; //!<
int       numeroTrou = 0;     //!< de 0 à NB_TROUS
int       nbTrous    = NB_TROUS; //!< le nombre de trous détectables
bool      refresh    = false;    //!< demande rafraichissement de l'écran OLED
bool      antiRebond = false;    //!< anti-rebond
Afficheur afficheur(ADRESSE_I2C_OLED,
                    BROCHE_I2C_SDA,
                    BROCHE_I2C_SCL);                //!< afficheur OLED SSD1306
String    entete        = String(EN_TETE_TRAME);    // caractère séparateur
String    separateur    = String(DELIMITEUR_CHAMP); // caractère séparateur
String    delimiteurFin = String(DELIMITEURS_FIN);  // fin de trame

String extraireChamp(const String& trame, unsigned int numeroChamp)
{
    String       champ;
    unsigned int compteurCaractere  = 0;
    unsigned int compteurDelimiteur = 0;
    char         fin                = '\n';

    if(delimiteurFin.length() > 0)
        fin = delimiteurFin[0];

    for(unsigned int i = 0; i < trame.length(); i++)
    {
        if(trame[i] == separateur[0] || trame[i] == fin)
        {
            compteurDelimiteur++;
        }
        else if(compteurDelimiteur == numeroChamp)
        {
            champ += trame[i];
            compteurCaractere++;
        }
    }

    return champ;
}

/**
 * @brief Envoie une trame via le Bluetooth
 *
 */
void envoyerTrameTir(int numeroTable, int numeroTrou)
{
    char trameEnvoi[BUFFER_TRAME];

    // Format : $p;s\n
    sprintf((char*)trameEnvoi,
            "%s%d;%d\n",
            entete.c_str(),
            numeroTable,
            numeroTrou);
    ESPBluetooth.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG
    String trame = String(trameEnvoi);
    trame.remove(trame.indexOf("\n"), 1);
    Serial.print("[main] > ");
    Serial.println(trame);
#endif
}

void envoyerTrameAssociation()
{
    char trameEnvoi[BUFFER_TRAME];

    // Format : $Ap\n
    sprintf((char*)trameEnvoi, "%sA%d\n", entete.c_str(), NUMERO_PISTE);
    ESPBluetooth.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG
    String trame = String(trameEnvoi);
    trame.remove(trame.indexOf("\n"), 1);
    Serial.print("[main] > ");
    Serial.println(trame);
#endif
}

/**
 * @brief Déclenchée par interruption sur le bouton SW1
 * @fn tirer()
 */
void IRAM_ATTR tirer()
{
    if(etatPartie != EnCours || antiRebond || tirEncours)
        return;

    tirEncours = true;
    antiRebond = true;
}

/**
 * @brief Lit une trame via le Bluetooth
 *
 * @fn lireTrame(String &trame)
 * @param trame la trame reçue
 * @return bool true si une trame a été lue, sinon false
 */
bool lireTrame(String& trame)
{
    if(ESPBluetooth.available())
    {
        trame.clear();
        trame = ESPBluetooth.readStringUntil(DELIMITEUR_FIN);
#ifdef DEBUG
        Serial.print("[main] < ");
        Serial.println(trame);
#endif
        trame.concat(DELIMITEUR_FIN); // remet le DELIMITEUR_FIN
        return true;
    }

    return false;
}

/**
 * @brief Vérifie si la trame reçue est valide et retorune le type de la trame
 *
 * @fn verifierTrame(String &trame)
 * @param trame
 * @return TypeTrame le type de la trame
 */
TypeTrame verifierTrame(String& trame)
{
    String format;

    for(int i = 0; i < TypeTrame::NB_TRAMES; i++)
    {
        // Format : $...
        format = entete + nomsTrame[i];
#ifdef DEBUG
        Serial.print("[main] Type de trame : ");
        Serial.print(format);
        Serial.println(" ?");
#endif
        if(trame.indexOf(format) != -1)
        {
            return (TypeTrame)i;
        }
    }
#ifdef DEBUG
    Serial.println("[main] Type de trame : inconnu");
#endif
    return Inconnu;
}

void reinitialiserAffichage()
{
    afficheur.setMessageLigne(Afficheur::Ligne1, "");
    afficheur.setMessageLigne(Afficheur::Ligne2, "");
    afficheur.setMessageLigne(Afficheur::Ligne3, "");
    refresh = true;
}

/**
 * @brief Initialise les ressources du programme
 *
 * @fn setup
 *
 */
void setup()
{
    Serial.begin(115200);
    while(!Serial)
        ;
    delay(1000); // Attendre que le moniteur série soit prêt

#ifdef DEBUG
    Serial.println("[main] Jolly Jumpi 2025");
    Serial.println("[main] Piste         : " + String(NUMERO_PISTE));
    Serial.println("[main] Nb trous      : " + String(NB_TROUS));
    Serial.println("[main] Precision tir : " + String(PRECISION_TIR) + " %");
#endif

    // Configuration E/S
    pinMode(GPIO_LED_ROUGE, OUTPUT);
    pinMode(GPIO_LED_ORANGE, OUTPUT);
    pinMode(GPIO_LED_VERTE, OUTPUT);
    pinMode(GPIO_SW1, INPUT_PULLUP);
    digitalWrite(GPIO_LED_ROUGE, LOW);
    digitalWrite(GPIO_LED_ORANGE, LOW);
    digitalWrite(GPIO_LED_VERTE, LOW);

    // Simulateur de tir
    attachInterrupt(digitalPinToInterrupt(GPIO_SW1), tirer, FALLING);

    // Afficheur OLED
    afficheur.initialiser();

    String titre  = "";
    String stitre = "=====================";
    char   str[LG_STR];
    String nomBluetooth = "jp-piste-" + String(NUMERO_PISTE); // NUMERO_PISTE

    // Initialisation de la communication Bluetooth
#ifdef BLUETOOTH_MASTER
    ESPBluetooth.begin(nomBluetooth, true);

    // Copie l'adresse MAC par défaut
    mempcpy(adresseMACServeur, adresseMACServeurDefaut, LONGUEUR_ADRESSE_MAC);

    // Gère la connexion au serveur
    // Recherche du serveur ?
    if(ENABLE_DISCOVERY)
    {
        if(RECHERCHE_ASYNCHRONE)
        {
            demarrerRecherchePeripheriques();
            delay(TEMPS_RECHERCHE);
            arreterRecherchePeripheriques();
        }
        else
        {
            demarrerRecherchePeripheriques(TEMPS_RECHERCHE);
        }

        etatConnexion = false;
        if(serveurTrouve)
        {
            // il faut que les deux périphériques soient "appairés" (paired)
            if(USE_NAME_SERVER)
            {
                // il faut que le serveur soit en mode "découverte"
                // (discoverable) pour faire la résolution du nom
                connecte = connecter(nomServeur, CODE_PIN, ENABLE_SSP);
            }
            else
            {
                // Plus rapide !
                connecte = connecter(adresseMACServeur, CODE_PIN, ENABLE_SSP);
            }
        }
    }
    if(!connecte || !ENABLE_DISCOVERY)
    {
        // il faut que les deux périphériques soient "appairés" (paired)
        if(USE_NAME_SERVER)
        {
            // il faut que le serveur soit en mode "découverte" (discoverable)
            connecte = connecter(nomServeur, CODE_PIN, ENABLE_SSP);
        }
        else
        {
            connecte = connecter(adresseMACServeur, CODE_PIN, ENABLE_SSP);
        }
    }
    if(!connecte)
    {
        while(!ESPBluetooth.connected(ATTENTE_CONNEXION))
        {
            // ESPBluetooth.confirmReply(true);
            Serial.println(
              "[Jolly Jumpi] Impossible de se connecter au serveur !");
            delay(1000);
            if(appairageReussi)
            {
                // On va reessayer
                if(USE_NAME_SERVER)
                {
                    // il faut que le serveur soit en mode "découverte"
                    // (discoverable) pour faire la résolution du nom
                    connecte = connecter(nomServeur, CODE_PIN, ENABLE_SSP);
                }
                else
                {
                    // Plus rapide !
                    connecte =
                      connecter(adresseMACServeur, CODE_PIN, ENABLE_SSP);
                }
            }
        }
    }
#ifdef DEBUG
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adresseMACServeur[0],
            adresseMACServeur[1],
            adresseMACServeur[2],
            adresseMACServeur[3],
            adresseMACServeur[4],
            adresseMACServeur[5]);
    if(USE_NAME_SERVER)
        Serial.print("[main] Connexion vers le serveur " + nomServeur);
    else
        Serial.print("[main] Connexion vers le serveur " + String(str));
    Serial.print(" : ");
    Serial.println(connecte ? "ok" : "echec");
#endif
#else // BLUETOOTH_SLAVE
    ESPBluetooth.begin(nomBluetooth);
#endif
    const uint8_t* adresseESP32 = esp_bt_dev_get_address();
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adresseESP32[0],
            adresseESP32[1],
            adresseESP32[2],
            adresseESP32[3],
            adresseESP32[4],
            adresseESP32[5]);

    stitre = String("== ") + String(str) + String(" ==");
    titre  = nomBluetooth;
#ifdef DEBUG
    Serial.println("[main] Nom : " + nomBluetooth);
    Serial.println("[main] Adresse : " + String(str));
#endif

    // Initialise l'affichage
    afficheur.setTitre(titre);
    afficheur.setSTitre(stitre);
    afficheur.afficher();

    // initialise le générateur pseudo-aléatoire
    // Serial.println(randomSeed(analogRead(34)));
    esp_random();
}

/**
 * @brief Boucle infinie d'exécution du programme
 *
 * @fn loop
 *
 */
void loop()
{
    String    trame;
    TypeTrame typeTrame;

    if(ENABLE_SSP && demandeConfirmationAppairage)
    {
        ESPBluetooth.confirmReply(true);
    }

    if(refresh)
    {
        afficheur.afficher();
        refresh = false;
    }

    if(antiRebond)
    {
        afficheur.afficher();
        delay(300);
        antiRebond = false;
    }

    if(tirEncours)
    {
        char strMessageDisplay[24];
        int  tir = random(0, long(NB_TROUS * (100. / PRECISION_TIR))) +
                  1; // dépend de sa précision !
        if(tir >= 1 && tir <= NB_TROUS)
        {
            int numeroTable = NUMERO_PISTE;
            envoyerTrameTir(numeroTable, tir);
            sprintf(strMessageDisplay, "-> Piste %d trou %d", numeroTable, tir);
        }
        else
        {
            sprintf(strMessageDisplay, "-> loupé !");
        }

        afficheur.setMessageLigne(Afficheur::Ligne1, String(strMessageDisplay));
        String message = afficheur.getMessageLigne(Afficheur::Ligne2);
        message.remove(0, 3);
        afficheur.setMessageLigne(Afficheur::Ligne2, message);
        tirEncours = false;
#ifdef DEBUG
        Serial.println("[main] Tir " + String(strMessageDisplay));
#endif
        refresh = true;
    }

    if(lireTrame(trame))
    {
        typeTrame = verifierTrame(trame);
        if(typeTrame != Inconnu)
            afficheur.setMessageLigne(Afficheur::Ligne4,
                                      String("Trame : ") +
                                        nomsTrame[typeTrame]);
        refresh = true;
#ifdef DEBUG
        if(typeTrame >= 0)
            Serial.println("[main] Trame : " + nomsTrame[typeTrame]);
#endif
        switch(typeTrame)
        {
            case Inconnu:
                break;
            case TypeTrame::ASSOCIATION:
                //$A\n
                if(etatPartie == Finie)
                {
                    estAssocie = true;
                    digitalWrite(GPIO_LED_ROUGE, LOW);
                    digitalWrite(GPIO_LED_ORANGE, LOW);
                    digitalWrite(GPIO_LED_VERTE, LOW);
                    afficheur.setMessageLigne(Afficheur::Ligne2,
                                              String("Associe"));
                    afficheur.afficher();
#ifdef DEBUG
                    Serial.println("[main] Association");
#endif
                    envoyerTrameAssociation();
                }
                break;
            case TypeTrame::MODE_JEU:
                //$Cm\n
                if(etatPartie == Finie && estAssocie)
                {
                    int mode = trame.charAt(2) - '0';
                    modeJeu  = (ModeJeu)mode;
                    digitalWrite(GPIO_LED_ROUGE, LOW);
                    digitalWrite(GPIO_LED_ORANGE, HIGH);
                    digitalWrite(GPIO_LED_VERTE, LOW);
                    switch(mode)
                    {
                        case ModeJeu::Standard:
                            afficheur.setMessageLigne(Afficheur::Ligne2,
                                                      String("Mode Standard"));
                            break;
                        case ModeJeu::Aleatoire:
                            afficheur.setMessageLigne(Afficheur::Ligne2,
                                                      String("Mode Aleatoire"));
                            break;
                        case ModeJeu::Chrono:
                            afficheur.setMessageLigne(Afficheur::Ligne2,
                                                      String("Mode Chrono"));
                            break;
                        default:
                            afficheur.setMessageLigne(Afficheur::Ligne3,
                                                      String("Mode : ") +
                                                        String(mode));
                            break;
                    }
                    afficheur.afficher();
#ifdef DEBUG
                    Serial.println("[main] Mode jeu : " + String(mode));
#endif
                }
                break;
            case TypeTrame::DEBUT_PARTIE:
                if(etatPartie == Finie && estAssocie)
                {
                    reinitialiserAffichage();
                    etatPartie = EnCours;
                    digitalWrite(GPIO_LED_ROUGE, LOW);
                    digitalWrite(GPIO_LED_ORANGE, LOW);
                    digitalWrite(GPIO_LED_VERTE, HIGH);
                    afficheur.setMessageLigne(Afficheur::Ligne3,
                                              String("En cours"));
                    afficheur.afficher();
#ifdef DEBUG
                    Serial.println("[main] Nouvelle partie");
#endif
                }
                break;
            case TypeTrame::FIN_PARTIE:
                if(etatPartie == EnCours)
                {
                    reinitialiserAffichage();
                    etatPartie = Finie;
                    estAssocie = false;
                    digitalWrite(GPIO_LED_ROUGE, HIGH);
                    digitalWrite(GPIO_LED_ORANGE, LOW);
                    digitalWrite(GPIO_LED_VERTE, LOW);
                    afficheur.setMessageLigne(Afficheur::Ligne3,
                                              String("Finie"));
                    afficheur.afficher();
#ifdef DEBUG
                    Serial.println("[main] Partie finie");
#endif
                }
                break;
            default:
#ifdef DEBUG
                Serial.println("[main] Trame invalide !");
#endif
                break;
        }
    }
}
