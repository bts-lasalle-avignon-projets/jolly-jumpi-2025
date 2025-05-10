#include "bluetooth.h"

extern BluetoothSerial ESPBluetooth;
extern String          prefixeNomServeur;
extern String          nomServeur;
extern uint8_t         adresseMACServeur[ESP_BD_ADDR_LEN];
extern bool            serveurTrouve;
extern bool            etatConnexion;
extern bool            appairageReussi;
extern bool            demandeConfirmationAppairage;

void trouverPeripherique(BTAdvertisedDevice* peripherique)
{
#ifdef DEBUG
    Serial.println("[Bluetooth] Peripherique decouvert : " +
                   String(peripherique->getName().c_str()) + " [" +
                   String(peripherique->getAddress().toString().c_str()) +
                   "] " + String(peripherique->getRSSI()) + " dBm");
#endif
    if(peripherique->haveName() &&
       prefixeNomServeur.startsWith(String(peripherique->getName().c_str())))
    {
        serveurTrouve = true;
        nomServeur    = String(peripherique->getName().c_str());
        mempcpy(adresseMACServeur,
                peripherique->getAddress().getNative(),
                ESP_BD_ADDR_LEN);
#ifdef DEBUG
        Serial.println("[Bluetooth] Serveur decouvert !");
#endif
    }
}

void demanderConfirmation(uint32_t numVal)
{
    demandeConfirmationAppairage = true;
#ifdef DEBUG
    Serial.print("[Bluetooth] Demande de confirmation : ");
    Serial.println(numVal);
#endif
}

void finaliserAuthentification(boolean succes)
{
    demandeConfirmationAppairage = false;

    if(succes)
    {
        appairageReussi = true;
#ifdef DEBUG
        Serial.println("[Bluetooth] Appairage reussi !");
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
    if(event == ESP_SPP_SRV_OPEN_EVT)
    {
        etatConnexion = true;
#ifdef DEBUG
        Serial.println("[Bluetooth] Etat connecte");
#endif
    }
    else if(event == ESP_SPP_CLOSE_EVT)
    {
        etatConnexion = false;
#ifdef DEBUG
        Serial.println("[Bluetooth] Etat deconnecte");
#endif
    }
    else if(event == ESP_SPP_DATA_IND_EVT)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Reception");
#endif
    }
}

void demarrerRecherchePeripheriques(uint32_t tempsRecherche)
{
#ifdef DEBUG
    Serial.println("[Bluetooth] Recherche de peripheriques");
#endif
    BTScanResults* resultatsRecherche = ESPBluetooth.discover(tempsRecherche);
#ifdef DEBUG
    Serial.println("[Bluetooth] Recherche de peripheriques terminee");
    Serial.println("[Bluetooth] Nombre de peripheriques decouverts : " +
                   String(resultatsRecherche->getCount()));
    for(int i = 0; i < resultatsRecherche->getCount(); i++)
    {
        BTAdvertisedDevice* peripherique = resultatsRecherche->getDevice(i);
        Serial.println("[Bluetooth] Peripherique decouvert : " +
                       String(peripherique->getName().c_str()) + " [" +
                       String(peripherique->getAddress().toString().c_str()) +
                       "] " + String(peripherique->getRSSI()) + " dBm");
        if(peripherique->haveName() && prefixeNomServeur.startsWith(String(
                                         peripherique->getName().c_str())))
        {
            serveurTrouve = true;
            nomServeur    = String(peripherique->getName().c_str());
            mempcpy(adresseMACServeur,
                    peripherique->getAddress().getNative(),
                    ESP_BD_ADDR_LEN);
#ifdef DEBUG
            Serial.println("[Bluetooth] Serveur decouvert !");
#endif
        }
    }
#endif
}

void demarrerRecherchePeripheriques()
{
#ifdef DEBUG
    Serial.println("[Bluetooth] Recherche de peripheriques");
#endif
    ESPBluetooth.discoverAsync(trouverPeripherique);
}

