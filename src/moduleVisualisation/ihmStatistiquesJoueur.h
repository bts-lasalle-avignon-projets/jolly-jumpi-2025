#ifndef IHMSTATISTIQUESJOUEUR_H
#define IHMSTATISTIQUESJOUEUR_H

#include <QtWidgets>

#define SIMULATION_CLAVIER_STATISTIQUESJOUEUR
#define NOMBRE_COLONNE_STATISTIQUES 6

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
    QStringList             nomLabelsEntete;
    QStringList             nomLabels;
    enum NomLabelsEntete
    {
        JOUEUR = 0,
        SCORE,
        PLACE,
        NB_LABELS_ENTETE
    };
    enum NomLabels
    {
        NUMERO_TIR = 0,
        SCORE_TIR,
        SCORE_CUMULE,
        TEMPS_TIR,
        ECART_TIR,
        TROU_PREFERE,
        NB_LABELS
    };

    void editerLabelEntete(const QString numero, int indexLabelEntete);
    void editerLabel(const QString& labelRecherche, const QString& valeur);
    void afficherStatistiques(const QString numero);
    void deroulerStatistiques();
    void appliquerMiseEnForme(QLayout* layout, int taillePolice);
    void redimensionnerLabel();

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
