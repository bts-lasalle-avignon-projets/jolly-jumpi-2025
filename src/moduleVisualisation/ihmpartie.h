#ifndef IHMPARTIE_H
#define IHMPARTIE_H

#include <QtWidgets>

#define SIMULATION_CLAVIER_PARTIE
#define RPI

namespace Ui
{
class Partie;
}

class IHMClassement;
class IHMStatistiquesJoueur;
class Communication;
class GestionPartie;

class IHMPartie : public QWidget
{
    Q_OBJECT

  private:
    Ui::Partie*            uiPartie;
    IHMClassement*         ihmClassement;
    IHMStatistiquesJoueur* ihmStatistiquesJoueur;
    Communication*         communication; //!< association vers Communication
    GestionPartie*         gestionPartie; //!< composition vers GestionPartie

    void creerLigneCourse(int nombreJoueurs);
    void initialiserEmplacementLabel();
    void deplacerLigneArrivee();
    void creerLigneArrivee(QString ligne);

#ifdef SIMULATION_CLAVIER_PARTIE
    void simulerPartie();
#endif

  protected:
    void showEvent(QShowEvent* event);

  public:
    explicit IHMPartie(Communication* communication, QWidget* parent = nullptr);
    virtual ~IHMPartie();

  signals:
    void etatPartieModifie(int etatPartie);

  private slots:
    void fermer();
    void afficherClassement();
    void afficherStatistiquesJoueur();
    void mettreAJoursCourse(QString numero, int scoreTir);
    void mettreAJoursChronometre(int secondes);
};

#endif // IHMPARTIE_H
