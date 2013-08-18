#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStyle *style = QStyleFactory::create("Fusion");
    QPalette p;
    a.setStyle(style);

    MainWindow w;
    w.show();

    return a.exec();
}
