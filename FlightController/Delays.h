#pragma once

#include <QGridLayout>
#include <QSpinBox>
#include <QWidget>

#include "DeviceConfig.h"

class DelayWatcher : public QObject {
  Q_OBJECT

 public:
  DelayWatcher(DeviceConfig *config, int delayIndex, QSpinBox *box,
               QObject *parent = NULL);
  uint16_t getValue();

 private:
  DeviceConfig *config;
  int delayIndex;
  QSpinBox *box;

 private slots:
  void changed(int delay_ms);
};

class Delays : public QWidget {
  Q_OBJECT
 public:
  std::vector<DelayWatcher *> delayWatchers;
  explicit Delays(DeviceConfig *config, QWidget *parent = 0);
  ~Delays();
  void init();

signals:
 private:
  DeviceConfig *_config;
  QGridLayout *_grid;
  void _deinit();
};