void arreterRecherchePeripheriques()
{
#ifdef DEBUG
    Serial.println("[Bluetooth] Arret de la recherche de peripheriques");
#endif
    ESPBluetooth.discoverAsyncStop();
}

bool connecter(uint8_t adresseDistante[], const char* pin, bool enableSSP)
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
    Serial.println("[Bluetooth] Connexion vers le serveur " + String(str));
#endif
    if(enableSSP)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Activer SSP (Secure Simple Pairing)");
#endif
        ESPBluetooth.enableSSP();
        if(pin && *pin)
        {
#ifdef DEBUG
            Serial.println("[Bluetooth] Code PIN : " + String(pin));
#endif
            ESPBluetooth.setPin(pin);
        }
        ESPBluetooth.onConfirmRequest(demanderConfirmation);
        ESPBluetooth.onAuthComplete(finaliserAuthentification);
    }

    ESPBluetooth.register_callback(gererEvenementBluetooth);
    bool estConnecte =
      ESPBluetooth.connect(adresseDistante, 0, ESP_SPP_SEC_NONE);
    if(estConnecte)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Connexion reussie !");
#endif
    }
    else
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Echec connexion !");
#endif
        ESPBluetooth.confirmReply(true);
        reconnecter();
    }

    return estConnecte;
}

bool connecter(String nomDistant, const char* pin, bool enableSSP)
{
    if(!ESPBluetooth.isReady(true, ATTENTE_PRET))
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Non initialisé ou mode master non actif !");
#endif
        return false;
    }
    if(enableSSP)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Activer SSP (Secure Simple Pairing)");
#endif
        ESPBluetooth.enableSSP();
        if(pin && *pin)
        {
#ifdef DEBUG
            Serial.println("[Bluetooth] Code PIN : " + String(pin));
#endif
            ESPBluetooth.setPin(pin);
        }
        ESPBluetooth.onConfirmRequest(demanderConfirmation);
        ESPBluetooth.onAuthComplete(finaliserAuthentification);
    }

    ESPBluetooth.register_callback(gererEvenementBluetooth);
#ifdef DEBUG
    Serial.println("[Bluetooth] Connexion vers le serveur " + nomDistant);
#endif
    bool estConnecte = ESPBluetooth.connect(nomDistant);
    if(estConnecte)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Connexion reussie !");
#endif
    }
    else
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Echec connexion !");
#endif
        ESPBluetooth.confirmReply(true);
        reconnecter();
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
        Serial.println("[Bluetooth] Aucun peripherique appaire !");
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
        Serial.println("[Bluetooth] Aucun peripherique appaire !");
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
        Serial.print("[Bluetooth] Peripherique appaire : ");
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
        /*if(listePeripheriquesAppaires[i][0] == adressePeripherique[0] &&
           listePeripheriquesAppaires[i][1] == adressePeripherique[1] &&
           listePeripheriquesAppaires[i][2] == adressePeripherique[2] &&
           listePeripheriquesAppaires[i][3] == adressePeripherique[3] &&
           listePeripheriquesAppaires[i][4] == adressePeripherique[4] &&
           listePeripheriquesAppaires[i][5] == adressePeripherique[5])*/
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
            Serial.println("[Bluetooth] Peripherique " + String(str) +
                           " appaire");
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
    Serial.println("[Bluetooth] Peripherique " + String(str) + " non appaire");
#endif
    free(listePeripheriquesAppaires);
    return false;
}

int getPeripheriquesAppaires(uint nb, esp_bd_addr_t* listePeripheriquesAppaires)
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
        Serial.println("[Bluetooth] Aucun peripherique appaire !");
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
        Serial.println("[Bluetooth] Aucun peripherique appaire !");
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
        Serial.print("[Bluetooth] Suppression peripherique appaire [" +
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
