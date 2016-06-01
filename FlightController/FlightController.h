#ifndef FLIGHTCONTROLLER_H
#define FLIGHTCONTROLLER_H

#include <QMainWindow>

namespace Ui {
class FlightController;
}

class FlightController : public QMainWindow
{
    Q_OBJECT

public:
    explicit FlightController(QWidget *parent = 0);
    ~FlightController();

private:
    Ui::FlightController *ui;

};

#endif // FLIGHTCONTROLLER_H
