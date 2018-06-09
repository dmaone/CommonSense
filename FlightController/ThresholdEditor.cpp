#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>

#include "../c2/c2_protocol.h"
#include "DeviceInterface.h"
#include "ThresholdEditor.h"
#include "singleton.h"
#include "ui_ThresholdEditor.h"

ThresholdEditor::ThresholdEditor(DeviceConfig *config, QWidget *parent)
    : QFrame(parent), ui(new Ui::ThresholdEditor), grid(new QGridLayout()) {
  ui->setupUi(this);
  deviceConfig = config;
  initDisplay();
  connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyThresholds()));
  connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(resetThresholds()));
  connect(ui->adjustButton, SIGNAL(clicked()), this, SLOT(adjustThresholds()));
}

void ThresholdEditor::show(void) {
  if (deviceConfig->bValid) {
    updateDisplaySize(deviceConfig->numRows, deviceConfig->numCols);
    resetThresholds();
    QWidget::show();
  } else {
    QMessageBox::critical(this, "Error", "Matrix not configured - cannot edit");
  }
}

ThresholdEditor::~ThresholdEditor() { delete ui; }

void ThresholdEditor::initDisplay() {
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    grid->addWidget(new QLabel(QString("%1").arg(i)), 0, i, 1, 1,
                    Qt::AlignRight);
    if (i <= ABSOLUTE_MAX_ROWS) {
      grid->addWidget(new QLabel(QString("%1").arg(i)), i, 0, 1, 1,
                      Qt::AlignRight);
    }
  }
  for (uint8_t i = 0; i < ABSOLUTE_MAX_ROWS; i++) {
    for (uint8_t j = 0; j < ABSOLUTE_MAX_COLS; j++) {
      QSpinBox *l = new QSpinBox();
      l->setMaximum(254);
      l->setAlignment(Qt::AlignRight);
      l->setStyleSheet("background-color: #ffffff;");
      display[i][j] = l;
      grid->addWidget(l, i + 1, j + 1, 1, 1);
    }
  }
  ui->Dashboard->setLayout(grid);
}

void ThresholdEditor::updateDisplaySize(uint8_t rows, uint8_t cols) {
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    if (i <= ABSOLUTE_MAX_ROWS)
      grid->itemAtPosition(i, 0)->widget()->setVisible(i <= rows);
    grid->itemAtPosition(0, i)->widget()->setVisible(i <= cols);
  }
  for (uint8_t i = 0; i < ABSOLUTE_MAX_ROWS; i++) {
    for (uint8_t j = 0; j < ABSOLUTE_MAX_COLS; j++) {
      display[i][j]->setVisible((i < rows) & (j < cols));
    }
  }
  adjustSize();
}

void ThresholdEditor::adjustThresholds() {
  for (uint8_t i = 0; i < deviceConfig->numRows; i++) {
    for (uint8_t j = 0; j < deviceConfig->numCols; j++) {
      display[i][j]->setValue(display[i][j]->value() +
                              ui->adjustSpinbox->value());
    }
  }
}

void ThresholdEditor::applyThresholds() {
  for (uint8_t i = 0; i < deviceConfig->numRows; i++) {
    for (uint8_t j = 0; j < deviceConfig->numCols; j++) {
      deviceConfig->thresholds[i][j] = display[i][j]->value();
    }
  }
}

void ThresholdEditor::resetThresholds() {
  for (uint8_t i = 0; i < deviceConfig->numRows; i++) {
    for (uint8_t j = 0; j < deviceConfig->numCols; j++) {
      display[i][j]->setValue(deviceConfig->thresholds[i][j]);
    }
  }
  qInfo() << "Loaded threshold map";
}

void ThresholdEditor::receiveScancode(uint8_t row, uint8_t col,
                                      DeviceInterface::KeyStatus status) {
  if (status == DeviceInterface::KeyPressed) {
    display[row][col]->setStyleSheet("background-color: #ffff33;");
  } else {
    display[row][col]->setStyleSheet("background-color: #ffffff;");
  }
}

void ThresholdEditor::on_closeButton_clicked() { this->close(); }
