#include "joueur.h"
#include <QDebug>

Joueur::Joueur() : numero(JOUEUR_INCONNU), score(0), trouPrefere(AUCUN)
{
}

Joueur::~Joueur()
{
}

void Joueur::ajouterTir(const int& score, const int& temps)
{
    qDebug() << Q_FUNC_INFO << "Tir" << score << ":" << temps << "(s)";
    tirs.append(Tir(score, temps));
}

void Joueur::afficherTirs() const
{
    for(const Tir& tir: tirs)
    {
        qDebug() << "Score:" << tir.getScore()
                 << "Temps (s):" << tir.getTemps();
    }
}
