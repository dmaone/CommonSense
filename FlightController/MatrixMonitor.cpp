#include <algorithm>
#include <stdint.h>

#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QLCDNumber>
#include <QMessageBox>
#include <QTextStream>

#include "DeviceInterface.h"
#include "Events.h"
#include "MatrixMonitor.h"
#include "settings.h"
#include "ui_MatrixMonitor.h"

MatrixMonitor::MatrixMonitor(QWidget *parent)
    : QFrame(parent, Qt::Tool), ui(new Ui::MatrixMonitor), debug(0),
      displayMode(DisplayNow), grid(new QGridLayout()),
      _warmupRows(ABSOLUTE_MAX_ROWS) {
  ui->setupUi(this);

  initDisplay();
  auto& di = DeviceInterface::get();
  connect(this, SIGNAL(sendCommand(c2command, uint8_t)), &di,
          SLOT(sendCommand(c2command, uint8_t)));
  connect(this, SIGNAL(setStatusBit(deviceStatus, bool)), &di,
          SLOT(setStatusBit(deviceStatus, bool)));
  di.installEventFilter(this);
  deviceConfig = di.config;
}

void MatrixMonitor::show() {
  if (deviceConfig->bValid) {
    updateDisplaySize(deviceConfig->numRows, deviceConfig->numCols);
    QWidget::show();
    QWidget::raise();
  } else {
    QMessageBox::critical(this, "Error",
                          "Matrix not configured - cannot monitor");
  }
}

MatrixMonitor::~MatrixMonitor() { delete ui; }

void MatrixMonitor::initDisplay() {
  this->enableTelemetry(0);
  grid->setSpacing(0);
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    grid->addWidget(new QLabel(QString("%1").arg(i)), 0, i, 1, 1,
                    Qt::AlignCenter);
    if (i <= ABSOLUTE_MAX_ROWS) {
      grid->addWidget(new QLabel(QString("%1").arg(i)), i, 0, 1, 1,
                      Qt::AlignRight);
    }
  }
  for (uint8_t i = 0; i < ABSOLUTE_MAX_ROWS; i++) {
    for (uint8_t j = 0; j < ABSOLUTE_MAX_COLS; j++) {
      QWidget *w = new QWidget;
      grid->addWidget(w, i + 1, j + 1, 1, 1);
      QVBoxLayout *ll = new QVBoxLayout;
      ll->setSpacing(0);
      w->setLayout(ll);

      QLCDNumber *l = new QLCDNumber(3);
      l->setSegmentStyle(QLCDNumber::Filled);
      l->setMinimumHeight(25);
      display[i][j] = l;
      ll->addWidget(l);
      QLabel *lbl = new QLabel("-- -- --");
      ll->addWidget(lbl, 0, Qt::AlignRight);
      statsDisplay[i][j] = lbl;
    }
  }
  ui->Dashboard->setLayout(grid);
  _resetCells();
}

void MatrixMonitor::updateDisplaySize(uint8_t rows, uint8_t cols) {
  this->enableTelemetry(0);
  for (uint8_t i = 1; i <= ABSOLUTE_MAX_COLS; i++) {
    if (i <= ABSOLUTE_MAX_ROWS)
      grid->itemAtPosition(i, 0)->widget()->setVisible(i <= rows);
    grid->itemAtPosition(0, i)->widget()->setVisible(i <= cols);
  }
  for (uint8_t i = 0; i < ABSOLUTE_MAX_ROWS; i++) {
    for (uint8_t j = 0; j < ABSOLUTE_MAX_COLS; j++) {
      grid->itemAtPosition(i + 1, j + 1)
          ->widget()
          ->setVisible((i < rows) && (j < cols));
    }
  }
  _resetCells();
  adjustSize();
}

bool MatrixMonitor::eventFilter(QObject* /* obj */,
                                QEvent *event) {
  if (event->type() == DeviceMessage::ET) {
    QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
    if (pl->at(0) != C2RESPONSE_MATRIX_ROW)
      return false;
    if (_warmupRows > 0) {
      _warmupRows--;
      return true;
    }
    auto& di = DeviceInterface::get();
    uint8_t row = pl->at(1);
    uint8_t max_cols = pl->at(2);
    for (uint8_t i = 0; i < max_cols; i++) {
      QLCDNumber *cell = display[row][i];
      uint8_t level = pl->constData()[3 + i];
      const auto thr = deviceConfig->thresholds[row][i];
      if (thr == K_IGNORE_KEY) {
        cell->setStyleSheet("background-color: #999999;");
      } else if (di.getStatusBit(deviceStatus::C2DEVSTATUS_INSANE)) {
        if (level > 0) {
          cell->setStyleSheet("color: black; background-color: #ff3333;");
        } else {
          cell->setStyleSheet("");
        }
      } else {
        if (
          (deviceConfig->bNormallyLow && level > thr) ||
          (!deviceConfig->bNormallyLow && level < thr)
        ) {
          cell->setStyleSheet("color: black; background-color: #33ff33;");
        } else {
          cell->setStyleSheet("");
        }
      }
      _updateStatCell(row, i, level);
      switch (displayMode) {
      case DisplayNow:
        cell->display(cells[row][i].now);
        break;
      case DisplayMin:
        cell->display(cells[row][i].min);
        break;
      case DisplayMax:
        cell->display(cells[row][i].max);
        break;
      case DisplayAvg:
        cell->display((uint8_t)(cells[row][i].sum / cells[row][i].count));
        break;
      default:
        qCritical() << "Unknown display mode selected!!";
        close();
      }
    }
  }
  return false;
}

