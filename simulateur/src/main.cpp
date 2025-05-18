/**
 * @file src/main.cpp
 * @brief Programme principal Jolly Jumpi 2025
 * @author Thierry Vaira
 * @version 1.0
 */
#include "../include/jolly-jumpi.h"
#include <Arduino.h>
#include <afficheur.h>
#include <bluetooth.h>

// Configuration Bluetooth
// Définition des modes de fonctionnement (client/serveur) pour le module
#define BLUETOOTH_SLAVE  0 //!< esclave (attendra la connexion d'un client)
#define BLUETOOTH_MASTER 1 //!< maître (se connectera au serveur)
// Choix du mode
#define BLUETOOTH_MODE BLUETOOTH_MASTER
//#define BLUETOOTH_MODE BLUETOOTH_SLAVE

#define ENABLE_DISCOVERY false //!< Activer la recherche de périphériques
#define RECHERCHE_ASYNCHRONE                                                   \
    true //!< Activer la recherche asynchrone sinon synchrone
#define TEMPS_RECHERCHE 10000 //!< Temps d'attente de recherche de périphériques
//#define PREFIXE_NOM_MODULE "sedatech" //!< Le préfixe à rechercher
#define PREFIXE_NOM_MODULE "jp-visu" //!< Le préfixe à rechercher

#define ENABLE_SSP false  //!< Activer le Secure Simple Pairing
#define CODE_PIN   "1234" //!< Code PIN pour l'appairage

#define LONGUEUR_ADRESSE_MAC 6 //!< Longueur de l'adresse MAC

// Configuration des différents modes de fonctionnement
#if BLUETOOTH_MODE == BLUETOOTH_MASTER
#define USE_NAME_SERVER                                                        \
    false //!< Utiliser le nom du serveur sinon l'adresse MAC

//#define NOM_SERVEUR         "sedatech" //!< Le nom du serveur par défaut
#define NOM_SERVEUR       "jp-visu" //!< Le nom du serveur par défaut
#define ATTENTE_CONNEXION 10000     //!< Temps d'attente de connexion au serveur

// Adresse MAC par défaut du serveur
// Si USE_NAME_SERVER à false, on utilise l'adresse MAC (plus rapide)
uint8_t adresseMACServeurDefaut[LONGUEUR_ADRESSE_MAC] = {
    0x2C, 0xCF, 0x67, 0x94, 0xD1, 0xC6 
};                                     // Raspberry Pi 5
/*uint8_t adresseMACServeurDefaut[LONGUEUR_ADRESSE_MAC] = {
    0x3C, 0xE9, 0xF7, 0x61, 0x82, 0x20
}; // pour sedatech*/
#elif BLUETOOTH_MODE == BLUETOOTH_SLAVE
#else
#error Il faut choisir un mode BLUETOOTH_SLAVE ou BLUETOOTH_MASTER
#endif

// Brochages E/S
#define GPIO_LED_ROUGE   5    //!< Partie finie
#define GPIO_LED_ORANGE  17   //!< Prêt à jouer
#define GPIO_LED_VERTE   16   //!< Partie en cours
#define GPIO_SW1         12   //!< Pour simuler un tir
#define ADRESSE_I2C_OLED 0x3c //!< Adresse I2C de l'OLED
#define BROCHE_I2C_SDA   21   //!< Broche SDA
#define BROCHE_I2C_SCL   22   //!< Broche SCL

// Variables globales
// Bluetooth
BluetoothSerial ESPBluetooth;
#if BLUETOOTH_MODE == BLUETOOTH_MASTER

String  nomServeur = String(NOM_SERVEUR); //!< Le nom du serveur découvert
uint8_t adresseMACServeur[LONGUEUR_ADRESSE_MAC] = {
    0
}; //!< Adresse MAC du serveur découvert
#endif
String prefixeNomModule =
  String(PREFIXE_NOM_MODULE); //!< Le préfixe à rechercher
bool connecte = false;        //!< connecté au serveur
bool connexionVersServeur =
  false; //!< état de la connexion au serveur (MODE MASTER)
bool connexionDuClient = false; //!< état de la connexion du client (MODE SLAVE)
bool serveurTrouve     = false; //!< serveur trouvé
bool demandeConfirmationAppairage = false;
bool appairageReussi              = false; //!< Appairage réussi
bool decouverteTerminee           = false; //!< Découverte terminée

