#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

#include "DeviceInterface.h"
#include "Macros.h"
#include "ScancodeList.h"

namespace {
  static const QString kNew{"-new-"};
} // namespace

Macros::Macros(DeviceConfig& config) :
    QFrame{nullptr, Qt::Tool}, config_{config} {
  ui->setupUi(this);
  ScancodeList scanCodes;
  ui->scanCode->addItems(*scanCodes.list);
  connect(ui->scanCode, SIGNAL(currentIndexChanged(int)), SLOT(setDirty_()));

  connect(ui->macroListCombo, SIGNAL(currentIndexChanged(int)),
      this, SLOT(selectMacro_(int)));

  connect(ui->triggerEvent, SIGNAL(currentIndexChanged(int)),
      this, SLOT(setDirty_()));

  connect(ui->addButton, &QPushButton::clicked,
      this, [this](){ appendMacro_(); });

  connect(ui->closeButton, &QPushButton::clicked, this, [this](){ close(); });

  connect(ui->deleteButton, &QPushButton::clicked,
      this, [this](){ deleteCurrentMacro_(); });

  connect(ui->resetButton, &QPushButton::clicked, this, [this](){ reset_(); });

  connect(ui->revertButton, &QPushButton::clicked,
      this, [this](){ selectMacro_(ui->macroListCombo->currentIndex()); });

}

void Macros::show() {
  ui->macroListCombo->clear();
  for (auto& m : config_.macros) {
    ui->macroListCombo->addItem(m.fullName());
  }
  ui->macroListCombo->addItem(kNew);
  currentMacro_ = config_.macros.size() - 1;
  dirty_ = false;
  ui->macroListCombo->setCurrentIndex(std::max(currentMacro_, 0));
  QWidget::show();
  QWidget::raise();
}

void Macros::reset_() {
  auto result = QMessageBox::question(
      this, "Are you sure?", "Erase all macros?",
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes) {
    config_.macros.clear();
    currentMacro_ = 0;
    show();
  }
}

void Macros::populateSteps_(QByteArray &bytes) {
  int row = -1;
  int mptr = 0;
  while(mptr < bytes.size()) {
    addStep_(++row);
    auto *cmd = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 0));
    auto *delay = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 1));
    auto *sc = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 2));
    auto curByte = static_cast<uint8_t>(bytes[mptr]);
    delay->setCurrentIndex(
        (curByte & MACROCMD_DELAY_MASK) >> MACROCMD_DELAY_SHIFT);
    switch (curByte >> MACROCMD_CMD_SHIFT) {
      case MACROCMD_TYPE:
        cmd->setCurrentIndex(3);
        sc->setCurrentIndex(static_cast<uint8_t>(bytes[++mptr]));
        break;
      case MACROCMD_ACTUATE:
        cmd->setCurrentIndex((bytes[mptr] & MACROCMD_ACTUATE_KEYUP) ? 2 : 1);
        sc->setCurrentIndex(static_cast<uint8_t>(bytes[++mptr]));
        break;
      case MACROCMD_IGNORED:
        qInfo() << "WTF unsupported macro command";
        break;
      case MACROCMD_WAIT: // wait
        cmd->setCurrentIndex(4);
        break;
    }
    ++mptr;
  }
  dirty_ = false;
}

QByteArray Macros::encodeSteps_(int /* macro_row */) {
  QByteArray retval;
  for (int row = 0; row < ui->bodyTable->rowCount() - 1; row++) {
    auto *cmd = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 0));
    auto *delay = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 1));
    auto *sc = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 2));
    bool shortCommand = false;
    uint8_t commandByte = abs(delay->currentIndex()) << MACROCMD_DELAY_SHIFT;
    // See c2_protocol.h for explanations on encoding.
    switch (cmd->currentIndex()) {
      case 0: // Nothing selected - skip
        continue;
      case 1:
        commandByte += MACROCMD_ACTUATE << MACROCMD_CMD_SHIFT;
        break;
      case 2:
        commandByte += MACROCMD_ACTUATE << MACROCMD_CMD_SHIFT;
        commandByte += MACROCMD_ACTUATE_KEYUP;
        break;
      case 3:
        commandByte += MACROCMD_TYPE << MACROCMD_CMD_SHIFT; // secretly nop
        break;
      case 4:
         commandByte += MACROCMD_WAIT << MACROCMD_CMD_SHIFT;
         shortCommand = true;
        break;
    }
    retval.append(commandByte);
    if (!shortCommand) {
      retval.append(((char)sc->currentIndex()));
    }
  }
  return retval;
}

