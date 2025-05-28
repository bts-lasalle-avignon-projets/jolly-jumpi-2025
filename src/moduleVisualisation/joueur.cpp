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
    for(const Tir& tir: tirs)
    {
        qDebug() << Q_FUNC_INFO << "numero" << numero << "score"
                 << tir.recupererScore() << "temps" << tir.recupererTemps();
    }
}

QList<Tir> Joueur::recupererTirs() const
{
    return tirs;
}

void Joueur::definirScore(const int scoreTir)
{
    score += scoreTir;
    qDebug() << Q_FUNC_INFO << "numero" << numero << "score" << score;
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
    qDebug() << Q_FUNC_INFO;
    std::vector<QList<QString> > statistiquesJoueur;
    int                          tempsTirPrecedent    = 0;
    int                          scoreCumuleCroissant = 0;
    QList<Tir>                   listeTir;
    int                          indexTir = 1;

    if(tirs.isEmpty())
    {
        QList<QString> ligneStatistiquesJoueur;
        ligneStatistiquesJoueur << "0"
                                << "0"
                                << "0"
                                << "0"
                                << "0"
                                << "0";
        statistiquesJoueur.push_back(ligneStatistiquesJoueur);
        qDebug() << Q_FUNC_INFO << "0 tir]" << ligneStatistiquesJoueur;
        return statistiquesJoueur;
    }

    for(const Tir& tir: tirs)
    {
        listeTir << tir;
        determinerTrouPreferer(listeTir);
        scoreCumuleCroissant += tir.recupererScore();
        int ecartTir =
          calculerEcartTir(tempsTirPrecedent, tir.recupererTemps());

        QList<QString> ligneStatistiquesJoueur;
        ligneStatistiquesJoueur << QString::number(indexTir);
        ligneStatistiquesJoueur << QString::number(tir.recupererScore());
        ligneStatistiquesJoueur << QString::number(scoreCumuleCroissant);
        ligneStatistiquesJoueur << convertirTemps(tir.recupererTemps());
        ligneStatistiquesJoueur << convertirTemps(ecartTir);
        ligneStatistiquesJoueur << QString::number(trouPrefere);
        statistiquesJoueur.push_back(ligneStatistiquesJoueur);
        qDebug() << Q_FUNC_INFO << ligneStatistiquesJoueur;

        indexTir++;
        tempsTirPrecedent = tir.recupererTemps();
    }
    return statistiquesJoueur;
}

std::vector<QList<QString> > Joueur::recupererStatistiquesJoueur()
{
    return genererStatistiquesJoueur();
}

QString Joueur::convertirTemps(const int& secondes)
{
    QString temps;
    QString minute  = QString::number(secondes / 60);
    QString seconde = QString::number(secondes % 60);
    if(seconde.toInt() < 10)
        temps = minute + ":0" + seconde;
    else
        temps = minute + ":" + seconde;
    qDebug() << Q_FUNC_INFO << "temps" << temps;
    return temps;
}

int Joueur::definirPlace(const int& placeClassement)
{
    place = placeClassement;
}

int Joueur::recupererPlace() const
{
    return place;
}
