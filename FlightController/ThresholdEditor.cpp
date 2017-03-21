#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>

#include "ThresholdEditor.h"
#include "ui_ThresholdEditor.h"
#include "DeviceInterface.h"
#include "singleton.h"

ThresholdEditor::ThresholdEditor(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ThresholdEditor),
    grid(new QGridLayout())
{
    ui->setupUi(this);
    //TODO DeviceInterface &di = Singleton<DeviceInterface>::instance();
    // TODO deviceConfig = di.getConfigPtr();
}

void ThresholdEditor::show(void)
{
    if (deviceConfig->matrixCols && deviceConfig->matrixCols)
    {
        initDisplay(deviceConfig->matrixRows, deviceConfig->matrixCols);
        connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importThresholds()));
        connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyThresholds()));
        connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(resetThresholds()));
        QWidget::show();
    } else
        QMessageBox::critical(this, "Error", "Matrix not configured - cannot edit layout");
}

ThresholdEditor::~ThresholdEditor()
{
    delete ui;
}


void ThresholdEditor::initDisplay(uint8_t rows, uint8_t cols)
{
    for (uint8_t i = 1; i<=deviceConfig->matrixCols; i++)
    {
        grid->addWidget(new QLabel(QString("%1").arg(i)), 0, i, 1, 1, Qt::AlignRight);
        grid->itemAtPosition(0, i)->widget()->setVisible(i<=cols);
        if (i <= deviceConfig->matrixRows) {
            grid->addWidget(new QLabel(QString("%1").arg(i)), i, 0, 1, 1, Qt::AlignRight);
            grid->itemAtPosition(i, 0)->widget()->setVisible(i<=rows);
        }
    }
    for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
    {
        for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
        {
            QSpinBox *l = new QSpinBox();
            l->setMaximumWidth(40);
            l->setMaximumHeight(25);
            display[i][j] = l;
            grid->addWidget(l, i+1, j+1, 1, 1);
        }
    }
    resetThresholds();
    ui->Dashboard->setLayout(grid);
}

void ThresholdEditor::importThresholds()
{

}

void ThresholdEditor::applyThresholds()
{
    for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
    {
        for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
        {
            //TODO uint8_t kc = display[i][j]->value();
            // TODO deviceConfig->storage[(deviceConfig->row_params[i].rowNumber-1)*deviceConfig->matrixCols + deviceConfig->col_params[j].colNumber-1] = kc;
        }
    }
}


void ThresholdEditor::resetThresholds()
{
    for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
    {
        for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
        {
            /* TODO
            uint8_t kc = deviceConfig->storage[i*deviceConfig->matrixCols + j];
            QSpinBox *cell = display[i][j];
            cell->setValue(kc);
            */
        }
    }
    emit logMessage(QString("Loaded thresholds from device"));
}
