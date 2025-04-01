#ifndef ACCUEIL_H
#define ACCUEIL_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Accueil; }
QT_END_NAMESPACE

class Accueil : public QWidget
{
    Q_OBJECT

public:
    Accueil(QWidget *parent = nullptr);
    ~Accueil();

private:
    Ui::Accueil *ui;
};
#endif // ACCUEIL_H
