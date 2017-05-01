#include "ExpansionHeader.h"
#include "ui_ExpansionHeader.h"

ExpansionHeader::ExpansionHeader(DeviceConfig *config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExpansionHeader), _config(config)
{
    ui->setupUi(this);
    for (uint8_t i=0; i < (sizeof expModeNames / sizeof expModeNames[0]); i++)
    {
        ui->modeBox->addItem(expModeNames[i]);
    }
}

ExpansionHeader::~ExpansionHeader()
{
    delete ui;
}

void ExpansionHeader::init()
{
    std::vector<uint8_t> params = _config->expHeaderParams();
    ui->modeBox->setCurrentIndex(params[0]);
    ui->Param1->setValue(params[1]);
    ui->Param2->setValue(params[2]);
}

void ExpansionHeader::_updateParamVisibility()
{
    if(ui->modeBox->currentText().contains("Solenoid"))
    {
        ui->Param1Label->setText("Drive time, ms");
        ui->Param1Label->setEnabled(true);
        ui->Param2Label->setText("Cooldown time, ms");
        ui->Param2Label->setEnabled(true);
        ui->Param1->setEnabled(true);
        ui->Param2->setEnabled(true);
    }
    else
    {
        ui->Param1Label->setText("Unused");
        ui->Param1Label->setEnabled(false);
        ui->Param2Label->setText("Unused");
        ui->Param2Label->setEnabled(false);
        ui->Param1->setEnabled(false);
        ui->Param2->setEnabled(false);
    }
}

void ExpansionHeader::on_modeBox_currentIndexChanged(int idx)
{
    Q_UNUSED(idx);
    _updateParamVisibility();
}

void ExpansionHeader::accept(void)
{
    _config->setExpHeaderParams(ui->modeBox->currentIndex(), ui->Param1->value(), ui->Param2->value());
    QDialog::accept();
}
