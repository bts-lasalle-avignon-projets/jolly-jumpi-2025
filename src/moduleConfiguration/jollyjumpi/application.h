#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class application; }
QT_END_NAMESPACE

class application : public QMainWindow
{
    Q_OBJECT

public:
    application(QWidget *parent = nullptr);
    ~application();

private:
    Ui::application *ui;
};
#endif // APPLICATION_H
