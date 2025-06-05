#ifndef IHMCLASSEMENT_H
#define IMHCLASSEMENT_H

#include <QtWidgets>

#define SIMULATION_CLAVIER_CLASSEMENT
#define NOMBRE_COLONNE_EDITABLE 2

#define INDEX_PLACE  0
#define INDEX_NUMERO 1
#define INDEX_SCORE  2

namespace Ui
{
class Classement;
}

class GestionPartie;

class IHMClassement : public QWidget
{
    Q_OBJECT

  private:
    Ui::Classement* uiClassement;
    GestionPartie*  gestionPartie; //!< association vers GestionPartie
    QStringList     nomLabels;
    enum NomLabels
    {
        INCONNU = -1,
        PLACE,
        JOUEUR,
        SCORE,
        NB_LABELS
    };

    void editerLabelChrono();
    void editerLabelPremierJoueurNomJoueur(QString numeroJoueur);
    void editerLabelPremierJoueurScore(QString score);
    void editerLabel(const QString& labelRecherche, const QString& valeur);
    void afficherClassement();
    void redimensionnerLabel();
    void appliquerMiseEnForme(QLayout* layout, int taillePolice);

#ifdef SIMULATION_CLAVIER_CLASSEMENT
    void simulerClassement();
#endif

  protected:
    void showEvent(QShowEvent* event);

  public:
    explicit IHMClassement(GestionPartie* gestionPartie,
                           QWidget*       parent = nullptr);
    virtual ~IHMClassement();

  private slots:
    void fermer();
};

#endif // IHMPARTIE_H