// Jolly Jumpi
EtatPartie etatPartie = Finie;             //!< l'état de la partie
ModeJeu    modeJeu    = ModeJeu::Standard; //!< le mode de jeu
bool estAssocie = false;    //!< le module est associé pour joueur une partie
bool tirEncours = false;    //!<
int  numeroTrou = 0;        //!< de 0 à NB_TROUS
int  nbTrous    = NB_TROUS; //!< le nombre de trous détectables
unsigned long tempsDebutTir = 0;
bool          refresh    = false; //!< demande rafraichissement de l'écran OLED
bool          antiRebond = false; //!< anti-rebond
String        entete     = String(EN_TETE_TRAME);
String        separateur = String(DELIMITEUR_CHAMP);
String        delimiteurFin = String(DELIMITEURS_FIN);
Afficheur     afficheur(ADRESSE_I2C_OLED,
                    BROCHE_I2C_SDA,
                    BROCHE_I2C_SCL); //!< afficheur OLED SSD1306

// Les fonctions
/**
 * @brief Détermine le nombre de points en fonction du trou
 * @fn getScore(int tir)
 */
int getScore(int tir, unsigned long tempsReponse)
{
    char str[LG_STR];
    int  points = 0;
    int  bonus  = 1;

    switch(modeJeu)
    {
        case ModeJeu::Standard:
#ifdef DEBUG
            Serial.println("[main] Mode : standard");
            Serial.println("[main] Trou : " + String(tir) +
                           String(" -> score : ") +
                           String(pointsTrou[tir - 1]));
#endif
            return pointsTrou[tir - 1];
            break;
        case ModeJeu::Aleatoire:
            points = random(0, long(NB_TROUS)) + 1;
#ifdef DEBUG
            Serial.println("[main] Mode : aléatoire");
            Serial.println("[main] Trou : " + String(points));
            Serial.println("[main] Score : " + String(pointsTrou[points]));
#endif
            return pointsTrou[points];
            break;
        case ModeJeu::Chrono:
            if(tempsReponse < TIR_RAPIDE)
                bonus = 2;
            points = pointsTrou[tir - 1] * bonus;
#ifdef DEBUG
            Serial.println("[main] Mode : chrono");
            Serial.println("[main] Trou : " + String(tir));
            sprintf(str, "Temps tir : %lu ms", tempsReponse);
            Serial.println("[main] " + String(str));
            Serial.println("[main] Score : " + String(points));
#endif
            return points;
            break;
        default:
            return 0;
    }
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

void reinitialiserAffichage()
{
    afficheur.setMessageLigne(Afficheur::Ligne1, ""); // Mode
    afficheur.setMessageLigne(Afficheur::Ligne2, ""); // Action
    afficheur.setMessageLigne(Afficheur::Ligne3, ""); // Etat/Résultat
    refresh = true;
}

void initialiserAffichagePartie(String message)
{
    afficheur.setMessageLigne(Afficheur::Ligne1, String("Connecte"));
    switch(modeJeu)
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
            afficheur.setMessageLigne(Afficheur::Ligne2, String("Mode Chrono"));
            break;
        default:
            afficheur.setMessageLigne(Afficheur::Ligne2, String("Mode ?"));
            break;
    }
    afficheur.setMessageLigne(Afficheur::Ligne3, message); // Etat de la partie
    afficheur.setMessageLigne(Afficheur::Ligne4, String("")); // Tir score
    refresh = true;
}

void afficherBluetooth(String message)
{
    afficheur.setMessageLigne(Afficheur::Ligne1, message);
    afficheur.afficher();
}

void afficherConnexion(String message)
{
#if BLUETOOTH_MODE == BLUETOOTH_MASTER
    afficheur.setMessageLigne(Afficheur::Ligne1, String("Mode master"));
#else // BLUETOOTH_SLAVE
    afficheur.setMessageLigne(Afficheur::Ligne1, String("Mode slave"));
#endif
    afficheur.setMessageLigne(Afficheur::Ligne2, message);
    afficheur.setMessageLigne(Afficheur::Ligne3, "");
    afficheur.afficher();
}

void afficherErreurConnexion()
{
    afficheur.setMessageLigne(Afficheur::Ligne3, "Erreur connexion");
    afficheur.afficher();
}

void afficherConnexionReussie()
{
    afficheur.setMessageLigne(Afficheur::Ligne3, "Connexion reussie");
    afficheur.afficher();
}

void demarrerRecherche()
{
    if(RECHERCHE_ASYNCHRONE)
    {
        afficheur.setMessageLigne(Afficheur::Ligne2, "Recherche ...");
        afficheur.afficher();
        demarrerRecherchePeripheriques();
        delay(TEMPS_RECHERCHE);
        arreterRecherchePeripheriques();
        afficheur.setMessageLigne(Afficheur::Ligne2, "Recherche finie.");
        afficheur.afficher();
    }
    else
    {
        afficheur.setMessageLigne(Afficheur::Ligne2, "Recherche ...");
        afficheur.afficher();
        demarrerRecherchePeripheriques(TEMPS_RECHERCHE);
        afficheur.setMessageLigne(Afficheur::Ligne2, "Recherche finie.");
        afficheur.afficher();
    }
}

