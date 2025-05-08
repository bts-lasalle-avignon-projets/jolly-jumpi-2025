#include "bluetooth.h"

extern BluetoothSerial ESPBluetooth;
extern String          prefixeNomServeur;
extern String          nomServeur;
extern bool            serveurTrouve;
extern uint8_t         adresseMACServeur[ESP_BD_ADDR_LEN];
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
#ifdef DEBUG
    if(succes)
    {
        Serial.println("[Bluetooth] Appairage reussi !");
    }
    else
    {
        Serial.println("[Bluetooth] Echec apparaige !");
    }
#endif
}

void gererEvenementBluetooth(esp_spp_cb_event_t  event,
                             esp_spp_cb_param_t* param)
{
    if(event == ESP_SPP_SRV_OPEN_EVT)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Connexion");
#endif
    }
    else if(event == ESP_SPP_CLOSE_EVT)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Deconnexion");
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
        if(pin)
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
#ifdef DEBUG
    Serial.println("[Bluetooth] Connexion vers le serveur " + nomDistant);
#endif
    if(enableSSP)
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Activer SSP (Secure Simple Pairing)");
#endif
        ESPBluetooth.enableSSP();
        if(pin)
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
    // disconnect() may take upto 10 secs max
    if(ESPBluetooth.disconnect())
    {
#ifdef DEBUG
        Serial.println("[Bluetooth] Déconnexion réussiée !");
#endif
    }
    // this would reconnect to the name (will use address, if resolved) or
    // address used with connect(name/address).
    return ESPBluetooth.connect();
}