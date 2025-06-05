#include "tir.h"

Tir::Tir() : score(0), temps(0)
{
}

Tir::Tir(int score, int temps) : score(score), temps(temps)
{
}

Tir::~Tir()
{
}

int Tir::recupererScore() const
{
    return score;
}

int Tir::recupererTemps() const
{
    return temps;
}
