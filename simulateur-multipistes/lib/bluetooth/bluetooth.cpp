#include "bluetooth.h"

extern BluetoothSerial ESPBluetooth;
extern String          prefixeNomModule;
extern String          nomServeur;
extern uint8_t         adresseMACServeur[ESP_BD_ADDR_LEN];

extern bool
  connexionVersServeur; //!< état de la connexion au serveur (MODE MASTER)
extern bool connexionDuClient; //!< état de la connexion du client (MODE SLAVE)
extern bool serveurTrouve;
extern bool demandeConfirmationAppairage;
extern bool appairageReussi;
extern bool decouverteTerminee;

extern String entete;
extern String separateur;
extern String delimiteurFin;

bool initialiserBluetooth(String nomBluetooth, const char* pin, bool enableSSP)
{
    bool initialise = false;

#if ESP_ARDUINO_VERSION < ESP_ARDUINO_VERSION_VAL(3, 0, 0)
#if BLUETOOTH_MODE == BLUETOOTH_MASTER
#ifdef DEBUG
    Serial.println("[Bluetooth] Bluetooth mode : master");
#endif
    initialise = ESPBluetooth.begin(nomBluetooth, true);
#elif BLUETOOTH_MODE == BLUETOOTH_SLAVE
#ifdef DEBUG
    Serial.println("[Bluetooth] Bluetooth mode : slave");
#endif
    initialise = ESPBluetooth.begin(nomBluetooth);
#endif
#endif

    if(pin && *pin)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Code PIN : " + String(pin));
#endif
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 2, 0)
        ESPBluetooth.setPin(pin, strlen(pin)); // v3.2.0
#else
        ESPBluetooth.setPin(pin); // v2.2.0
#endif
    }
    if(enableSSP)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Activer SSP (Secure Simple Pairing)");
#endif
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 2, 0)
        ESPBluetooth.enableSSP(false, false); // NoInputNoOutput
#else
        ESPBluetooth.enableSSP();
#endif
        ESPBluetooth.onConfirmRequest(demanderConfirmation);
        ESPBluetooth.onAuthComplete(finaliserAuthentification);
    }
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 2, 0)
    else
    {
        ESPBluetooth.disableSSP();
    }
#endif

#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 2, 0)
#if BLUETOOTH_MODE == BLUETOOTH_MASTER
#ifdef DEBUG
    Serial.println("[Bluetooth] Bluetooth mode : master");
#endif
    initialise = ESPBluetooth.begin(nomBluetooth, true);
#elif BLUETOOTH_MODE == BLUETOOTH_SLAVE
#ifdef DEBUG
    Serial.println("[Bluetooth] Bluetooth mode : slave");
#endif
    initialise = ESPBluetooth.begin(nomBluetooth);
#endif
#endif

    return initialise;
}

void trouverPeripherique(BTAdvertisedDevice* peripherique)
{
#ifdef DEBUG
    Serial.println("[Bluetooth] Péripherique découvert : " +
                   String(peripherique->getName().c_str()) + " [" +
                   String(peripherique->getAddress().toString().c_str()) +
                   "] " + String(peripherique->getRSSI()) + " dBm");
#endif
    if(peripherique->haveName())
    {
        String prefixeNom = String(peripherique->getName().c_str());
        if(prefixeNom.startsWith(prefixeNomModule))
        {
#ifdef DEBUG
            Serial.println("[Bluetooth] Serveur découvert !");
#endif
            if(estPeripheriqueAppaire(
                 (uint8_t*)peripherique->getAddress().getNative()))
            {
                serveurTrouve = true;
                nomServeur    = String(peripherique->getName().c_str());
                mempcpy(adresseMACServeur,
                        peripherique->getAddress().getNative(),
                        ESP_BD_ADDR_LEN);
#ifdef DEBUG
                Serial.println("[Bluetooth] Serveur découvert appairé !");
#endif
            }
            else
            {
                // Demander l'appairage
                // ESPBluetooth.pair(peripherique->getAddress().getNative());
            }
        }
    }
}

