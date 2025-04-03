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

#define SIMULATION_CLAVIER_ACCUEIL

#define PERIODE_DEFILEMENT 3000

namespace Ui
{
class Accueil;
}

class IHMPartie;

/**
 * @class IHM
 * @brief Déclaration de la classe IHM
 * @details Cette classe gère l'interface graphique de l'application
 */
class IHM : public QWidget
{
    Q_OBJECT

  private:
    Ui::Accueil* uiAccueil;
    IHMPartie*   ihmPartie;
    QTimer*      minuteurDefilement;
    int          numeroMessage;
    QStringList  listeMessages;

#ifdef SIMULATION_CLAVIER_ACCUEIL
    void simulerAffichageFenetre();
#endif

  public:
    IHM(QWidget* parent = nullptr);
    virtual ~IHM();

  private slots:
    void defilerTexte();
    void fermer();
#ifdef SIMULATION_CLAVIER_ACCUEIL
    void afficherPartie();
    void afficherClassement();
    void afficherStatistiques();
#endif
};

#endif // IHM_H
