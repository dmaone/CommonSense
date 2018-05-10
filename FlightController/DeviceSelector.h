#ifndef DEVICESELECTOR_H
#define DEVICESELECTOR_H

#include <QWidget>

namespace Ui {
  class DeviceSelector;
}

class DeviceSelector : public QWidget
{
  Q_OBJECT

public:
  explicit DeviceSelector(QWidget *parent = 0);
  ~DeviceSelector();

private:
  Ui::DeviceSelector *ui;
};

#endif // DEVICESELECTOR_H
