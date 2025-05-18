# Simulateur Jolly Jumpi 2025

## Présentation du protocole implanté dans le simulateur ESP'ACE

Ce document présente rapidement le fonctionnement du simulateur ainsi que le protocole implémenté. Le protocole complet est disponible dans Google Drive. Actuellement, la version du protocole est la **0.9**.

## Configuration du simulateur

Il faut définir le numéro de piste :

```cpp
// Configuration table Jolly Jumpi
#define PRECISION_TIR 80 //!< Précision du tir (en %)
#define NB_TROUS      6
//#define NUMERO_PISTE 1 //!< Numéro de la piste (cf. platformio.ini)

// Configuration Bluetooth
//#define BLUETOOTH_SLAVE //!< esclave (attend les connexions des clients)
#define BLUETOOTH_MASTER //!< maître (se connecte au serveur)
#ifdef BLUETOOTH_MASTER
#define USE_NAME_SERVER                                                        \
    false //!< Utiliser le nom du serveur sinon l'adresse MAC
#define ENABLE_DISCOVERY true //!< Activer la recherche de périphériques
#define RECHERCHE_ASYNCHRONE                                                   \
    true //!< Activer la recherche asynchrone sinon synchrone
#define ENABLE_SSP          false      //!< Activer le Secure Simple Pairing
#define PREFIXE_NOM_SERVEUR "sedatech" //!< Le préfixe à rechercher
#define NOM_SERVEUR         "sedatech" //!< Le nom du serveur par défaut
//#define PREFIXE_NOM_SERVEUR "jp-visu"  //!< Le préfixe à rechercher
//#define NOM_SERVEUR         "jp-visu"  //!< Le nom du serveur par défaut
#define CODE_PIN "1234" //!< Code PIN pour l'appairage
// Les timeout en ms
#define ATTENTE_CONNEXION    10000 //!< Temps d'attente de connexion au serveur
#define TEMPS_RECHERCHE      10000 //!< Temps d'attente de recherche de périphériques
#define LONGUEUR_ADRESSE_MAC 6 //!< Longueur de l'adresse MAC
// Si USE_NAME_SERVER à false, on utilise l'adresse MAC (plus rapide)
// uint8_t         adresseMACServeur[LONGUEUR_ADRESSE_MAC] = { 0x00, 0x1A, 0x7D,
// 0xDA, 0x71, 0x0A
// };
uint8_t adresseMACServeurDefaut[LONGUEUR_ADRESSE_MAC] = {
    0x3C, 0xE9, 0xF7, 0x61, 0x82, 0x20
};    // pour sedatech
#else // BLUETOOTH_SLAVE
#endif
```

> Cela génère un nom de périphérique Bluetooth `jp-piste-x` où `x` est le `NUMERO_PISTE`

Et pour simuler un tir, on utilise :

```cpp
#define GPIO_SW1         12   //!< Pour simuler un tir
```

> Le bouton SW2 permet d'effacer les périphériques appairés.

## Bluetooth

On peut choisir deux modes d'exploitation du Bluetooth :

```cpp
//#define BLUETOOTH_SLAVE //!< esclave (attendra la connexion d'un client)
#define BLUETOOTH_MASTER //!< maître (se connectera au serveur)
```

Si besoin, il est possible d'activer la découverte des périphériques Bluetooth :

```cpp
#define ENABLE_DISCOVERY     true //!< Activer la recherche de périphériques
#define RECHERCHE_ASYNCHRONE true //!< Activer la recherche asynchrone sinon synchrone
```

Pour se connecter à un périphérique on peut utiliser son nom (plus long) ou son adresse MAC (plus rapide) :

```cpp
#define USE_NAME_SERVER     false //!< Utiliser le nom du serveur sinon l'adresse MAC
```

### Appairage manuel côté PC

Le plus simple est d'appairer les "tables" ESP32 côté PC :

- on peut commencer à rendre le PC découvrable (indéfiniment) :

```sh
$ sudo bluetoothctl
[bluetooth]# power on
[bluetooth]# discoverable on
[bluetooth]# discoverable-timeout 0
[bluetooth]# advertise on
```

- puis l'appairage :

```sh
[bluetooth]# power on
[bluetooth]# agent on
[bluetooth]# pairable on
[bluetooth]# scan on
...
[bluetooth]# trust XX:XX:XX:XX:XX:XX
[bluetooth]# pair XX:XX:XX:XX:XX:XX

[bluetooth]# # paired-devices
Device XX:XX:XX:XX:XX:XX jp-piste-1
...
```

On peut vérifier la présence du service :

```sh
$ sdptool search --bdaddr local SP
Searching for SP on FF:FF:FF:00:00:00 ...
Service Name: jp-visu
Service RecHandle: 0x10011
Service Class ID List:
  "" (0x0003)
  "Serial Port" (0x1101)
Protocol Descriptor List:
  "L2CAP" (0x0100)
  "RFCOMM" (0x0003)
    Channel: 1
Profile Descriptor List:
  "Serial Port" (0x1101)
    Version: 0x0100
```

> Voir aussi `/etc/bluetooth/main.conf`

```conf
[General]
Name = jp-visu
#Class = 0x1F00
Class = 0x000100
DiscoverableTimeout = 0
PairableTimeout = 0
FastConnectable = true
```

> Voir aussi `/etc/bluetooth/input.conf`

```conf
[General]
ClassicBondedOnly=false
```

```sh
$ hciconfig noauth
```

```sh
$ sudo bluetoothctl
[bluetooth]# power on
[bluetooth]# agent NoInputNoOutput
[bluetooth]# default-agent
[bluetooth]# discoverable on
[bluetooth]# pairable on
```

## Trames

- Vers le module de visualisation (Écran jumpi)

  - Envoyer le score : `$Tp;s\n`
  > `s` (le score) et `p` (numéro de piste) sont des caractères numériques

Du module de visualisation (Écran jumpi)

- Trames de détection :

  - Demander association : `$A\n`
  - Envoyer le mode jeu : `$Cm\n`
  - Démarrer la partie : `$D\n`
  - Signaler la fin de la partie : `$F\n`

> `m` (le mode) et `p` (nombre de joueur) sont des caractères numériques

|       Mode        | Trame |
| :---------------: | :---: |
|     Standard      | $C1\n |
| Couleur aléatoire | $C2\n |
|      Chrono       | $C3\n |

## platform.ini

```ini
[env:lolin32]
platform = espressif32
board = lolin32
framework = arduino
lib_deps =
  thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays @ ^4.2.0
upload_port = /dev/ttyUSB0
upload_speed = 115200
monitor_port = /dev/ttyUSB0
monitor_speed = 115200
```

## Auteur

- Thierry Vaira <<tvaira@free.fr>>
