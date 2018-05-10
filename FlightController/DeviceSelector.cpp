#include "DeviceSelector.h"
#include "ui_DeviceSelector.h"

DeviceSelector::DeviceSelector(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::DeviceSelector)
{
  ui->setupUi(this);
}

DeviceSelector::~DeviceSelector()
{
  delete ui;
}