void demanderConfirmation(uint32_t numVal)
{
    demandeConfirmationAppairage = true;
#ifdef DEBUG
    Serial.print("[Bluetooth] Demande de confirmation : ");
    Serial.println(numVal);
#endif
    ESPBluetooth.confirmReply(true);
}

void finaliserAuthentification(boolean succes)
{
    demandeConfirmationAppairage = false;
    if(succes)
    {
        appairageReussi = true;
#ifdef DEBUG
        Serial.println("[Bluetooth] Appairage réussi !");
#endif
    }
    else
    {
        appairageReussi = false;
#ifdef DEBUG
        Serial.println("[Bluetooth] Echec apparaige !");
#endif
    }
}

void gererEvenementBluetooth(esp_spp_cb_event_t  event,
                             esp_spp_cb_param_t* param)
{
    switch(event)
    {
        case ESP_SPP_INIT_EVT:
            // Initialisation réussie
#ifdef DEBUG
            Serial.println("[Bluetooth] Initialisation réussie !");
#endif
            break;
        case ESP_SPP_DISCOVERY_COMP_EVT:
            // Découverte terminée
            decouverteTerminee = true;
#ifdef DEBUG
            Serial.println("[Bluetooth] Découverte terminée !");
#endif
            break;
        case ESP_SPP_OPEN_EVT:
            // On est connecté au serveur
            connexionVersServeur = true;
#ifdef DEBUG
            Serial.println(
              "[Bluetooth] On est connecté au serveur ! (MODE MASTER)");
#endif
            break;
        case ESP_SPP_CLOSE_EVT:
            // Déconnexion réussie
            connexionVersServeur = false;
            connexionDuClient    = false;
#ifdef DEBUG
            Serial.println("[Bluetooth] Déconnecté !");
#endif
            break;
        case ESP_SPP_START_EVT:
            // Serveur démarré
#ifdef DEBUG
            Serial.println("[Bluetooth] Serveur démarré ! (MODE SLAVE)");
#endif
            break;
        case ESP_SPP_CL_INIT_EVT:
            // Client initialisé
#ifdef DEBUG
            Serial.println("[Bluetooth] Client initialisé ! (MODE MASTER)");
#endif
            break;
        case ESP_SPP_DATA_IND_EVT:
            // Données reçues
            break;
        case ESP_SPP_WRITE_EVT:
            // Données envoyées
            break;
        case ESP_SPP_SRV_OPEN_EVT:
            // On est connecté au client
            connexionDuClient = true;
#ifdef DEBUG
            Serial.println(
              "[Bluetooth] On est connecté au client ! (MODE SLAVE)");
#endif
            break;
        case ESP_SPP_SRV_STOP_EVT:
            // Serveur arrêté
            connexionDuClient = false;
#ifdef DEBUG
            Serial.println("[Bluetooth] Serveur arrêté ! (MODE SLAVE)");
#endif
            break;
        default: // Autre événement
#ifdef DEBUG
            Serial.println("[Bluetooth] event : " + String(event));
#endif
            break;
    }
}

void demarrerRecherchePeripheriques(uint32_t tempsRecherche)
{
#ifdef DEBUG
    Serial.println("[Bluetooth] Recherche de péripheriques");
#endif
    BTScanResults* resultatsRecherche = ESPBluetooth.discover(tempsRecherche);
#ifdef DEBUG
    Serial.println("[Bluetooth] Recherche de péripheriques terminée");
    Serial.println("[Bluetooth] Nombre de péripheriques découverts : " +
                   String(resultatsRecherche->getCount()));
    for(int i = 0; i < resultatsRecherche->getCount(); i++)
    {
        BTAdvertisedDevice* peripherique = resultatsRecherche->getDevice(i);
        Serial.println("[Bluetooth] Péripherique découvert : " +
                       String(peripherique->getName().c_str()) + " [" +
                       String(peripherique->getAddress().toString().c_str()) +
                       "] " + String(peripherique->getRSSI()) + " dBm");
        if(peripherique->haveName())
        {
            String prefixeNom = String(peripherique->getName().c_str());
            if(prefixeNom.startsWith(prefixeNomModule))
            {
#ifdef DEBUG
                Serial.println("[Bluetooth] Serveur découvert !");
#endif
                if(estPeripheriqueAppaire(
                     (uint8_t*)peripherique->getAddress().getNative()))
                {
                    serveurTrouve = true;
                    nomServeur    = String(peripherique->getName().c_str());
                    mempcpy(adresseMACServeur,
                            peripherique->getAddress().getNative(),
                            ESP_BD_ADDR_LEN);
#ifdef DEBUG
                    Serial.println("[Bluetooth] Serveur découvert appairé !");
#endif
                }
                else
                {
                    // Demander l'appairage
                    // ESPBluetooth.pair(peripherique->getAddress().getNative());
                }
            }
        }
    }
#endif
}

