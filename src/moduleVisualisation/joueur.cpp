#include "joueur.h"
#include <QDebug>

Joueur::Joueur() : numero(JOUEUR_INCONNU), score(0), trouPrefere(AUCUN)
{
    qDebug() << Q_FUNC_INFO;
}

Joueur::~Joueur()
{
    qDebug() << Q_FUNC_INFO;
}

int Joueur::getNumero() const
{
    return numero;
}

void Joueur::setNumero(const int& numero)
{
    this->numero = numero;
}

void Joueur::ajouterTir(const int& score, const int& temps)
{
    qDebug() << Q_FUNC_INFO << "numero" << numero << "score" << score << "temps"
             << temps;
    tirs.append(Tir(score, temps));
}

void Joueur::afficherTirs() const
{
    qDebug() << Q_FUNC_INFO << "numero" << numero;
    for(const Tir& tir: tirs)
    {
        qDebug() << "score" << tir.getScore() << "temps" << tir.getTemps();
    }
}