#if BLUETOOTH_MODE == BLUETOOTH_MASTER
void gererConnexionServeur()
{
    char str[LG_STR];
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adresseMACServeur[0],
            adresseMACServeur[1],
            adresseMACServeur[2],
            adresseMACServeur[3],
            adresseMACServeur[4],
            adresseMACServeur[5]);

    // Gère la connexion au serveur
    // Recherche du serveur ?
    if(ENABLE_DISCOVERY)
    {
        demarrerRecherche();

        if(serveurTrouve)
        {
            // il faut que les deux périphériques soient "appairés" (paired)
            if(USE_NAME_SERVER)
            {
                afficherConnexion(String("-> ") + nomServeur);
                // il faut que le serveur soit en mode "découverte"
                // (discoverable) pour faire la résolution du nom
                connecte = connecter(nomServeur, CODE_PIN, ENABLE_SSP);
            }
            else
            {
                afficherConnexion(String("-> ") + String(str));
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
            afficherConnexion(String("-> ") + nomServeur);
            // il faut que le serveur soit en mode "découverte" (discoverable)
            connecte = connecter(nomServeur, CODE_PIN, ENABLE_SSP);
        }
        else
        {
            afficherConnexion(String("-> ") + String(str));
            connecte = connecter(adresseMACServeur, CODE_PIN, ENABLE_SSP);
        }
    }
    if(!connecte)
    {
        while(!ESPBluetooth.connected(ATTENTE_CONNEXION))
        {
            // ESPBluetooth.confirmReply(true);
            if(USE_NAME_SERVER)
                afficherConnexion(String("-> ") + nomServeur);
            else
                afficherConnexion(String("-> ") + String(str));
            afficherErreurConnexion();
#ifdef DEBUG
            Serial.println("[main] Impossible de se connecter au serveur !");
#endif
            delay(1000);
            // if(appairageReussi)
            {
                /*afficheur.setMessageLigne(Afficheur::Ligne2,
                                          String("Appairage ok"));*/
                // On va reessayer
                if(USE_NAME_SERVER)
                {
                    afficherConnexion(String("-> ") + nomServeur);
                    // il faut que le serveur soit en mode "découverte"
                    // (discoverable) pour faire la résolution du nom
                    connecte = connecter(nomServeur, CODE_PIN, ENABLE_SSP);
                }
                else
                {
                    afficherConnexion(String("-> ") + String(str));
                    // Plus rapide !
                    connecte =
                      connecter(adresseMACServeur, CODE_PIN, ENABLE_SSP);
                }
            }
        }
    }
    if(connecte || connexionVersServeur)
    {
        afficherConnexionReussie();
    }
    else
    {
        afficherErreurConnexion();
    }
#ifdef DEBUG
    if(USE_NAME_SERVER)
        Serial.print("[main] Connexion vers le serveur " + nomServeur);
    else
        Serial.print("[main] Connexion vers le serveur " + String(str));
    Serial.print(" : ");
    Serial.println(connecte ? "ok" : "echec");
    Serial.println("[main] connexionVersServeur : " +
                   String(connexionVersServeur));
#endif
}
#endif

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

    // Initialise l'afficheur OLED
    afficheur.initialiser();

    String titre  = "            ";
    String stitre = "=====================";
    char   str[LG_STR];
    String nomBluetooth = "jp-piste-" + String(NUMERO_PISTE); // NUMERO_PISTE

    // Initialisation de la communication Bluetooth
#if BLUETOOTH_MODE == BLUETOOTH_MASTER
#ifdef DEBUG
    Serial.println("[main] Bluetooth mode : master");
#endif
    ESPBluetooth.begin(nomBluetooth, true);
#elif BLUETOOTH_MODE == BLUETOOTH_SLAVE
    ESPBluetooth.begin(nomBluetooth);
#ifdef DEBUG
    Serial.println("[main] Bluetooth mode : slave");
#endif
#endif

    // Affichage informations Bluetooth
    const uint8_t* adresseESP32 = esp_bt_dev_get_address();
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adresseESP32[0],
            adresseESP32[1],
            adresseESP32[2],
            adresseESP32[3],
            adresseESP32[4],
            adresseESP32[5]);
    titre += nomBluetooth;
    stitre = String("== ") + String(str) + String(" ==");
