#include "gestionPartie.h"
#include <QString>
#include <QDebug>

void gestionPartie::configurerPartie(int mode, int nbJoueurs)
{
    QString trame = QString("$PC%1;%2\n").arg(mode).arg(nbJoueurs);
    qDebug() << "Envoi trame : " << trame;
}

void gestionPartie::lancerPartie()
{
}

void gestionPartie::arreterPartie()
{
}
