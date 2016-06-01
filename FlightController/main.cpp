#include <QApplication>
#include "FlightController.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FlightController w;
    w.show();

    return a.exec();
}
