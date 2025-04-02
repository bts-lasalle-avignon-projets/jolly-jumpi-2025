/**
 * @file ihm.h
 *
 * @brief Déclaration de la classe IHM
 * @author Nicolas Pessina
 * @version 1.0
 */

#ifndef IHM_H
#define IHM_H

#include <QtWidgets>

/**
 * @def NOM_MODULE
 * @brief Le nom du module de visualisation
 */
#define NOM_MODULE "jolly-jumpi"

/**
 * @def VERSION_MODULE
 * @brief La version du module de visualisation
 */

#define VERSION_MODULE "1.0"

namespace Ui
{
class Accueil;
class Partie;
class Classement;
class StatistiquesJoueur;
}

class GestionPartie;

/**
 * @class IHM
 * @brief Déclaration de la classe IHM
 * @details Cette classe gère l'interface graphique de l'application
 */
class IHM : public QWidget
{
    Q_OBJECT

  private:
    GestionPartie*          gestionPartie; //!< association vers GestionPartie
    Ui::Accueil*            uiAccueil;
    Ui::Partie*             uiPartie;
    Ui::Classement*         uiClassement;
    Ui::StatistiquesJoueur* uiStatistiquesJoueur;

  public:
    IHM(QWidget* parent = nullptr);
    ~IHM();
};

#endif // IHM_H
