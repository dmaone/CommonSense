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
#include "Telemetry.h"
#include "settings.h"

namespace {

const QString kEmptyStatLabel{"--- --- ---"};

const QString kStart{"Start!"};
const QString kStop{"Stop!"};

} // namespace

Telemetry::Telemetry(DeviceInterface& di) :
    QFrame{nullptr, Qt::Tool}, di_{di} {
  ui->setupUi(this);

  connect(
      &di, SIGNAL(keypress(DeviceInterface::KeyState)),
      this, SLOT(keypress(DeviceInterface::KeyState)));

  di.installEventFilter(this);

  connect(ui->modeBox, SIGNAL(currentTextChanged(QString)),
      this, SLOT(setDisplayMode_(QString)));

  auto click = [this] (auto button, auto who, auto slot) {
    this->connect(button, SIGNAL(clicked()), who, slot);
  };

  click(ui->closeButton, this, SLOT(close_()));
  click(ui->exportButton, this, SLOT(export_()));
  click(ui->resetButton, this, SLOT(resetCells_()));
  click(ui->runButton, this, SLOT(run_()));
  click(ui->setThresholdsButton, this, SLOT(setThresholds_()));
}

void Telemetry::init() {
  initialized_.store(false);
  grid_ = std::make_unique<QGridLayout>();

  LabelList labels;
  for (uint8_t i = 1; i <= di_.config.numCols; ++i) {
    labels.emplace_back(std::make_unique<QLabel>(QString("%1").arg(i)));
    grid_->addWidget(labels.back().get(), 0, i, 1, 1, Qt::AlignRight);
  }

  std::vector<Cell> cells{di_.config.getMatrixSize()};
  auto cell = cells.begin();
  for (uint8_t i = 1; i <= di_.config.numRows; ++i) {
    labels.emplace_back(std::make_unique<QLabel>(QString("%1").arg(i)));
    grid_->addWidget(labels.back().get(), i, 0, 1, 1, Qt::AlignRight);
    for (uint8_t j = 1; j <= di_.config.numCols; ++j) {
      cell->subLayout.setSpacing(0);
      cell->subLayout.setContentsMargins(0, 0, 0, 0);
      cell->widget.setLayout(&cell->subLayout);

      cell->readout.setDigitCount(3);
      cell->readout.setSegmentStyle(QLCDNumber::Filled);
      cell->readout.setMinimumHeight(25);
      cell->subLayout.addWidget(&cell->readout);

      cell->stats.setText(kEmptyStatLabel);
      cell->subLayout.addWidget(&cell->stats, 0, Qt::AlignRight);

      grid_->addWidget(&cell->widget, i, j, 1, 1);
      ++cell;
    }
  }

  ui->Dashboard->setLayout(grid_.get());
  adjustSize();
  cells_.swap(cells);
  labels_.swap(labels);
  resetCells_();
  initialized_.store(true);
  // TODO fix start button label and/or disable telemetry on connect
  // enableReporting_(di_.getStatusBit(C2DEVSTATUS_TELEMETRY_MODE));
}

void Telemetry::deinit() {
  if (!initialized_.exchange(false)) {
    return;
  }
  isActive_ = false;
  // Do not reset cells here - no crime if old values stay, but avoids SIGSEGV
  // on a dangling pointer in the UI when closing.
}

Telemetry::Cell& Telemetry::getCell_(uint8_t row, uint8_t col) {
  return cells_.at(row * di_.config.numCols + col);
}

