#ifndef JOLLY_JUMPI_H
#define JOLLY_JUMPI_H

#include <Arduino.h>

// Configuration table Jolly Jumpi
#define NB_PISTES     8  //!< Nombre de pistes
#define PRECISION_TIR 80 //!< Précision du tir (en %)
#define NB_TROUS      6
#define TIR_RAPIDE    2000 //!< Tir réussi rapide (en ms)

// Protocole (cf. Google Drive)
#define EN_TETE_TRAME    "$"
#define DELIMITEUR_CHAMP ";"
#define DELIMITEURS_FIN  "\n"
#define DELIMITEUR_DATAS ';'
#define DELIMITEUR_FIN   '\n'

// Divers
#define LG_STR 32 //!< Longueur des messages texte (pour affichage)

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

const int pointsTrou[NB_TROUS] = {
    1, 1, 1, 3, 3, 5
}; //!< nombre de points affectés à chaque trou dans le mode de jeu standard

#endif