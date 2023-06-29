#include <QApplication>
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow* q_widget = new MainWindow(nullptr);
    q_widget->show();

    return a.exec();
}
