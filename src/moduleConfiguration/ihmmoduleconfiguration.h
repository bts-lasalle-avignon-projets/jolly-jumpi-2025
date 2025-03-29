/**
 * @file ihmmoduleconfiguration.h
 *
 * @brief Déclaration de la classe IHMModuleConfiguration
 * @author Axel Ariati
 * @version 1.0
 */

#ifndef IHMMODULECONFIGURATION_H
#define IHMMODULECONFIGURATION_H

#include <QtWidgets>

/**
 * @def APPLICATION
 * @brief Le nom de l'application
 */
#define APPLICATION "ModuleConfiguration"

/**
 * @def VERSION
 * @brief La version de l'application
 */
#define VERSION "1.0"

QT_BEGIN_NAMESPACE
namespace Ui
{
class IHMModuleConfiguration;
}
QT_END_NAMESPACE

/**
 * @class IHMModuleConfiguration
 * @brief Déclaration de la classe IHMModuleConfiguration
 * @details Cette classe gère l'interface graphique de l'application
 */
class IHMModuleConfiguration : public QMainWindow
{
    Q_OBJECT

  public:
    IHMModuleConfiguration(QWidget* parent = nullptr);
    ~IHMModuleConfiguration();

  private:
    Ui::IHMModuleConfiguration* ui;
};

#endif // IHMMODULECONFIGURATION_H
