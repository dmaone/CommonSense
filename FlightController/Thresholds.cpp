#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>

#include "DeviceInterface.h"
#include "Thresholds.h"

Thresholds::Thresholds(DeviceInterface& di) :
    QFrame{nullptr, Qt::Tool}, di_{di} {
  ui->setupUi(this);
  initDisplay();
  connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyThresholds()));
  connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(resetThresholds()));
  connect(ui->incButton, SIGNAL(clicked()), this, SLOT(increaseThresholds()));
  connect(ui->decButton, SIGNAL(clicked()), this, SLOT(decreaseThresholds()));

  connect(
      &di, SIGNAL(keypress(DeviceInterface::KeyState)),
      this, SLOT(keypress(DeviceInterface::KeyState)));

  di.installEventFilter(this);
}

void Thresholds::show() {
  if (!di_.config.bValid) {
    QMessageBox::critical(this, "Error", "Matrix not configured - cannot edit");
    return;
  }
  updateDisplaySize(di_.config.numRows, di_.config.numCols);
  resetThresholds();
  QWidget::show();
  QWidget::raise();
}

void Thresholds::initDisplay() {
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    _grid.addWidget(new QLabel(QString("%1").arg(i)), 0, i, 1, 1,
                    Qt::AlignRight);
    if (i <= ABSOLUTE_MAX_ROWS) {
      _grid.addWidget(new QLabel(QString("%1").arg(i)), i, 0, 1, 1,
                      Qt::AlignRight);
    }
  }
  for (uint8_t i = 0; i < ABSOLUTE_MAX_ROWS; i++) {
    for (uint8_t j = 0; j < ABSOLUTE_MAX_COLS; j++) {
      QSpinBox *l = new QSpinBox();
      l->setMaximum(254);
      l->setMinimum(0);
      l->setAlignment(Qt::AlignRight);
      l->setMaximumWidth(80);
      connect(l, QOverload<int>::of(&QSpinBox::valueChanged),
          [this, l](int){ paintCell(l); });
      display[i][j] = l;
      _grid.addWidget(l, i + 1, j + 1, 1, 1);
    }
  }
  ui->Dashboard->setLayout(&_grid);
}

void Thresholds::updateDisplaySize(uint8_t rows, uint8_t cols) {
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    if (i <= ABSOLUTE_MAX_ROWS)
      _grid.itemAtPosition(i, 0)->widget()->setVisible(i <= rows);
    _grid.itemAtPosition(0, i)->widget()->setVisible(i <= cols);
  }
  for (uint8_t i = 0; i < ABSOLUTE_MAX_ROWS; i++) {
    for (uint8_t j = 0; j < ABSOLUTE_MAX_COLS; j++) {
      display[i][j]->setVisible((i < rows) & (j < cols));
    }
  }
  adjustSize();
}

void Thresholds::adjustThresholds(size_t delta) {
  size_t nonZeroes{0};
  for (uint8_t i = 0; i < di_.config.numRows; i++) {
    for (uint8_t j = 0; j < di_.config.numCols; j++) {
      nonZeroes += display[i][j]->value();
    }
  }
  for (uint8_t i = 0; i < di_.config.numRows; i++) {
    for (uint8_t j = 0; j < di_.config.numCols; j++) {
      auto threshold = display[i][j]->value();
      if (threshold == 0 && nonZeroes > 0) {
        continue;
      }
      display[i][j]->setValue(threshold + delta);
    }
  }
}

void Thresholds::paintCell(QSpinBox *cell) {
  cell->setStyleSheet(
      cell->value() == K_IGNORE_KEY ? "background-color: #999999" : "");
}

void Thresholds::increaseThresholds() {
  adjustThresholds(ui->adjustSpinbox->value());
}

void Thresholds::decreaseThresholds() {
  adjustThresholds(0 - ui->adjustSpinbox->value());
}

void Thresholds::applyThresholds() {
  for (uint8_t i = 0; i < di_.config.numRows; i++) {
    for (uint8_t j = 0; j < di_.config.numCols; j++) {
      di_.config.thresholds[i][j] = display[i][j]->value();
    }
  }
}

void Thresholds::resetThresholds() {
  for (uint8_t i = 0; i < di_.config.numRows; i++) {
    for (uint8_t j = 0; j < di_.config.numCols; j++) {
      display[i][j]->setValue(di_.config.thresholds[i][j]);
      paintCell(display[i][j]);
    }
  }
  qInfo() << "Loaded threshold map";
}

bool Thresholds::eventFilter(QObject* /* obj */, QEvent* event) {
  if (event->type() != DeviceMessage::ET) {
    return false;
  }

  QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
  if (pl->at(0) != C2RESPONSE_MATRIX_ROW
      || !di_.getStatusBit(deviceStatus::C2DEVSTATUS_INSANE)) {
    return false;
  }

  uint8_t row = pl->at(1);
  uint8_t max_cols = pl->at(2);
  for (uint8_t i = 0; i < max_cols; i++) {
    const auto thr = di_.config.thresholds[row][i];
    qDebug() << row << " " << i << " " << thr << " = " << pl->constData()[3 + i];
    if (thr == K_IGNORE_KEY) {
      display[row][i]->setStyleSheet("background-color: #999999;");
    } else if (pl->constData()[3 + i] > 0) {
      display[row][i]->setStyleSheet("color: black; background-color: #ff3333;");
    } else {
      display[row][i]->setStyleSheet("");
    }
  }
  return false;
}

void Thresholds::keypress(DeviceInterface::KeyState state) {
  if (state.status != DeviceInterface::KeyPressed) {
    return paintCell(display[state.row][state.col]);
  }
  display[state.row][state.col]->setStyleSheet("color: black; background-color: #ffff33");
}

void Thresholds::on_closeButton_clicked() { this->close(); }


