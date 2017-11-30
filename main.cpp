#include <QApplication>
#include "controller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<double> >("QVector<double>");

    Controller controller;
    controller.start();

    return a.exec();
}