void demarrerRecherchePeripheriques()
{
#ifdef DEBUG
    Serial.println("[Bluetooth] Recherche asynchrone de péripheriques");
#endif
    ESPBluetooth.discoverAsync(trouverPeripherique);
}

void arreterRecherchePeripheriques()
{
#ifdef DEBUG
    Serial.println(
      "[Bluetooth] Arrêt de la recherche asynchrone de péripheriques");
#endif
    ESPBluetooth.discoverAsyncStop();
}

bool connecter(uint8_t adresseDistante[])
{
    if(!ESPBluetooth.isReady(true, ATTENTE_PRET))
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Non initialisé ou mode master non actif !");
#endif
        return false;
    }
#ifdef DEBUG
    char str[18];
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adresseDistante[0],
            adresseDistante[1],
            adresseDistante[2],
            adresseDistante[3],
            adresseDistante[4],
            adresseDistante[5]);
    Serial.println("[Bluetooth] Connexion vers le serveur " + String(str) +
                   String(" ..."));
#endif
    ESPBluetooth.register_callback(gererEvenementBluetooth);
    bool estConnecte =
      // ESPBluetooth.connect(adresseDistante, 0, ESP_SPP_SEC_NONE);
      ESPBluetooth.connect(adresseDistante);
    if(estConnecte)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Connexion réussie !");
#endif
    }
    else
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Echec connexion !");
#endif
    }

    return estConnecte;
}

bool connecter(String nomDistant)
{
    if(!ESPBluetooth.isReady(true, ATTENTE_PRET))
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Non initialisé ou mode master non actif !");
#endif
        return false;
    }
    ESPBluetooth.register_callback(gererEvenementBluetooth);
#ifdef DEBUG
    Serial.println("[Bluetooth] Connexion vers le serveur " + nomDistant +
                   String(" ..."));
#endif
    bool estConnecte = ESPBluetooth.connect(nomDistant);
    if(estConnecte)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Connexion réussie !");
#endif
    }
    else
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Echec connexion !");
#endif
    }

    return estConnecte;
}

bool reconnecter()
{
    if(!ESPBluetooth.isReady(true, ATTENTE_PRET))
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Non initialisé ou mode master non actif !");
#endif
        return false;
    }
    // 10 s max
    if(ESPBluetooth.disconnect())
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Déconnexion réussie !");
#endif
    }
#ifdef DEBUG
    Serial.println("[Bluetooth] Tentative reconnexion");
#endif

    return ESPBluetooth.connect();
}

void afficherPeripheriquesAppaires()
{
    int nbPeripheriquesAppaires = getNbPeripheriquesAppaires();
    if(nbPeripheriquesAppaires == 0)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Aucun péripherique appairé !");
#endif
        return;
    }

    esp_bd_addr_t* listePeripheriquesAppaires = nullptr;
    listePeripheriquesAppaires =
      (esp_bd_addr_t*)malloc(sizeof(esp_bd_addr_t) * nbPeripheriquesAppaires);
    if(listePeripheriquesAppaires == nullptr)
    {
        return;
    }

    int nbPeripheriquesAppairesLus =
      getPeripheriquesAppaires(nbPeripheriquesAppaires,
                               listePeripheriquesAppaires);
    if(nbPeripheriquesAppairesLus == 0)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Aucun péripherique appairé !");
