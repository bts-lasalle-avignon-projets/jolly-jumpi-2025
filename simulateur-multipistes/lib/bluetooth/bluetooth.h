#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "../../include/jolly-jumpi.h"
#include <Arduino.h>
#include <BluetoothSerial.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include <esp_spp_api.h>
#include <esp_log.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled
#endif

#define ATTENTE_PRET (10 * 1000)
#define BUFFER_TRAME 64 //!< Buffer pour les trames

bool initialiserBluetooth(String nomBluetooth, const char* pin, bool enableSSP);
void trouverPeripherique(BTAdvertisedDevice* peripherique);
void demanderConfirmation(uint32_t numVal);
void finaliserAuthentification(boolean succes);
void gererEvenementBluetooth(esp_spp_cb_event_t  event,
                             esp_spp_cb_param_t* param);
void demarrerRecherchePeripheriques(uint32_t tempsRecherche);
void demarrerRecherchePeripheriques();
void arreterRecherchePeripheriques();
bool connecter(uint8_t adresseDistante[]);
bool connecter(String nomDistant);
bool reconnecter();

void afficherPeripheriquesAppaires();
int  getNbPeripheriquesAppaires();
bool estPeripheriqueAppaire(uint8_t* adressePeripherique);
int getPeripheriquesAppaires(int nb, esp_bd_addr_t* listePeripheriquesAppaires);

bool supprimerAppairagePeripherique(uint8_t* adressePeripherique);
void supprimerPeripheriquesAppaires();
// void getNomPeripherique(uint8_t adresseDistante[]);

bool      lireTrame(String& trame);
TypeTrame verifierTrame(String& trame);
String    extraireChamp(const String& trame, unsigned int numeroChamp);
void      envoyerTrameTir(int numeroTable, int score);
void      envoyerTrameAssociation(int nbPistes = NB_PISTES);

#endif // BLUETOOTH_H
