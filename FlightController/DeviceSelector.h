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
  DeviceSelector(const DeviceList& list);
  ~DeviceSelector() {};

  QString getResult() {
    return buttons_[selectedIndex_]->text();
  }

 public slots:
  void onClick(size_t idx);

 private:
  QVBoxLayout layout_{};
  QLabel label_{};
  std::vector<std::unique_ptr<QPushButton>> buttons_{};
  size_t selectedIndex_{0};
};
