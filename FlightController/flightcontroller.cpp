#include "FlightController.h"
#include "ui_flightcontroller.h"
#include "singleton.h"
#include "DeviceInterface.h"

FlightController::FlightController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FlightController)
{
    ui->setupUi(this);
    connect(ui->ClearButton, SIGNAL(clicked()), ui->LogViewport, SLOT(clearButtonClick()));
    connect(ui->CopyAllButton, SIGNAL(clicked()), ui->LogViewport, SLOT(copyAllButtonClick()));
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    di.setLogger(ui->LogViewport);
    di.connect();
}

FlightController::~FlightController()
{
    delete ui;
}
