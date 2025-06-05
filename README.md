![Qt Build ModuleVisualisation](https://github.com/bts-lasalle-avignon-projets/jolly-jumpi-2025/actions/workflows/qt-build-moduleVisualisation.yml/badge.svg) [![release](https://img.shields.io/github/v/release/bts-lasalle-avignon-projets/jolly-jumpi-2025)](https://github.com/bts-lasalle-avignon-projets/jolly-jumpi-2025/releases)

# Projet BTS CIEL 2025 : Jolly-Jumpi

- [Projet BTS CIEL 2025 : Jolly-Jumpi](#projet-bts-ciel-2025--jolly-jumpi)
  - [Présentation](#présentation)
  - [Fonctionnalités](#fonctionnalités)
  - [Scénarios](#scénarios)
  - [Diagramme de classes](#diagramme-de-classes)
  - [Protocole de communication](#protocole-de-communication)
  - [Gestion de projet](#gestion-de-projet)
  - [Équipe de développement](#équipe-de-développement)

---

## Présentation

Jolly Jumpi est un système inspiré du "Derby Forain" ou "Tiercé Forain" dont le but est d'être le premier à franchir la ligne d'arrivée en marquant des points.

Le jeu se compose de :

- 1 à 8 joueurs
- 1 piste par joueur
- 6 trous par piste (trois de 1, deux de 3 et un de 5)
- Des balles pour chaque joueur
- Plusieurs modes de jeu

Les règles sont les suivantes :

- Tirer les balles dans les trous
- Rester courtois avec les joueurs adverses

Le matériel est :

- Une télévision pour afficher le déroulement de la course et le classement (et des statistiques) de la partie
- Une tablette afin de configurer la partie
- Un module de détection de balle

Jolly Jumpi est développé en **C++** avec le _framework_ **Qt**.

![](./images/Readme/compositionSysteme.webp)

## Fonctionnalités

- Le module de configuration :

![](./images/Readme/DiagrammeCasUtilisation-moduleConfiguration.webp)

- Le module de visualisation :

![](./images/Readme/DiagrammeCasUtilisation-moduleVisualisation.webp)

![](./images/Readme/jollyJumPi_moduleVisualisation_tele.gif)

## Scénarios

- Configurer et gérer une partie

![](./images/Readme/DiagrammeDeSequence-GestionPartie.webp)

- Visualiser l'historique

![](./images/Readme/DiagrammeDeSequence-Statistiques.webp)

- Interrompre une partie

![](./images/Readme/diagrammeSequenceAbandon.png)

## Diagramme de classes

- Module de configuration :

![](./images/Readme/DiagrammeDeClasse-moduleConfiguration.webp)

- Module de visualisation

![](./images/Readme/DiagrammeDeClasse-moduleVisualisation.png)

## Protocole de communication

![](./images/Readme/ProtocolCommunication.webp)

Quelques diagrammes des échanges :

![](./images/Readme/DiagrammeDesEchangesV2-Configuration.png)

![](./images/Readme/DiagrammeDesEchangesV2-GestionPartie.png)

![](./images/Readme/DiagrammeDesEchangesV2-AbandonPartie.png)

![](./images/Readme/DiagrammeDesEchangesV2-AfficherStats.png)

## Gestion de projet

[GitHub Project](https://github.com/orgs/bts-lasalle-avignon-projets/projects/24)

- itération 1 : Structure de base

  - Les bases

    > - [x] Créer des différents diagrammes
    > - [X] Définir le protocole de communication

  - Créer le squelette du projet

    > - [X] Créer de l'arborescence commune

  - Module de visualisation : Créer les classes et attributs

    > - [X] Créer les classes
    > - [X] Créer les attributs

  - Module de visualisation : Créer les affichages de base

    > - [X] Créer la page Accueil
    > - [X] Créer la page Connexion
    > - [X] Créer la page Jeu
    > - [X] Créer la page Classement
    > - [X] Créer la page Statistiques:Joueur

  - Module de visualisation : Gérer les communications

    > - [X] Recupérer un message
    > - [X] Traiter le message
    > - [X] Formater un message à envoyer
    > - [X] Envoyer un message

- itération 2 : Jeu de base (en mode simulation)

  - Module de visualisation : Gérer partie

    - Configurer partie

      > - [X] Ajouter des joueurs (1 à 8)
      > - [X] Démarrer/Communiquer la configuration de la partie au module de détection

    - Créer chronomètre

      > - [X] Créer un chronomètre

    - Mettre à jour l'affichage

      > - [X] Changer de page
      > - [X] Afficher la page Accueil
      > - [X] ~~Afficher la page Connexion~~ Afficher info état connexion
      > - [X] Afficher la page Jeu
      > - [X] Mettre à jour la page Jeu
      > - [X] Afficher la page Classement à la fin d'une partie (10 seconde)
      > - [X] Afficher la page Statistiques:Joueur après la page Classement (10sec par joueur)

    - Gérer événement partie

      > - [X] Calculer le score d'un joueur
      > - [X] Enregistrer les données de la partie en local
      > - [X] Abandonner la partie
      > - [X] Mettre fin à la partie
      > - [X] Envoyer la fin de partie aux autres modules

- itération 3 : Ajout d'un historique des parties jouées

  - Module de visualisation :

    > - [ ] Créer la page Statistiques:Globales
    > - [ ] Afficher la page Statistiques:Globales

- Version 1.0 : Modification de l'interface graphique

## Équipe de développement

**Etudiants IR :**

- Le module de configuration : [@aariati](https://github.com/aariati) <<axel.ariati.pro@gmail.com>>
- Le module de visualisation : [@npessina1](https://github.com/npessina1) <<pessina.nicolas.pro@gmail.com>>

---
&copy; 2024-2025 LaSalle Avignon
