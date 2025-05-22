#ifndef IHMSTATISTIQUESJOUEUR_H
#define IHMSTATISTIQUESJOUEUR_H

#include <QtWidgets>

#define SIMULATION_CLAVIER_STATISTIQUESJOUEUR
#define NOMBRE_COLONNE_STATISTIQUES 6

#define TEMPS_AFFICHAGE_JOUEUR  10 // en secondes
#define NOMBRE_MAX_TIR_POSSIBLE 10

namespace Ui
{
class StatistiquesJoueur;
}

class GestionPartie;

class IHMStatistiquesJoueur : public QWidget
{
    Q_OBJECT

  private:
    Ui::StatistiquesJoueur* uiStatistiquesJoueur;
    GestionPartie*          gestionPartie; //!< association vers GestionPartie
    int                     indexJoueurStatistiques;
    void                    editerLabelNomJoueur(QString numeroJoueur);
    void                    editerLabelScore(QString scoreJoueur);
    void                    editerLabelPlace(QString placeJoueur);
    void editerLabelNumeroTir(QString ligne, QString numeroTir);
    void editerLabelScoreTir(QString ligne, QString scoreTir);
    void editerLabelScoreCumule(QString ligne, QString scoreCumule);
    void editerLabelTempsTir(QString ligne, QString tempsTir);
    void editerLabelEcartTir(QString ligne, QString tempsEntreTir);
    void editerLabelTrouPrefere(QString ligne, QString tourPrefere);
    void afficherStatistiques(const QString numero);
    void deroulerStatistiques();
    void reitialiserLabel();

#ifdef SIMULATION_CLAVIER_STATISTIQUESJOUEUR
    void simulerStatistiquesJoueur();
#endif

  protected:
    void showEvent(QShowEvent* event);

  public:
    explicit IHMStatistiquesJoueur(GestionPartie* gestionPartie,
                                   QWidget*       parent = nullptr);
    virtual ~IHMStatistiquesJoueur();

  private slots:
    void fermer();
    void deroulerStatistiquesSuivant();
};

#endif // IHMPARTIE_H
