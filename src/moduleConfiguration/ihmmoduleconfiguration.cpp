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

    ui->nombreDeJoueur->setRange(1, 8);

    qDebug() << Q_FUNC_INFO << this;

    setWindowTitle(QString(APPLICATION) + QString(" v") + QString(VERSION));

    connect(ui->lancer,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onLancerClicked);

    connect(ui->annuler,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onAnnulerClicked);

    connect(ui->annulerConnexion,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onAnnulerConnexionClicked);

    connect(ui->confirmer,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onConfirmerClicked);

    connect(ui->nombreDeJoueur,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &IHMModuleConfiguration::mettreAJourAffichageJoueurs);

    mettreAJourAffichageJoueurs();

    connect(ui->nombreDeJoueur,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &IHMModuleConfiguration::attenteConnexionPiste);
    attenteConnexionPiste();

#ifdef RASPBERRY_PI
    showFullScreen();
#endif
}

IHMModuleConfiguration::~IHMModuleConfiguration()
{
    delete ui;
    qDebug() << Q_FUNC_INFO << this;
}

void IHMModuleConfiguration::onLancerClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->configuration);
}

void IHMModuleConfiguration::onAnnulerClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->accueil);
}

void IHMModuleConfiguration::onAnnulerConnexionClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->configuration);
}

void IHMModuleConfiguration::onConfirmerClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->attenteConnexion);
}

void IHMModuleConfiguration::mettreAJourAffichageJoueurs()
{
    int nombreJoueurs = ui->nombreDeJoueur->value();

    QLabel* labelsJoueurs[] = { ui->joueur1, ui->joueur2, ui->joueur3,
                                ui->joueur4, ui->joueur5, ui->joueur6,
                                ui->joueur7, ui->joueur8 };

    for(int i = 0; i < 8; ++i)
    {
        labelsJoueurs[i]->setVisible(i < nombreJoueurs);
    }
}

void IHMModuleConfiguration::attenteConnexionPiste()
{
    int nombreJoueurs = ui->nombreDeJoueur->value();

    QLabel* labelsPistes[] = { ui->connexionPiste1, ui->connexionPiste2,
                               ui->connexionPiste3, ui->connexionPiste4,
                               ui->connexionPiste5, ui->connexionPiste6,
                               ui->connexionPiste7, ui->connexionPiste8 };

    for(int i = 0; i < 8; ++i)
    {
        labelsPistes[i]->setVisible(i < nombreJoueurs);
    }
}
