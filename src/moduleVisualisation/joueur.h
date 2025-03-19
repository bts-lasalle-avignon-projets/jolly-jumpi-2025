#ifndef JOUEUR_H
#define JOUEUR_H

class Joueur
{
  public:
    int numero, score, trouPrefere;
    Tir tableauDesTirs[];
};

Joueur joueur;

class Tir
{
  public:
    int score, temps;
};

Tir tir;

#endif