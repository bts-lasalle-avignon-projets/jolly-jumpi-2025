/**
 * @file ihmmoduleconfiguration.cpp
 *
 * @brief Définition de la classe IHMModuleConfiguration
 * @author Axel Ariati
 * @version 1.0
 */

#include "ihmmoduleconfiguration.h"
#include "ui_ihmmoduleconfiguration.h"
#include "bluetooth.h"

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
    bluetoothInstance = new Bluetooth(this);

    QMovie* movie = new QMovie(":/images/loader.gif");

    movie->setScaledSize(ui->labelGif_2->size());
    ui->labelGif_2->setMovie(movie);
    movie->start();

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

    connect(ui->annulerPiste,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onAnnulerPisteClicked);

    connect(ui->confirmer,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onConfirmerClicked);

    connect(ui->nombreDeJoueur,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &IHMModuleConfiguration::mettreAJourAffichageJoueurs);

    mettreAJourAffichageJoueurs();

    connect(ui->demarrer,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onDemarrerClicked);

    connect(ui->interrompre,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onInterrompreClicked);

    connect(ui->menuAccueil,
            &QPushButton::clicked,
            this,
            &IHMModuleConfiguration::onMenuClicked);

    connect(bluetoothInstance,
            &Bluetooth::associationReussie,
            this,
            &IHMModuleConfiguration::gererAssociationReussie);

    connect(bluetoothInstance,
            &Bluetooth::partieDemarree,
            this,
            &IHMModuleConfiguration::gererDemarragePartie);
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

void IHMModuleConfiguration::afficherArreter()
{
    ui->stackedWidget->setCurrentWidget(ui->accueil);
}

void IHMModuleConfiguration::onAnnulerClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->accueil);
}

void IHMModuleConfiguration::onAnnulerPisteClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->configuration);
}
static int modeToInt(const QString& mode)
{
    if(mode == "Normal")
        return 0;
    if(mode == "Aléatoire")
        return 1;
    if(mode == "Temps")
        return 2;
    return 0;
}

void IHMModuleConfiguration::onConfirmerClicked()
{
    bluetoothInstance->envoyer("$A\n");
    int nombreJoueurs = ui->nombreDeJoueur->value();

    QString modeTexte = ui->modeDeJeu->currentText();

    int mode = 0;
    if(modeTexte == "Normal")
        mode = 0;
    else if(modeTexte == "Aléatoire")
        mode = 1;
    else if(modeTexte == "Temps")
        mode = 2;
    else
        qWarning() << "Mode de jeu inconnu : " << modeTexte;

    QString trame = QString("$PC%1;%2\n").arg(mode).arg(nombreJoueurs);
    qDebug() << "[Trame envoyée] " << trame;

    bluetoothInstance->envoyer(trame);

    ui->stackedWidget->setCurrentWidget(ui->attentePiste);
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

void IHMModuleConfiguration::onDemarrerClicked()
{
    bluetoothInstance->envoyer("$PD\n");
}
void IHMModuleConfiguration::onInterrompreClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->configuration);
}

void IHMModuleConfiguration::onMenuClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->accueil);
}

void IHMModuleConfiguration::gererAssociationReussie()
{
    int     nombreJoueurs = ui->nombreDeJoueur->value();
    QString modeTexte     = ui->modeDeJeu->currentText();
    int     mode          = modeToInt(modeTexte);

    QString trame = QString("$PC%1;%2\n").arg(mode).arg(nombreJoueurs);
    bluetoothInstance->envoyer(trame);

    ui->stackedWidget->setCurrentWidget(ui->connexionReussie);
}

void IHMModuleConfiguration::gererDemarragePartie()
{
    ui->stackedWidget->setCurrentWidget(ui->partieEnCours);
}
