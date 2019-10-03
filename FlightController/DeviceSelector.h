#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <memory>
#include <string>

using DeviceList = std::vector<std::pair<QString, std::string>>;

class DeviceSelector : public QDialog
{
  Q_OBJECT

 public:
  DeviceSelector(DeviceList& list, QWidget *parent = 0);
  ~DeviceSelector() {};

  QString getResult() {
    return buttons_[selectedIndex]->text();
  }
 public slots:
  void onClick(size_t idx);

 private:
  std::unique_ptr<QVBoxLayout> layout_;
  std::unique_ptr<QLabel> label_;
  std::vector<std::unique_ptr<QPushButton>> buttons_{};
  size_t selectedIndex{0};
};
