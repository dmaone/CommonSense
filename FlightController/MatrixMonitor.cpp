#include <stdint.h>

#include <QLCDNumber>
#include <QPalette>
#include "MatrixMonitor.h"
#include "ui_MatrixMonitor.h"
#include "DeviceInterface.h"
#include "singleton.h"
#include "Events.h"

MatrixMonitor::MatrixMonitor(uint8_t cols, uint8_t rows, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MatrixMonitor),
    rows(rows), cols(cols)
{
    ui->setupUi(this);
    connect(ui->VoltagesButton, SIGNAL(clicked()), this, SLOT(voltagesButtonClick()));
    connect(ui->StatesButton, SIGNAL(clicked()), this, SLOT(statesButtonClick()));
    connect(ui->CloseButton, SIGNAL(clicked()), this, SLOT(closeButtonClick()));
    connect(ui->FreezeButton, SIGNAL(clicked()), this, SLOT(freezeButtonClick()));
    grid.reset(new QGridLayout);
    for (uint8_t i = 0; i<rows; i++)
    {
        for (uint8_t j = 0; j<cols; j++)
        {
            display[i][j].reset(new QLCDNumber(4));
            display[i][j].data()->setSegmentStyle(QLCDNumber::Filled);
            display[i][j].data()->setMinimumHeight(25);
            grid.data()->addWidget(display[i][j].data(), (int)i, (int)j);
        }
    }
    ui->Dashboard->setLayout(grid.data());
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    connect(this, SIGNAL(notifyDevice(QByteArray)), &di, SLOT(notifyDevice(QByteArray)));
    di.installEventFilter(this);
}

MatrixMonitor::~MatrixMonitor()
{

    delete ui;
}

bool MatrixMonitor::eventFilter(QObject *obj, QEvent *event){
    if (event->type() == DeviceMessage::ET )
    {
        QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
        if (pl->at(0) != (unsigned char)1)
            return false;
        uint8_t col = pl->at(1);
        if (col < 8)
            col = 8 - col;
        for (uint8_t i = 0; i<8; i++) {
//                qDebug()<<QString("%1 %2 %3").arg(col).arg(i).arg(pl->at(2+2*i));
            display[i][col].data()->display(pl->at(3+2*i));
            if (pl->at(3+2*i) > 4)
                display[i][col].data()->setStyleSheet("background-color: #00ff00;");
            else
                display[i][col].data()->setStyleSheet("background-color: #ffffff;");
        }
        return true;
    }
    return false;
    //return QObject::eventFilter(obj,event);
}

void MatrixMonitor::setMode(Mode m)
{
    QByteArray pkt= QByteArray::fromHex("0404");
    pkt.append((unsigned char)m);
    emit notifyDevice(pkt);
}

void MatrixMonitor::voltagesButtonClick(void)
{
    this->setMode(MatrixMonitor::Voltages);
}

void MatrixMonitor::statesButtonClick(void)
{
    this->setMode(MatrixMonitor::States);
}

void MatrixMonitor::freezeButtonClick(void)
{
    this->setMode(MatrixMonitor::Off);
}

void MatrixMonitor::closeButtonClick(void)
{
    this->setMode(MatrixMonitor::Off);
    this->close();
}