bool Telemetry::eventFilter(QObject* /* obj */, QEvent* event) {
  if (initialized_.load() == false) {
    return false;
  }
  if (event->type() != DeviceMessage::ET) {
    return false;
  }
  QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
  if (pl->at(0) != C2RESPONSE_TELEMETRY_ROW) {
    return false;
  }
  if (warmupRows_ > 0) {
    --warmupRows_;
    return true;
  }

  uint8_t row = pl->at(1);
  uint8_t max_cols = pl->at(2);
  for (uint8_t i = 0; i < max_cols; ++i) {
    auto& cell = getCell_(row, i);
    uint8_t level = pl->constData()[3 + i];
    const auto thr = di_.config.thresholds[row][i];
    if (thr == K_IGNORE_KEY) {
      cell.readout.setStyleSheet("background-color: #999999;");
    } else if (di_.getStatusBit(deviceStatus::C2DEVSTATUS_INSANE)) {
      if (level > 0) {
        cell.readout.setStyleSheet("color: black; background-color: #ff3333;");
      } else {
        cell.readout.setStyleSheet("");
      }
    } else {
      if (di_.config.capabilities.isNormallyLow ? level > thr : level < thr) {
        cell.readout.setStyleSheet("color: black; background-color: #33ff33;");
      } else {
        cell.readout.setStyleSheet("");
      }
    }
    cell.now = level;
    cell.min = std::min(level, cell.min);
    cell.max = std::max(level, cell.max);
    cell.sum += level;
    cell.count++;

    cell.stats.setText(QString("%1 %2 %3")
        .arg(cell.min).arg(cell.sum / cell.count).arg(cell.max));

    switch (displayMode_) {
    case DisplayNow:
      cell.readout.display(cell.now);
      break;
    case DisplayMin:
      cell.readout.display(cell.min);
      break;
    case DisplayMax:
      cell.readout.display(cell.max);
      break;
    case DisplayAvg:
      cell.readout.display(((uint8_t)(cell.sum / cell.count)));
      break;
    default:
      qCritical() << "Unknown display mode selected!!";
      close();
    }
  }
  return false;
}

void Telemetry::enableReporting_(bool newState) {
  if (initialized_.load() == false) {
    ui->runButton->setText(kStart);
    isActive_ = false;
  }
  ui->runButton->setText(newState ? kStop : kStart);
  if (isActive_ == newState) {
    return;
  }
  isActive_ = newState;
  // TODO feed commands slowly, so they won't be lost.
  // di_.setStatusBit(C2DEVSTATUS_OUTPUT_ENABLED, false);
  di_.sendCommand(C2CMD_ENABLE_TELEMETRY, newState ? 1 : 0);
  di_.setStatusBit(C2DEVSTATUS_SCAN_ENABLED, true);
}

void Telemetry::closeEvent(QCloseEvent *event) {
  this->enableReporting_(false);
  event->accept();
}

// slots -----------------------------------------------------------------------

void Telemetry::close_() {
  this->enableReporting_(false);
  this->close();
}

void Telemetry::export_() {
  QSettings settings;
  QFileDialog fd(nullptr, "Choose one file to export to");
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
    for (uint8_t i = 0; i < di_.config.numRows; i++) {
      for (uint8_t j = 0; j < di_.config.numCols; j++) {
        const auto& cell = getCell_(i, j);
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

void Telemetry::keypress(DeviceInterface::KeyState state) {
  auto& readout = getCell_(state.row,state.col).readout;
  if (state.status == DeviceInterface::KeyPressed) {
    readout.setStyleSheet("color: black; background-color: #ffff33");
  } else {
    readout.setStyleSheet("");
  }
}

void Telemetry::resetCells_() {
  enableReporting_(false);
  for (auto& cell : cells_) {
    cell.count = 0;
    cell.sum = 0;
    cell.now = 0;
    cell.min = 255;
    cell.max = 0;
    cell.readout.display(0);
    cell.stats.setText(kEmptyStatLabel);
  }
  warmupRows_ = ABSOLUTE_MAX_ROWS; // Workaround - stale data may come in couple
                                   // of first rows.
}

void Telemetry::run_() {
  this->enableReporting_(!isActive_);
}

void Telemetry::setDisplayMode_(QString newValue) {
  if (newValue == "Now")
    displayMode_ = DisplayNow;
  else if (newValue == "Min")
    displayMode_ = DisplayMin;
  else if (newValue == "Max")
    displayMode_ = DisplayMax;
  else if (newValue == "Avg")
    displayMode_ = DisplayAvg;
  else
    qCritical() << "Unknown display mode selected!!";
}

void Telemetry::setThresholds_() {
  if (initialized_.load() == false) {
    return;
  }
  for (uint8_t i = 0; i < di_.config.numRows; i++) {
    for (uint8_t j = 0; j < di_.config.numCols; j++) {
      di_.config.thresholds[i][j] = getCell_(i,j).readout.intValue();
    }
  }
}
