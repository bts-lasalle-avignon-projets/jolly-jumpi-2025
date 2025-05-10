#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include <esp_spp_api.h>
#include <esp_log.h>

#define ATTENTE_PRET (10 * 1000)

void trouverPeripherique(BTAdvertisedDevice* peripherique);
void demanderConfirmation(uint32_t numVal);
void finaliserAuthentification(boolean succes);
void gererEvenementBluetooth(esp_spp_cb_event_t  event,
                             esp_spp_cb_param_t* param);
void demarrerRecherchePeripheriques(uint32_t tempsRecherche);
void demarrerRecherchePeripheriques();
void arreterRecherchePeripheriques();
bool connecter(uint8_t     adresseDistante[],
               const char* pin       = nullptr,
               bool        enableSSP = false);
bool connecter(String nomDistant, const char* pin, bool enableSSP);
bool reconnecter();

void afficherPeripheriquesAppaires();
int  getNbPeripheriquesAppaires();
bool estPeripheriqueAppaire(uint8_t* adressePeripherique);
int  getPeripheriquesAppaires(uint           nb,
                              esp_bd_addr_t* listePeripheriquesAppaires);

bool supprimerAppairagePeripherique(uint8_t* adressePeripherique);
void supprimerPeripheriquesAppaires();
// void getNomPeripherique(uint8_t adresseDistante[]);

#endif // BLUETOOTH_H