void Macros::selectMacro_(int index) {
  bool existingMacro = (index + 1 != ui->macroListCombo->count());
  if (dirty_) {
    QMessageBox::question(this, "Warning",
        "All your base are belong to us - unsaved changes will be lost",
        QMessageBox::Ok);
  }
  ui->bodyTable->clearContents();
  ui->bodyTable->setRowCount(1);
  ui->bodyTable->setColumnCount(3);
  ui->bodyTable->setHorizontalHeaderLabels(
      QStringList() << "Command" << "Delay" << "Key");
  QPushButton *addStepButton = new QPushButton("+");
  connect(addStepButton, SIGNAL(clicked()), SLOT(appendStep_()));
  ui->bodyTable->setSpan(0, 0, 1, 3);
  ui->bodyTable->setCellWidget(0, 0, addStepButton);
  if (!existingMacro) {
    ui->addButton->setText("Add");
    ui->scanCode->setCurrentIndex(0);
    ui->triggerEvent->setCurrentIndex(2);
  } else {
    ui->addButton->setText("Save");
    auto& m = config_.macros[index];
    ui->scanCode->setCurrentIndex(m.keyCode);
    ui->triggerEvent->setCurrentText(m.getTriggerEventText());
    populateSteps_(m.body);
  }
  ui->addButton->setEnabled(false);
  ui->deleteButton->setEnabled(existingMacro);
  dirty_ = false;
}

void Macros::appendMacro_() {
  ui->addButton->setEnabled(false);
  if (!dirty_) {
    return;
  }
  auto newMacro = Macro(ui->scanCode->currentIndex(),
                        ui->triggerEvent->currentText(),
                        encodeSteps_(currentMacro_));
  size_t pos = ui->macroListCombo->currentIndex();
  bool existingMacro{false};
  if (pos < config_.macros.size()) {
    config_.macros[pos] = newMacro;
    existingMacro = true;
  }
  size_t cur_pos = 0;
  for (const auto& m : config_.macros) {
    if (pos == cur_pos++) {
      continue;
    }
    if (newMacro.keyCode == m.keyCode && newMacro.flags == m.flags) {
      QMessageBox::warning(this, "Error",
          "That activation sequence already taken!");
      return;
    }
  }
  dirty_ = false;
  if (existingMacro) {
    ui->macroListCombo->setItemText(pos, newMacro.fullName());
  } else {
    // Must insert tap macros first, firmware depends on that order
    auto it = std::lower_bound(
        config_.macros.cbegin(), config_.macros.cend(), newMacro);
    config_.macros.insert(it, newMacro);

    ui->macroListCombo->removeItem(pos);
    ui->macroListCombo->addItem(newMacro.fullName());
    ui->macroListCombo->addItem(kNew);
    ui->macroListCombo->setCurrentIndex(pos+1);
  }
}

void Macros::deleteCurrentMacro_() {
  auto pos = ui->macroListCombo->currentIndex();
  ui->macroListCombo->removeItem(pos);
  config_.macros.erase(config_.macros.begin() + pos);
}

