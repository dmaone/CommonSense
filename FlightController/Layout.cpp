#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QTextStream>

#include "DeviceInterface.h"
#include "Layout.h"
#include "ScancodeList.h"
#include "settings.h"

Layout::Layout(DeviceInterface& di) :
    QFrame{nullptr, Qt::Tool}, config_{di.config} {
  ui->setupUi(this);
  connect(ui->importButton, SIGNAL(clicked()), this, SLOT(import_()));
  connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(export_()));

  connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply_()));
  connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(reset_()));
  connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));

  connect(ui->switchButton, SIGNAL(clicked()), this, SLOT(switchLayer_()));

  connect(
      &di, SIGNAL(keypress(DeviceInterface::KeyState)),
      this, SLOT(keypress(DeviceInterface::KeyState)));
}

void Layout::init() {
  ui->layerCombo->clear();
  ui->layerCombo->addItem(QString("Base Layer"));
  for (uint8_t i = 1; i < config_.numLayers; ++i) {
    ui->layerCombo->addItem(QString("Layer %1").arg(i));
  }

  LabelList labels;
  ComboList display;
  grid_ = std::make_unique<QGridLayout>();
  ui->Dashboard->setLayout(grid_.get());
  for (uint8_t i = 1; i <= config_.numCols; ++i) {
    labels.emplace_back(std::make_unique<QLabel>(QString("%1").arg(i)));
    grid_->addWidget(labels.back().get(), 0, i, 1, 1, Qt::AlignRight);
  }

  ScancodeList scancodes;
  for (uint8_t i = 1; i <= config_.numRows; ++i) {
    labels.emplace_back(std::make_unique<QLabel>(QString("%1").arg(i)));
    grid_->addWidget(labels.back().get(), i, 0, 1, 1, Qt::AlignRight);
    for (uint8_t j = 1; j <= config_.numCols; ++j) {
      display.emplace_back(std::make_unique<QComboBox>());
      auto& box = *display.back();
      box.setEditable(false);
      box.addItems(*scancodes.list);
      box.setMinimumContentsLength(scancodes.width);
      box.setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
      grid_->addWidget(&box, i, j, 1, 1);
    }
  }

  labels_.swap(labels);
  display_.swap(display);
  currentLayer_ = 0;
  syncUiMatrix_();
  adjustSize();
}

QComboBox& Layout::getUiCell_(uint8_t row, uint8_t col) {
  return *display_.at(row * config_.numCols + col);
}

void Layout::import_() {
  QSettings settings;
  QFileDialog fd(this, "Choose one file to import from");
  fd.setDirectory(settings.value(LAYOUTS_DIR_KEY).toString());
  fd.setNameFilter(tr("Layout files(*.l)"));
  fd.setDefaultSuffix(QString("l"));
  fd.setFileMode(QFileDialog::ExistingFile);
  if (!fd.exec()) {
    return;
  }

  QStringList fns = fd.selectedFiles();
  QFile f(fns.at(0));
  f.open(QIODevice::ReadOnly);
  QTextStream ds(&f);
  for (uint8_t l = 0; l < config_.numLayers; l++) {
    for (uint8_t i = 0; i < config_.numRows; i++) {
      for (uint8_t j = 0; j < config_.numCols; j++) {
        qint32 buf;
        ds >> buf;
        config_.layouts[l][i][j] = (uint8_t)buf;
      }
    }
  }
  f.close();
  syncUiMatrix_();
  qInfo() << "Imported layout from" << fns.at(0);
  settings.setValue(LAYOUTS_DIR_KEY, QFileInfo(fns.at(0)).canonicalPath());
}

void Layout::export_() {
  QSettings settings;
  QFileDialog fd(this, "Choose one file to export to");
  fd.setDirectory(settings.value(LAYOUTS_DIR_KEY).toString());
  fd.setNameFilter(tr("layout files(*.l)"));
  fd.setDefaultSuffix(QString("l"));
  fd.setAcceptMode(QFileDialog::AcceptSave);
  if (!fd.exec()) {
    return;
  }
  apply_();
  QStringList fns = fd.selectedFiles();
  QFile f(fns.at(0));
  f.open(QIODevice::WriteOnly);
  QTextStream ts(&f);
  ts.setIntegerBase(16);
  ts.setFieldAlignment(QTextStream::AlignRight);
  ts.setPadChar('0');
  for (uint8_t l = 0; l <config_.numLayers; l++) {
    for (uint8_t i = 0; i < config_.numRows; i++) {
      for (uint8_t j = 0; j < config_.numCols; j++) {
        ts << qSetFieldWidth(1) << (j ? ' ' : '\n');
        ts << "0x";
        ts.setFieldWidth(2);
        ts << config_.layouts[l][i][j];
      }
    }
    ts << qSetFieldWidth(1) << '\n';
  }
  ts << qSetFieldWidth(1) << '\n';
  f.close();
  qInfo() << "Exported layout to" << fns.at(0);
  settings.setValue(LAYOUTS_DIR_KEY, QFileInfo(fns.at(0)).canonicalPath());
}

void Layout::apply_() {
  for (uint8_t i = 0; i < config_.numRows; i++) {
    for (uint8_t j = 0; j < config_.numCols; j++) {
      config_.layouts[currentLayer_][i][j] = getUiCell_(i, j).currentIndex();
    }
  }
}

void Layout::syncUiMatrix_() {
  for (uint8_t i = 0; i < config_.numRows; i++) {
    for (uint8_t j = 0; j <config_.numCols; j++) {
      auto& cell = getUiCell_(i, j);
      if (config_.thresholds[i][j] == K_IGNORE_KEY) {
        cell.setCurrentIndex(0);
        cell.setEnabled(false);
      } else {
        cell.setCurrentIndex(config_.layouts[currentLayer_][i][j]);
        cell.setEnabled(true);
      }
    }
  }
}

void Layout::reset_() {
  syncUiMatrix_();
  qInfo() << "Reloaded layout from the latest 'applied' state";
}

void Layout::switchLayer_() {
  currentLayer_ = ui->layerCombo->currentIndex();
  syncUiMatrix_();
}

void Layout::keypress(DeviceInterface::KeyState state) {
  if (display_.empty()) {
    return; // never happens, but..
  }
  auto& cell = getUiCell_(state.row, state.col);
  if (state.status != DeviceInterface::KeyPressed) {
    cell.setStyleSheet("");
    return;
  }
  cell.setStyleSheet("color: black; background-color: #ffff33;");
  cell.setFocus();
}
