# jolly-jumpi-2025


- [Projet : Jolly-Jumpi](#jolly-jumpi-2025)
  - [Présentation](#présentation)
  - [Fonctionnalités](#fonctionnalités)
  - [Diaporama de présentation](#diaporama-de-présentation)
  - [Diagramme de séquence](#diagramme-de-séquence)
  - [Diagramme de classes](#diagramme-de-classes)
  - [Protocole de communication](#Protocole-de-communication)
  - [Visuels](#visuels)
  - [Changelog](#changelog)
  - [TODO](#todo)
  - [Planification des versions](#planification-des-versions)
  - [Défauts constatés non corrigés](#défauts-constatés-non-corrigés)
  - [Équipe de développement](#équipe-de-développement)

---


## Présentation

Jolly Jumpi est une copie du "Derby Forain" ou "Tiercé Forain", le but : Être le premier à franchir la ligne d'arrivée en marquant des points.

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

- Une télévision pour afficher le déroulement de la course et le classement/statistiques de la partie
- Une tablette afin de configurer la partie
- Un module de détection de balle

Jolly Jumpi est développé en **C++**, **QT**, **sql**.

![](./images/Readme/compositionSysteme.webp)

## fonctionnalités

![](./images/Readme/DiagrammeCasUtilisation-moduleConfiguration.webp)
![](./images/Readme/DiagrammeCasUtilisation-moduleVisualisation.webp)

## Diaporama de présentation


## Diagramme des échanges

![](./images/Readme/DiagrammeDesEchanges-Configuration.webp)
![](./images/Readme/DiagrammeDesEchanges-GestionPartie.webp)
![](./images/Readme/DiagrammeDesEchanges-AfficherStat.webp)
![](./images/Readme/DiagrammeDesEchanges-InterromprePartie.webp)


## Diagramme de séquence

![](./images/Readme/DiagrammeDeSequence-GestionPartie.webp)
![](./images/Readme/DiagrammeDeSequence-Statistiques.webp)
![](./images/Readme/DiagrammeDeSequence-InterromprePartie.webp)

## Diagramme de classes

![](./images/Readme/DiagrammeDeClasse-moduleConfiguration.webp)
![](./images/Readme/DiagrammeDeClasse-moduleVisualisation.webp)

## Protocole de communication

![](./images/Readme/ProtocolCommunication.webp)

## Visuels



## Changelog



## TODO

- v0.1 :

  - Les bases

    > - [X] Créer des différents diagrammes
    > - [X] Définir le protocol de communication

  - Créer le squelette du projet

    > - [X] Créer de l'arborescence commune

  - Module de configuration : Créer la base



  - Module de visualisation : Créer les classes et attributs

    > - [X] Créer les classes
    > - [X] Créer les attributs

  - Module de visualisation : Créer les affichages de base

    > - [ ] Créer la page Accueil
    > - [ ] Créer la page Connexion
    > - [ ] Créer la page Jeu
    > - [ ] Créer la page Classement
    > - [ ] Créer la page Statistiques:Joueur
  
  - Gérer les communications

    > - [ ] Recupérer une trame
    > - [ ] Transcrire la trame
    > - [ ] Est une trame du module de détection de balle
    > - [ ] Est une trame d'appairage
    > - [ ] Est une trame de score
    > - [ ] Est une trame du module de configuration
    > - [ ] Est une trame demandant la page des statistiques globales
    > - [ ] Est une trame demandant la page Accueil
    > - [ ] Est une trame marquant l'arret du jeu
    > - [ ] Composer une trame à envoyer
    > - [ ] Envoyer une trame

- v0.2 :

  - Module de visualisation : 

    > - [ ] Ajouter des joueurs (1 à 8)
    > - [ ] Ajouter un chonomètre
    > - [ ] Ajouter un minuteur (pour le classement et les statistiques joueur)
    > - [ ] Changer de page (reçois en argument le nom de la page)
    > - [ ] Afficher la page Accueil
    > - [ ] Afficher la page Connexion
    > - [ ] Afficher la page Jeu
    > - [ ] Afficher la page Classement à la fin d'une partie (10 seconde)
    > - [ ] Afficher la page Statistiques:Joueur après la page Classement (10sec par joueur)
    > - [ ] Démarrer/Communiquer la configuration de la partie au module de détection
    > - [ ] Calculer le score d'un joueur
    > - [ ] Enregistrer les données de la partie en local
    > - [ ] Enregistrer les données de la partie dans une base de données
    > - [ ] Mettre fin à la partie
    > - [ ] Abandonner la partie
    > - [ ] Envoyer la fin de partie aux autres modules

- v0.3 :

  - Module de visualisation : 

    > - [ ] Créer la page Statistiques:Globales
    > - [ ] Afficher la page Statistiques:Globales
    > - [ ] .......... (à poursuivre une fois la v0.2 terminée).

## Planification des versions

> Les versions sont numérotées de la manière suivante : `vX.YZ`
>
> - X = Mise à jour majeure
> - Y = nouvelle fonctionnalités
> - Z = Correction de bug (lettre)

- Version 0.1 : Structure de base

- Version 0.2 : Jeu de base (communication simulé)

- Version 0.3 : Ajout d'un historique des parties jouées

- Version 0.4 : Ajout des différents modes de jeu

- Version 0.5 : Modification de l'interface graphique

- Version 1.0 : Ajout du bluetooth

## Défauts constatés non corrigés



## Équipe de développement

**Etudiants IR :**
- @aariati : axel.ariati.pro@gmail.com
- @npessina1 : pessina.nicolas.pro@gmail.com

---

&copy; 2024-2025 LaSalle Avignon