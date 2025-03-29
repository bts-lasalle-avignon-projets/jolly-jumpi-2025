/**
 * @file ihmmoduleconfiguration.cpp
 *
 * @brief Définition de la classe IHMModuleConfiguration
 * @author Axel Ariati
 * @version 1.0
 */

#include "ihmmoduleconfiguration.h"
#include "ui_ihmmoduleconfiguration.h"
#include <QDebug>

/**
 * @brief Constructeur de la classe IHMModuleConfiguration
 *
 * @fn IHMModuleConfiguration::IHMModuleConfiguration
 * @param parent L'adresse de l'objet parent, si nullptr IHMModuleConfiguration
 * sera la fenêtre principale de l'application
 */
IHMModuleConfiguration::IHMModuleConfiguration(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::IHMModuleConfiguration)
{
    ui->setupUi(this);
    qDebug() << Q_FUNC_INFO << this;

    setWindowTitle(QString(APPLICATION) + QString(" v") + QString(VERSION));

#ifdef RASPBERRY_PI
    showFullScreen();
#endif
}

IHMModuleConfiguration::~IHMModuleConfiguration()
{
    delete ui;
    qDebug() << Q_FUNC_INFO << this;
}
