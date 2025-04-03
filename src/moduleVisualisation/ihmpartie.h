#ifndef IHMPARTIE_H
#define IHMPARTIE_H

#include <QtWidgets>

#define SIMULATION_CLAVIER_PARTIE

namespace Ui
{
class Partie;
}

class GestionPartie;

class IHMPartie : public QWidget
{
    Q_OBJECT

  private:
    Ui::Partie*    uiPartie;
    GestionPartie* gestionPartie; //!< association vers GestionPartie

#ifdef SIMULATION_CLAVIER_PARTIE
    void simulerPartie();
#endif

  protected:
    void showEvent(QShowEvent* event);

  public:
    explicit IHMPartie(QWidget* parent = nullptr);
    virtual ~IHMPartie();

  signals:

  private slots:
    void fermer();
};

#endif // IHMPARTIE_H