#endif
        return;
    }
    for(int i = 0; i < nbPeripheriquesAppairesLus; i++)
    {
        char str[18];
        sprintf(str,
                "%02x:%02x:%02x:%02x:%02x:%02x",
                listePeripheriquesAppaires[i][0],
                listePeripheriquesAppaires[i][1],
                listePeripheriquesAppaires[i][2],
                listePeripheriquesAppaires[i][3],
                listePeripheriquesAppaires[i][4],
                listePeripheriquesAppaires[i][5]);
#ifdef DEBUG
        Serial.print("[Bluetooth] Péripherique appairé : ");
        Serial.println(str);
#endif
    }
    free(listePeripheriquesAppaires);
}

int getNbPeripheriquesAppaires()
{
    return esp_bt_gap_get_bond_device_num();
}

bool estPeripheriqueAppaire(uint8_t* adressePeripherique)
{
    int nbPeripheriquesAppaires = getNbPeripheriquesAppaires();
    if(nbPeripheriquesAppaires == 0)
    {
        return false;
    }

    esp_bd_addr_t* listePeripheriquesAppaires = nullptr;
    listePeripheriquesAppaires =
      (esp_bd_addr_t*)malloc(sizeof(esp_bd_addr_t) * nbPeripheriquesAppaires);
    if(listePeripheriquesAppaires == nullptr)
    {
        return false;
    }

    int nbPeripheriquesAppairesLus =
      getPeripheriquesAppaires(nbPeripheriquesAppaires,
                               listePeripheriquesAppaires);
    if(nbPeripheriquesAppairesLus == 0)
    {
        return false;
    }
    for(int i = 0; i < nbPeripheriquesAppairesLus; i++)
    {
        if(memcmp(adressePeripherique,
                  listePeripheriquesAppaires[i],
                  ESP_BD_ADDR_LEN) == 0)
        /*if(listePeripheriquesAppaires[i][0] == adressePeripherique[0]
           && listePeripheriquesAppaires[i][1] == adressePeripherique[1]
           && listePeripheriquesAppaires[i][2] == adressePeripherique[2]
           && listePeripheriquesAppaires[i][3] == adressePeripherique[3]
           && listePeripheriquesAppaires[i][4] == adressePeripherique[4]
           && listePeripheriquesAppaires[i][5] ==
           adressePeripherique[5])*/
        {
#ifdef DEBUG
            char str[18];
            sprintf(str,
                    "%02x:%02x:%02x:%02x:%02x:%02x",
                    listePeripheriquesAppaires[i][0],
                    listePeripheriquesAppaires[i][1],
                    listePeripheriquesAppaires[i][2],
                    listePeripheriquesAppaires[i][3],
                    listePeripheriquesAppaires[i][4],
                    listePeripheriquesAppaires[i][5]);
            Serial.println("[Bluetooth] Péripherique " + String(str) +
                           " appairé");
#endif
            free(listePeripheriquesAppaires);
            return true;
        }
    }
#ifdef DEBUG
    char str[18];
    sprintf(str,
            "%02x:%02x:%02x:%02x:%02x:%02x",
            adressePeripherique[0],
            adressePeripherique[1],
            adressePeripherique[2],
            adressePeripherique[3],
            adressePeripherique[4],
            adressePeripherique[5]);
    Serial.println("[Bluetooth] Péripherique " + String(str) + " non appairé");
#endif
    free(listePeripheriquesAppaires);
    return false;
}

int getPeripheriquesAppaires(int nb, esp_bd_addr_t* listePeripheriquesAppaires)
{
    // typedef uint8_t esp_bd_addr_t[ESP_BD_ADDR_LEN]
    if(listePeripheriquesAppaires == nullptr || nb == 0)
    {
        return 0;
    }
    int nbLus = nb;
    esp_bt_gap_get_bond_device_list(&nbLus, listePeripheriquesAppaires);
    return nbLus;
}

