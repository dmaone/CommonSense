#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QTextStream>

#include "DeviceInterface.h"
#include "Pedals.h"
#include "settings.h"

Pedals::Pedals(DeviceConfig& config)
    : QFrame{nullptr, Qt::Tool}, config_{config} {
  ui->setupUi(this);
  initDisplay();
}

void Pedals::show() {
  if (config_.bValid) {
    sizeDisplay(config_.numRows, config_.numCols);
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    QWidget::show();
    QWidget::raise();
  } else
    QMessageBox::critical(this, "Error",
                          "Matrix not configured - cannot edit layout");
}

void Pedals::initDisplay() {
  /*
  ui->Dashboard->setLayout(grid);
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    grid->addWidget(new QLabel(QString("%1").arg(i)), 0, i, 1, 1,
                    Qt::AlignRight);
  }
  // Do not populate lists here - makes startup SLOW
    for (uint8_t i = 0; i < ABSOLUTE_MAX_COLS; i++) {
      display[i] = nullptr;
    }
    */
}

void Pedals::sizeDisplay(uint8_t rows, uint8_t cols) {
  /*
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    grid->itemAtPosition(0, i)->widget()->setVisible(i <= cols);
    if (i <= ABSOLUTE_MAX_ROWS) {
      grid->itemAtPosition(i, 0)->widget()->setVisible(i <= rows);
    }
  }
  ScancodeList scancodes;
  for (uint8_t i = 0; i < rows; i++) {
    for (uint8_t j = 0; j < cols; j++) {
      if (!display[i][j]) {
        QComboBox *l = new QComboBox();
        l->setEditable(false);
        l->addItems(*scancodes.list);
        l->setMinimumContentsLength(scancodes.width);
        l->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        display[i][j] = l;
        grid->addWidget(l, i + 1, j + 1, 1, 1);
        if (j == 0) {
          qInfo() << ".";
        }
      }
      display[i][j]->setVisible((i < rows) && (j < cols));
    }
  }
  revert();
  adjustSize();
  */
}


void Pedals::apply() {
  /*
  for (uint8_t i = 0; i < config_.numRows; i++) {
    for (uint8_t j = 0; j < config_.numCols; j++) {
      config_.layouts[currentLayer][i][j] = display[i][j]->currentIndex();
    }
  }
  */
}

void Pedals::setDisplay() {
  /*
  for (uint8_t i = 0; i < config_.numRows; i++) {
    for (uint8_t j = 0; j < config_.numCols; j++) {
      if (config_.thresholds[i][j] == K_IGNORE_KEY) {
        display[i][j]->setCurrentIndex(0);
        display[i][j]->setEnabled(false);
      } else {
        display[i][j]->setCurrentIndex(config_.layouts[currentLayer][i][j]);
        display[i][j]->setEnabled(true);
      }
    }
  }
  */
}

void Pedals::revert() {
  setDisplay();
  qInfo() << "Loaded pedal config from device";
}

void Pedals::receiveScancode(uint8_t row, uint8_t col,
                                   DeviceInterface::KeyStatus status) {
  if (!display[row][col])
    return;
  if (status == DeviceInterface::KeyPressed) {
    display[row][col]->setStyleSheet("color: black; background-color: #ffff33;");
    display[row][col]->setFocus();
  } else {
    display[row][col]->setStyleSheet("");
  }
}

void Pedals::on_closeButton_clicked() { this->close(); }
