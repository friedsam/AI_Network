#include <QtGui/QApplication>
#include <qwt_plot.h>

#include <iostream>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.plotData();

    return a.exec();
}
