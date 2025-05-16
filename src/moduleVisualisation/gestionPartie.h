#ifndef GESTION_PARTIE_H
#define GESTION_PARTIE_H

#include <QObject>
#include <QVector>

class Joueur;
class Communication;

namespace Ui
{
class GestionPartie;
}

class GestionPartie : public QObject
{
    Q_OBJECT
  public:
    GestionPartie(QObject* parent);
    ~GestionPartie();
    void commencerPartie();

  private:
    int                nombreJoueurs;
    int                modeDeJeu;
    int                etat;
    double             duree;
    QVector<Joueur*>   joueurs;
    Ui::GestionPartie* ui;
    Communication*     communication;
    bool               configurationPisteTerminee;

    void gererConfiguration(QString nombreJoueursRecu, QString modeDeJeuRecu);
    void creerJoueur();
    void initialiserJoueur();
    void configurerPiste(const QString& numeroPiste);
};

#endif
