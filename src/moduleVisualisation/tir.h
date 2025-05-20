#ifndef TIR_H
#define TIR_H

class Tir
{
  public:
    Tir();
    Tir(int score, int temps);
    ~Tir();
    int recupererScore() const;
    int recupererTemps() const;

  private:
    int score;
    int temps;
};

#endif
