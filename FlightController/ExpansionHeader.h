#ifndef EXPANSIONHEADER_H
#define EXPANSIONHEADER_H

#include <QDialog>

#include "DeviceConfig.h"

namespace Ui {
class ExpansionHeader;
}

class ExpansionHeader : public QDialog {
  Q_OBJECT

public:
  explicit ExpansionHeader(DeviceConfig *config, QWidget *parent = 0);
  ~ExpansionHeader();
  void init(void);

private:
  Ui::ExpansionHeader *ui;
  DeviceConfig *_config;
  void _updateParamVisibility(void);

private slots:
  void on_modeBox_currentIndexChanged(int idx);
  void accept(void);
};

#endif // EXPANSIONHEADER_H