void Macros::addStep_(int row) {
  ui->bodyTable->insertRow(row);
  QComboBox *cmd = new QComboBox();
  cmd->addItems(QStringList{"-select-", "Press", "Release", "Type", "Wait"});
  connect(cmd, SIGNAL(currentIndexChanged(int)), SLOT(setTriggerMode_(int)));
  cmd->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(cmd, SIGNAL(customContextMenuRequested(QPoint)),
      this, SLOT(contextMenu_(QPoint)));
  ui->bodyTable->setCellWidget(row, 0, cmd);
  QComboBox *delay = new QComboBox();
  for (size_t pos = 0; pos < config_.numDelays; ++pos) {
    delay->addItem(QString("%1 ms").arg(config_.getDelay(pos)));
  }
  connect(delay, SIGNAL(currentIndexChanged(int)), SLOT(setDirty_()));
  delay->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(delay, SIGNAL(customContextMenuRequested(QPoint)),
    this, SLOT(contextMenu_(QPoint)));
  ui->bodyTable->setCellWidget(row, 1, delay);
  QComboBox *sc = new QComboBox();
  sc->addItems(*ScancodeList().list);
  connect(sc, SIGNAL(currentIndexChanged(int)), SLOT(setDirty_()));
  sc->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(sc, SIGNAL(customContextMenuRequested(QPoint)),
    this, SLOT(contextMenu_(QPoint)));
  ui->bodyTable->setCellWidget(row, 2, sc);
}

void Macros::appendStep_() {
  addStep_(ui->bodyTable->rowCount() - 1);
  dirty_ = true;
}

void Macros::contextMenu_(QPoint pt) {
  QWidget *w = qobject_cast<QWidget *>(QObject::sender());
  if (!w) {
    return;
  }
  QAbstractScrollArea *sa = qobject_cast<QAbstractScrollArea *>(w);
  /* map to global coords */
  if (sa == nullptr)
    pt = ui->bodyTable->viewport()->mapFromGlobal(w->mapToGlobal(pt));
  else
    pt = ui->bodyTable->viewport()->mapFromGlobal(
        sa->viewport()->mapToGlobal(pt));
  /* correct point for LHS of table viewport, in case click was on vertical
   * header
   */
  pt.setX(std::max(pt.x(), 0));

  QModelIndex index = ui->bodyTable->indexAt(pt);
  if (!index.isValid() || index.row() == ui->bodyTable->rowCount() - 1)
      return;
  contextMenuRow_ = index.row();
  QMenu *menu = new QMenu(this);

  menu->addAction("&Insert step", this, SLOT(insertStep_()));
  menu->addAction("&Delete step", this, SLOT(deleteStep_()));

  menu->popup(ui->bodyTable->viewport()->mapToGlobal(pt));
}

void Macros::insertStep_() {
  if (contextMenuRow_ >= 0 && contextMenuRow_ < ui->bodyTable->rowCount() - 1) {
    addStep_(contextMenuRow_);
  }
}

void Macros::deleteStep_() {
  if (contextMenuRow_ >= 0 && contextMenuRow_ < ui->bodyTable->rowCount() - 1) {
    ui->bodyTable->removeRow(contextMenuRow_);
  }
}

void Macros::fillCommandParameters(int row, int command) {
  ui->bodyTable->cellWidget(row, 1)->setEnabled(true);
  ui->bodyTable->cellWidget(row, 2)->setEnabled(true);
  switch (command) {
    case 0:
      ui->bodyTable->cellWidget(row, 1)->setEnabled(false);
      ui->bodyTable->cellWidget(row, 2)->setEnabled(false);
      break;
    case 4:
      ui->bodyTable->cellWidget(row, 2)->setEnabled(false);
      break;
    default:
      break;
  }
}

void Macros::setTriggerMode_(int /* idx */) {
  auto *cb = qobject_cast<QComboBox *>(QObject::sender());
  if (!cb) {
    return;
  }
  int command = cb->currentIndex();
  for(int i=0; i < ui->bodyTable->rowCount(); i++) {
    if (ui->bodyTable->cellWidget(i, 0) == cb) {
      fillCommandParameters(i, command);
      break;
    }
  }
  dirty_ = true;
  ui->addButton->setEnabled(true);
}

void Macros::setDirty_() {
  dirty_ = true;
  ui->addButton->setEnabled(true);
}
