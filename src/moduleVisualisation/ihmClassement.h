#ifndef IHMCLASSEMENT_H
#define IMHCLASSEMENT_H

#include <QtWidgets>

#define SIMULATION_CLAVIER_CLASSEMENT

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

#ifdef SIMULATION_CLAVIER_CLASSEMENT
    void simulerClassement();
#endif

  protected:
    void showEvent(QShowEvent* event);

  public:
    explicit IHMClassement(GestionPartie* gestionPartie,
                           QWidget*       parent = nullptr);
    virtual ~IHMClassement();

  signals:

  private slots:
    void fermer();
};

#endif // IHMPARTIE_H
