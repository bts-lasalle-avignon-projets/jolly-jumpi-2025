# Simulateur Jolly Jumpi 2025

## Présentation du protocole implanté dans le simulateur ESP'ACE

Ce document présente rapidement le fonctionnement du simulateur ainsi que le protocole implémenté. Le protocole complet est disponible dans Google Drive. Actuellement, la version du protocole est la **0.9**.

## Configuration du simulateur

Il faut définir le numéro de piste :

```cpp
#define NUMERO_PISTE 1 //!< Numéro de la piste
```

> Cela génère un nom de périphérique Bluetooth `jp-piste-x` où `x` est le `NUMERO_PISTE`

Valeur par défaut :

```cpp
#define NB_TROUS         6
```

```cpp
#define GPIO_SW1         12   //!< Pour simuler un tir
```

## Trames

- Vers le module de visualisation (Écran jumpi)

  - Envoyer le score : `$p;s\n`
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
