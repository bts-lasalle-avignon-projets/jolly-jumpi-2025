#include "joueur.h"
#include <QDebug>

Joueur::Joueur() : numero(JOUEUR_INCONNU), score(0), trouPrefere(AUCUN)
{
    qDebug() << Q_FUNC_INFO;
}

Joueur::Joueur(int numero) : numero(numero), score(0), trouPrefere(AUCUN)
{
    qDebug() << Q_FUNC_INFO << "numero" << numero;
}

Joueur::~Joueur()
{
    qDebug() << Q_FUNC_INFO;
}

int Joueur::recupererNumero() const
{
    return numero;
}

void Joueur::definirNumero(const int& numero)
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
        qDebug() << "score" << tir.recupererScore() << "temps"
                 << tir.recupererTemps();
    }
}

QList<Tir> Joueur::recupererTirs() const
{
    return tirs;
}

void Joueur::definirScore(const int scoreTir)
{
    score += scoreTir;
    qDebug() << Q_FUNC_INFO << "score" << score;
}

int Joueur::recupererScore() const
{
    return score;
}

void Joueur::determinerTrouPreferer(const QList<Tir>& listeTir)
{
    QMap<int, int> frequenceScore;

    for(const Tir& tir: listeTir)
    {
        frequenceScore[tir.recupererScore()]++;
    }

    int scoreRetenu    = 0;
    int maxOccurrences = 0;

    for(auto indexScore = frequenceScore.begin();
        indexScore != frequenceScore.end();
        indexScore++)
    {
        if(indexScore.value() > maxOccurrences)
        {
            scoreRetenu    = indexScore.key();
            maxOccurrences = indexScore.value();
        }
    }

    trouPrefere = scoreRetenu;
}

int Joueur::calculerEcartTir(const int& tempsA, const int& tempsB)
{
    return tempsB - tempsA;
}

std::vector<QList<QString> > Joueur::genererStatistiquesJoueur()
{
    std::vector<QList<QString> > statistiquesJoueur;
    int                          tempsTirPrecedent    = 0;
    int                          scoreCumuleCroissant = 0;
    QList<Tir>                   listeTir;
    int                          indexTir = 1;
    for(const Tir& tir: tirs)
    {
        listeTir << tir;
        determinerTrouPreferer(listeTir);
        scoreCumuleCroissant += tir.recupererScore();

        QList<QString> ligneStatistiquesJoueur;
        ligneStatistiquesJoueur << QString::number(indexTir);
        ligneStatistiquesJoueur << QString::number(tir.recupererScore());
        ligneStatistiquesJoueur << QString::number(scoreCumuleCroissant);
        ligneStatistiquesJoueur << QString::number(tir.recupererTemps());
        ligneStatistiquesJoueur << QString::number(
          calculerEcartTir(tempsTirPrecedent, tir.recupererTemps()));
        ligneStatistiquesJoueur << QString::number(trouPrefere);
        statistiquesJoueur.push_back(ligneStatistiquesJoueur);
        qDebug() << Q_FUNC_INFO << ligneStatistiquesJoueur;

        indexTir++;
        tempsTirPrecedent = tir.recupererTemps();
    }
    qDebug() << Q_FUNC_INFO << statistiquesJoueur;
    return statistiquesJoueur;
}

std::vector<QList<QString> > Joueur::recupererStatistiquesJoueur()
{
    return genererStatistiquesJoueur();
}
