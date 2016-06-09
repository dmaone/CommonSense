#include <stdint.h>

#include <QLCDNumber>
#include <QPalette>
#include <QCloseEvent>
#include "MatrixMonitor.h"
#include "ui_MatrixMonitor.h"
#include "DeviceInterface.h"
#include "singleton.h"
#include "Events.h"
#include "../c2/c2_protocol.h"

MatrixMonitor::MatrixMonitor(uint8_t cols, uint8_t rows, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::MatrixMonitor),
    rows(rows), cols(cols),
    debug(0), displayMode(0), grid(new QGridLayout())
{
    this->enableOutput(0); // tell the keyboard so it does not spam us before we install handler.
    ui->setupUi(this);
    connect(ui->VoltagesButton, SIGNAL(clicked()), this, SLOT(voltagesButtonClick()));
    connect(ui->CloseButton, SIGNAL(clicked()), this, SLOT(closeButtonClick()));
    connect(ui->FreezeButton, SIGNAL(clicked()), this, SLOT(freezeButtonClick()));
    connect(ui->ModeSelector, SIGNAL(currentTextChanged(QString)), this, SLOT(displayModeChanged(QString)));
    initDisplay();
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    connect(this, SIGNAL(sendCommand(uint8_t, uint8_t)), &di, SLOT(sendCommand(uint8_t, uint8_t)));
    di.installEventFilter(this);
}

MatrixMonitor::~MatrixMonitor()
{
    delete grid;
    delete ui;
}

void MatrixMonitor::initDisplay(void)
{
    for (uint8_t i = 0; i<rows; i++)
    {
        for (uint8_t j = 0; j<cols; j++)
        {
            QLCDNumber *l = new QLCDNumber(4);
            l->setSegmentStyle(QLCDNumber::Filled);
            l->setMinimumHeight(25);
            l->setMaximumWidth(40);
            grid->addWidget(l, i, j);
            display[i][j] = l;
        }
    }
    ui->Dashboard->setLayout(grid);
}

bool MatrixMonitor::eventFilter(QObject *obj __attribute__((unused)), QEvent *event){
    if (event->type() == DeviceMessage::ET )
    {
        QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
        if (pl->at(0) != C2RESPONSE_MATRIX_STATUS)
            return false;
        uint8_t col = pl->at(1);
        if (col < 8)
            col = 7 - col; //Swap port 1
        for (uint8_t i = 0; i<8; i++) {
            QLCDNumber *cell = display[i][col];
            uint8_t level = pl->at(3+2*i);
            if (displayMode == 0
               or (displayMode == 1 and level < cell->intValue())
               or (displayMode == 2 and level > cell->intValue())
            )
                cell->display(level);
            if (level > 4)
                cell->setStyleSheet("background-color: #00ff00;");
            else
                cell->setStyleSheet("background-color: #ffffff;");
        }
        return true;
    }
    return false;
}

void MatrixMonitor::enableOutput(uint8_t m)
{
    emit sendCommand(C2CMD_GET_MATRIX_STATE, m);
}

void MatrixMonitor::voltagesButtonClick(void)
{
    this->enableOutput(1);
}

void MatrixMonitor::freezeButtonClick(void)
{
    this->enableOutput(0);
}

void MatrixMonitor::closeButtonClick(void)
{
    this->close();
}

void MatrixMonitor::closeEvent (QCloseEvent *event)
{
    this->enableOutput(0);
    event->accept();
}

void MatrixMonitor::displayModeChanged(QString s)
{
    if (s == "Min")
        displayMode = 1;
    else if (s == "Max")
        displayMode = 2;
    else
        displayMode = 0;
}