#ifdef DEBUG
    Serial.println("[main] Nom : " + nomBluetooth);
    Serial.println("[main] Adresse : " + String(str));
#endif
    afficheur.setTitre(titre);
    afficheur.setSTitre(stitre);
#if BLUETOOTH_MODE == BLUETOOTH_MASTER
    afficherBluetooth("Mode master");
#else // BLUETOOTH_SLAVE
    afficherBluetooth("Mode slave");
#endif

    // supprimerPeripheriquesAppaires();
    afficherPeripheriquesAppaires();

    // Initialisation de la communication Bluetooth
#if BLUETOOTH_MODE == BLUETOOTH_MASTER
    // Copie l'adresse MAC par défaut
    mempcpy(adresseMACServeur, adresseMACServeurDefaut, LONGUEUR_ADRESSE_MAC);
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adresseMACServeur[0],
            adresseMACServeur[1],
            adresseMACServeur[2],
            adresseMACServeur[3],
            adresseMACServeur[4],
            adresseMACServeur[5]);

    estPeripheriqueAppaire(adresseMACServeur); // Vérifie si le périphérique par
    // défaut est déjà appairé

    gererConnexionServeur();

    if(connecte)
    {
        afficheur.setMessageLigne(Afficheur::Ligne3, "Connexion ok");
        if(USE_NAME_SERVER)
            afficheur.setMessageLigne(Afficheur::Ligne2,
                                      String("Serveur ") + nomServeur);
        else
            afficheur.setMessageLigne(Afficheur::Ligne2,
                                      String("Serveur ") + String(str));
    }
    else
    {
        afficheur.setMessageLigne(Afficheur::Ligne3, "Erreur connexion");
        if(USE_NAME_SERVER)
            afficheur.setMessageLigne(Afficheur::Ligne2,
                                      String("Serveur ") + nomServeur);
        else
            afficheur.setMessageLigne(Afficheur::Ligne2,
                                      String("Serveur ") + String(str));
    }
    afficheur.afficher();
#elif BLUETOOTH_MODE == BLUETOOTH_SLAVE
#endif
    // initialise le générateur pseudo-aléatoire
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
            unsigned long tempsReponse = millis() - tempsDebutTir;
            int           score        = getScore(tir, tempsReponse);
            int           numeroTable  = NUMERO_PISTE;
            envoyerTrameTir(numeroTable, score);
            sprintf(strMessageDisplay, "-> Trou %d - score %d", tir, score);
            tempsDebutTir = millis();
        }
        else
        {
            sprintf(strMessageDisplay, "-> loupé !");
        }

        afficheur.setMessageLigne(Afficheur::Ligne4, String(strMessageDisplay));
        tirEncours = false;
        refresh    = true;
    }

    if(lireTrame(trame))
    {
        typeTrame = verifierTrame(trame);
#ifdef DEBUG
        /*if(typeTrame >= 0)
            Serial.println("[main] Trame : " + nomsTrame[typeTrame]);*/
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
                    initialiserAffichagePartie(String("Association"));
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
                    initialiserAffichagePartie(String("Attente départ"));
                    afficheur.afficher();
#ifdef DEBUG
                    Serial.println("[main] Mode jeu : " + String(mode));
#endif
                }
                break;
            case TypeTrame::DEBUT_PARTIE:
                if(etatPartie == Finie && estAssocie)
                {
                    etatPartie = EnCours;
                    digitalWrite(GPIO_LED_ROUGE, LOW);
                    digitalWrite(GPIO_LED_ORANGE, LOW);
                    digitalWrite(GPIO_LED_VERTE, HIGH);

                    initialiserAffichagePartie(String("En cours"));
                    afficheur.afficher();
#ifdef DEBUG
                    Serial.println("[main] Nouvelle partie");
#endif
                    if(modeJeu == ModeJeu::Chrono)
                    {
                        tempsDebutTir = millis();
                    }
                }
                break;
            case TypeTrame::FIN_PARTIE:
                if(etatPartie == EnCours)
                {
                    etatPartie = Finie;
                    estAssocie = false;
                    digitalWrite(GPIO_LED_ROUGE, HIGH);
                    digitalWrite(GPIO_LED_ORANGE, LOW);
                    digitalWrite(GPIO_LED_VERTE, LOW);
                    initialiserAffichagePartie(String("Finie"));
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

#if BLUETOOTH_MODE == BLUETOOTH_MASTER
    if(!connecte || !connexionVersServeur)
    {
        gererConnexionServeur();
    }
#endif
}