bool supprimerAppairagePeripherique(uint8_t* adressePeripherique)
{
    esp_err_t retour = esp_bt_gap_remove_bond_device(adressePeripherique);
    if(retour == ESP_OK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void supprimerPeripheriquesAppaires()
{
    int nbPeripheriquesAppaires = getNbPeripheriquesAppaires();
    if(nbPeripheriquesAppaires == 0)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Aucun periphérique appairé !");
#endif
        return;
    }

    esp_bd_addr_t* listePeripheriquesAppaires = nullptr;
    listePeripheriquesAppaires =
      (esp_bd_addr_t*)malloc(sizeof(esp_bd_addr_t) * nbPeripheriquesAppaires);
    if(listePeripheriquesAppaires == nullptr)
    {
        return;
    }

    int nbPeripheriquesAppairesLus =
      getPeripheriquesAppaires(nbPeripheriquesAppaires,
                               listePeripheriquesAppaires);
    if(nbPeripheriquesAppairesLus == 0)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Aucun péripherique appairé !");
#endif
        return;
    }
    for(int i = 0; i < nbPeripheriquesAppairesLus; i++)
    {
        char str[18];
        sprintf(str,
                "%02x:%02x:%02x:%02x:%02x:%02x",
                listePeripheriquesAppaires[i][0],
                listePeripheriquesAppaires[i][1],
                listePeripheriquesAppaires[i][2],
                listePeripheriquesAppaires[i][3],
                listePeripheriquesAppaires[i][4],
                listePeripheriquesAppaires[i][5]);
#ifdef DEBUG
        Serial.print("[Bluetooth] Suppression péripherique appairé [" +
                     String(str) + "] : ");
#endif
        bool retour =
          supprimerAppairagePeripherique(listePeripheriquesAppaires[i]);
#ifdef DEBUG
        Serial.println(retour ? "ok" : "echec");
#endif
    }
    free(listePeripheriquesAppaires);
}

/*void getNomPeripherique(uint8_t adresseDistante[])
{
    if(ESPBluetooth.isReady(true, ATTENTE_PRET))
    {
        esp_bt_gap_read_remote_name(adresseDistante);
    }
}*/

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
        Serial.print("[Bluetooth] < ");
        Serial.println(trame);
#endif
        trame.concat(DELIMITEUR_FIN); // remet le DELIMITEUR_FIN
        return true;
    }

    return false;
}

/**
 * @brief Vérifie si la trame reçue est valide et retourne le type de la
 * trame
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
        if(trame.indexOf(format) != -1)
        {
            return (TypeTrame)i;
        }
    }
#ifdef DEBUG
    Serial.println("[Bluetooth] Type de trame : inconnu");
#endif
    return Inconnu;
}

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
void envoyerTrameTir(int numeroTable, int score)
{
    char trameEnvoi[BUFFER_TRAME];

    // Format : $T;p;s\n
    sprintf((char*)trameEnvoi,
            "%sT%d;%d\n",
            entete.c_str(),
            numeroTable,
            score);
    ESPBluetooth.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG
    String trame = String(trameEnvoi);
    trame.remove(trame.indexOf("\n"), 1);
    Serial.print("[Bluetooth] > ");
    Serial.println(trame);
#endif
}

void envoyerTrameAssociation(int nbPistes /*= NB_PISTES*/)
{
    char trameEnvoi[BUFFER_TRAME];

    /*for(int i = 0; i < nbPistes; i++)
    {
        // Format : $Ap\n
        sprintf((char*)trameEnvoi, "%sA%d\n", entete.c_str(), i);
        ESPBluetooth.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
        delay(50);
#ifdef DEBUG
        String trame = String(trameEnvoi);
        trame.remove(trame.indexOf("\n"), 1);
        Serial.print("[Bluetooth] > ");
        Serial.println(trame);
#endif
    }*/

    // Format : $Ap\n
    sprintf((char*)trameEnvoi, "%sA%d\n", entete.c_str(), nbPistes);
    ESPBluetooth.write((uint8_t*)trameEnvoi, strlen((char*)trameEnvoi));
#ifdef DEBUG
    String trame = String(trameEnvoi);
    trame.remove(trame.indexOf("\n"), 1);
    Serial.print("[Bluetooth] > ");
    Serial.println(trame);
#endif
}
