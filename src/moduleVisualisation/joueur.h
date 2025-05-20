#ifndef JOUEUR_H
#define JOUEUR_H

#include <QList>
#include "tir.h"

#define JOUEUR_INCONNU -1
#define AUCUN          0

class Joueur
{
  public:
    Joueur();
    Joueur(int numero);
    ~Joueur();
    int        recupererNumero() const;
    void       definirNumero(const int& numero);
    void       ajouterTir(const int& score, const int& temps);
    void       afficherTirs() const;
    QList<Tir> recupererTirs() const;
    void       definirScore(const int scoreTir);
    int        recupererScore() const;
    void       determinerTrouPreferer(const QList<Tir>& listeTir);
    int        calculerEcartTir(const int& tempsA, const int& tempsB);
    std::vector<QList<QString> > genererStatistiquesJoueur();
    std::vector<QList<QString> > recupererStatistiquesJoueur();

  private:
    int        numero;
    int        score;
    int        trouPrefere;
    QList<Tir> tirs;
};

#endif
