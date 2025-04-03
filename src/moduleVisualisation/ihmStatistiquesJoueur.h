#ifndef IHMSTATISTIQUESJOUEUR_H
#define IHMSTATISTIQUESJOUEUR_H

#include <QtWidgets>

#define SIMULATION_CLAVIER_STATISTIQUESJOUEUR

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

#ifdef SIMULATION_CLAVIER_STATISTIQUESJOUEUR
    void simulerStatistiquesJoueur();
#endif

  protected:
    void showEvent(QShowEvent* event);

  public:
    explicit IHMStatistiquesJoueur(GestionPartie* gestionPartie,
                                   QWidget*       parent = nullptr);
    virtual ~IHMStatistiquesJoueur();

  signals:

  private slots:
    void fermer();
};

#endif // IHMPARTIE_H