void MatrixMonitor::receiveScancode(uint8_t row, uint8_t col,
                                      DeviceInterface::KeyStatus status) {
  if (status == DeviceInterface::KeyPressed) {
    display[row][col]->setStyleSheet("color: black; background-color: #ffff33");
  } else {
    display[row][col]->setStyleSheet("");
  }
}

void MatrixMonitor::enableTelemetry(uint8_t m) {
  ui->runButton->setText(m ? "Stop!" : "Start!");
  // TODO feed commands slowly, so they won't be lost.
  // emit setStatusBit(C2DEVSTATUS_OUTPUT_ENABLED, false);
  emit sendCommand(C2CMD_GET_MATRIX_STATE, m);
  emit setStatusBit(C2DEVSTATUS_SCAN_ENABLED, true);
}

void MatrixMonitor::on_runButton_clicked() {
  this->enableTelemetry(ui->runButton->text() == "Stop!" ? 0 : 1);
}

void MatrixMonitor::on_setThresholdsButton_clicked() {
  if (!deviceConfig->bValid)
    return;
  for (uint8_t i = 0; i < deviceConfig->numRows; i++) {
    for (uint8_t j = 0; j < deviceConfig->numCols; j++) {
      deviceConfig->thresholds[i][j] = display[i][j]->intValue();
    }
  }
}

void MatrixMonitor::on_closeButton_clicked() { this->close(); }

void MatrixMonitor::closeEvent(QCloseEvent *event) {
  this->enableTelemetry(0);
  event->accept();
}

void MatrixMonitor::on_modeBox_currentTextChanged(QString newValue) {
  if (newValue == "Now")
    displayMode = DisplayNow;
  else if (newValue == "Min")
    displayMode = DisplayMin;
  else if (newValue == "Max")
    displayMode = DisplayMax;
  else if (newValue == "Avg")
    displayMode = DisplayAvg;
  else
    qCritical() << "Unknown display mode selected!!";
}

void MatrixMonitor::_resetCells() {
  for (uint8_t i = 0; i < ABSOLUTE_MAX_ROWS; i++) {
    for (uint8_t j = 0; j < ABSOLUTE_MAX_COLS; j++) {
      cells[i][j] = {.now = 0, .min = 255, .max = 0, .sum = 0, .count = 0};
      _updateStatCellDisplay(i, j);
      display[i][j]->display(0);
    }
  }
  _warmupRows = ABSOLUTE_MAX_ROWS; // Workaround - stale data may come in couple
                                   // of first rows.
}

void MatrixMonitor::_updateStatCell(uint8_t row, uint8_t col, uint8_t level) {
  cells[row][col].now = level;
  cells[row][col].min = std::min(level, cells[row][col].min);
  cells[row][col].max = std::max(level, cells[row][col].max);
  cells[row][col].sum += level;
  cells[row][col].count++;
  _updateStatCellDisplay(row, col);
}

void MatrixMonitor::_updateStatCellDisplay(uint8_t row, uint8_t col) {
  if (cells[row][col].count == 0) {
    statsDisplay[row][col]->setText(QString("-- -- --"));
    return;
  }
  const auto& cell = cells[row][col];
  statsDisplay[row][col]->setText(QString("%1 %2 %3").arg(cell.min)
                                                     .arg(cell.sum / cell.count)
                                                     .arg(cell.max));
}

void MatrixMonitor::on_resetButton_clicked() {
  enableTelemetry(0);
  _resetCells();
}

void MatrixMonitor::on_exportButton_clicked() {
  QSettings settings;
  QFileDialog fd(Q_NULLPTR, "Choose one file to export to");
  fd.setDirectory(settings.value(SETTINGS_DIR_KEY).toString());
  fd.setNameFilter(tr("Matrix stats(*.csv)"));
  fd.setDefaultSuffix(QString("csv"));
  fd.setAcceptMode(QFileDialog::AcceptSave);
  if (fd.exec()) {
    QStringList fns = fd.selectedFiles();
    QFile f(fns.at(0));
    f.open(QIODevice::WriteOnly);
    QTextStream ts(&f);
    ts << "Row,Col,Min,Max,Avg,Sum,Count\n";
    ts.setIntegerBase(10);
    for (uint8_t i = 0; i < deviceConfig->numRows; i++) {
      for (uint8_t j = 0; j < deviceConfig->numCols; j++) {
        const auto& cell = cells[i][j];
        ts << i << "," << j << ",";
        ts << cell.min << "," << cell.max << ",";
        if (cell.count == 0) {
          ts << "0,";
        } else {
          ts << cell.sum / cell.count << ",";
        }
        ts << cell.sum << "," << cell.count << "\n";
      }
    }
    f.close();
  }
}
