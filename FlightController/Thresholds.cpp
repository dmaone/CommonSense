#include "Thresholds.h"

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "DeviceInterface.h"

Thresholds::Thresholds(DeviceInterface& di) :
    QFrame{nullptr, Qt::Tool}, di_{di} {
  ui->setupUi(this);

  connect(ui->applyButton, &QPushButton::clicked, this, [this](){ apply_(); });
  connect(ui->revertButton, &QPushButton::clicked, this, [this](){ reset_(); });
  connect(ui->incButton, &QPushButton::clicked,
      this, [this](){ bumpAll_(ui->adjustSpinbox->value()); });
  connect(ui->decButton, &QPushButton::clicked,
      this, [this](){ bumpAll_(0 - ui->adjustSpinbox->value()); });

  connect(ui->closeButton, &QPushButton::clicked, this, [this](){ close(); });

  connect(
      &di, SIGNAL(keypress(DeviceInterface::KeyState)),
      this, SLOT(keypress(DeviceInterface::KeyState)));

  di.installEventFilter(this);
}

void Thresholds::init() {
  LabelList labels;
  grid_ = std::make_unique<QGridLayout>();
  for (uint8_t i = 1; i <= di_.config.numCols; ++i) {
    labels.emplace_back(std::make_unique<QLabel>(QString("%1").arg(i)));
    grid_->addWidget(labels.back().get(), 0, i, 1, 1, Qt::AlignRight);
  }

  std::vector<QSpinBox> cells{di_.config.getMatrixSize()};
  auto cell = cells.begin();
  for (uint8_t i = 1; i <= di_.config.numRows; ++i) {
    labels.emplace_back(std::make_unique<QLabel>(QString("%1").arg(i)));
    grid_->addWidget(labels.back().get(), i, 0, 1, 1, Qt::AlignRight);
    for (uint8_t j = 1; j <= di_.config.numCols; ++j) {
      cell->setMaximum(254);
      cell->setMinimum(0);
      cell->setAlignment(Qt::AlignRight);
      cell->setMaximumWidth(80);
      auto& cellRef = *cell;
      connect(&cellRef, QOverload<int>::of(&QSpinBox::valueChanged),
          [this, &cellRef](int){ paintCell_(cellRef); });

      grid_->addWidget(&cellRef, i, j, 1, 1);
      ++cell;
    }
  }
  ui->Dashboard->setLayout(grid_.get());
  adjustSize();

  cells_.swap(cells);
  labels_.swap(labels);
  reset_();
}


QSpinBox& Thresholds::getCell_(uint8_t row, uint8_t col) {
  return cells_.at(row * di_.config.numCols + col);
}

void Thresholds::bumpAll_(int delta) {
  size_t nonZeroes{0};
  for (auto& cell : cells_) {
    nonZeroes += cell.value();
  }
  for (auto& cell : cells_) {
    auto threshold = cell.value();
    if (threshold == 0 && nonZeroes > 0) {
      continue; // Only bump zeroes when ALL cells are zero.
    }
    cell.setValue(threshold + delta);
  }
}

void Thresholds::paintCell_(QSpinBox& cell) {
  cell.setStyleSheet(cell.value() == SKIP_SCAN ? "background-color: #999999"
                                               : "");
}

void Thresholds::apply_() {
  for (uint8_t i = 0; i < di_.config.numRows; i++) {
    for (uint8_t j = 0; j < di_.config.numCols; j++) {
      di_.config.thresholds[i][j] = getCell_(i, j).value();
    }
  }
}

void Thresholds::reset_() {
  for (uint8_t i = 0; i < di_.config.numRows; i++) {
    for (uint8_t j = 0; j < di_.config.numCols; j++) {
      auto& cell = getCell_(i, j);
      cell.setValue(di_.config.thresholds[i][j]);
      paintCell_(cell);
    }
  }
  qInfo() << "Reloaded threshold map";
}

bool Thresholds::eventFilter(QObject* /* obj */, QEvent* event) {
  if (event->type() != DeviceMessage::ET) {
    return false;
  }

  QByteArray *pl = static_cast<DeviceMessage *>(event)->getPayload();
  if (pl->at(0) != C2RESPONSE_TELEMETRY_ROW
      || !di_.getStatusBit(deviceStatus::C2DEVSTATUS_INSANE)) {
    return false;
  }

  uint8_t row = pl->at(1);
  uint8_t max_cols = pl->at(2);
  for (uint8_t i = 0; i < max_cols; i++) {
    const auto thr = di_.config.thresholds[row][i];
    qDebug() << row << " " << i << " " << thr << " = " << pl->constData()[3 + i];
    auto& cell = getCell_(row, i);
    if (thr == SKIP_SCAN) {
      cell.setStyleSheet("background-color: #999999;");
    } else if (pl->constData()[3 + i] > 0) {
      cell.setStyleSheet("color: black; background-color: #ff3333;");
    } else {
      cell.setStyleSheet("");
    }
  }
  return false;
}

void Thresholds::keypress(DeviceInterface::KeyState state) {
  auto& cell = getCell_(state.row, state.col);
  if (state.status != DeviceInterface::KeyPressed) {
    paintCell_(cell);
    return;
  }
  cell.setStyleSheet("color: black; background-color: #ffff33");
}


