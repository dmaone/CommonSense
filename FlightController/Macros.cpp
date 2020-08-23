#include <QLabel>
#include <QMenu>
#include <QMessageBox>
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
  connect(ui->scanCode, SIGNAL(currentIndexChanged(int)), SLOT(userChanged()));
  connect(ui->triggerEvent,
      SIGNAL(currentIndexChanged(int)), SLOT(userChanged()));
}

void Macros::show() {
  ui->macroListCombo->clear();
  for (auto& m : config_.macros) {
    ui->macroListCombo->addItem(m.fullName());
  }
  ui->macroListCombo->addItem(kNew);
  currentMacro = config_.macros.size() - 1;
  changed = false;
  ui->macroListCombo->setCurrentIndex(std::max(currentMacro, 0));
  QWidget::show();
  QWidget::raise();
}

void Macros::on_revertButton_clicked() {
  on_macroListCombo_currentIndexChanged(ui->macroListCombo->currentIndex());
}

void Macros::on_resetButton_clicked() {
  auto result = QMessageBox::question(
      this, "Are you sure?", "Erase all macros?",
      QMessageBox::Yes | QMessageBox::No);
  if (result == QMessageBox::Yes) {
    config_.macros.clear();
    currentMacro = 0;
    show();
  }
}

void Macros::populateSteps(QByteArray &bytes) {
  int row = -1;
  int mptr = 0;
  while(mptr < bytes.size()) {
    addStep(++row);
    auto *cmd = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 0));
    auto *delay = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 1));
    auto *sc = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 2));
    auto curByte = static_cast<uint8_t>(bytes[mptr]);
    delay->setCurrentIndex((curByte >> 2) & 0x0f);
    switch (curByte >> 6) {
      case 0: // type
        cmd->setCurrentIndex(3);
        sc->setCurrentIndex(static_cast<uint8_t>(bytes[++mptr]));
        break;
      case 1: // press or release
        if (bytes[mptr] & 0x02) {
          // key up
          cmd->setCurrentIndex(2);
        } else {
          // key down
          cmd->setCurrentIndex(1);
        }
        sc->setCurrentIndex(static_cast<uint8_t>(bytes[++mptr]));
        break;
      case 2:
        qInfo() << "WTF unsupported macro command";
        break;
      case 3: // wait
        cmd->setCurrentIndex(4);
        break;
    }
    ++mptr;
  }
  changed = false;
}

QByteArray Macros::encodeSteps(int /* macro_row */) {
  QByteArray retval;
  for (int row = 0; row < ui->bodyTable->rowCount() - 1; row++) {
    auto *cmd = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 0));
    auto *delay = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 1));
    auto *sc = qobject_cast<QComboBox *>(ui->bodyTable->cellWidget(row, 2));
    bool shortCommand = false;
    uint8_t commandByte = ((uint8_t)delay->currentIndex() << 2);
    // See firmware sources for explanations on encoding.
    switch (cmd->currentIndex()) {
      case 0:
        continue;
      case 1: // press
        commandByte |= (0x01 << 6);
        break;
      case 2: // release
        commandByte |= (0x01 << 6) | 0x02;
        break;
      case 3: // tap
        break;
      case 4: // wait
         commandByte |= (0x03 << 6);
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

void Macros::on_macroListCombo_currentIndexChanged(int index) {
  bool existingMacro = (index + 1 != ui->macroListCombo->count());
  if (changed) {
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
  connect(addStepButton, SIGNAL(clicked()), SLOT(addStepButtonClicked()));
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
    populateSteps(m.body);
  }
  ui->addButton->setEnabled(false);
  ui->deleteButton->setEnabled(existingMacro);
  changed=false;
}

void Macros::on_addButton_clicked() {
  ui->addButton->setEnabled(false);
  if (!changed) {
    return;
  }
  auto newMacro = Macro(ui->scanCode->currentIndex(),
                        ui->triggerEvent->currentText(),
                        encodeSteps(currentMacro));
  size_t pos = ui->macroListCombo->currentIndex();
  bool existingMacro = false;
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
  changed = false;
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

void Macros::on_deleteButton_clicked() {
  auto pos = ui->macroListCombo->currentIndex();
  ui->macroListCombo->removeItem(pos);
  config_.macros.erase(config_.macros.begin() + pos);
}

void Macros::addStepButtonClicked() {
  auto row = ui->bodyTable->rowCount() - 1;
  addStep(row);
  changed = true;
}

void Macros::addStep(int row) {
  ui->bodyTable->insertRow(row);
  QComboBox *cmd = new QComboBox();
  cmd->addItems(QStringList{"-select-", "Press", "Release", "Type", "Wait"});
  connect(cmd, SIGNAL(currentIndexChanged(int)), SLOT(cmdIndexChanged(int)));
  cmd->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(cmd, SIGNAL(customContextMenuRequested(QPoint)),
    SLOT(showContextMenu(QPoint)));
  ui->bodyTable->setCellWidget(row, 0, cmd);
  QComboBox *delay = new QComboBox();
  for (size_t pos = 0; pos < config_.numDelays; ++pos) {
    delay->addItem(QString("%1 ms").arg(config_.getDelay(pos)));
  }
  connect(delay, SIGNAL(currentIndexChanged(int)), SLOT(userChanged()));
  delay->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(delay, SIGNAL(customContextMenuRequested(QPoint)),
    SLOT(showContextMenu(QPoint)));
  ui->bodyTable->setCellWidget(row, 1, delay);
  QComboBox *sc = new QComboBox();
  sc->addItems(*ScancodeList().list);
  connect(sc, SIGNAL(currentIndexChanged(int)), SLOT(userChanged()));
  sc->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(sc, SIGNAL(customContextMenuRequested(QPoint)),
    SLOT(showContextMenu(QPoint)));
  ui->bodyTable->setCellWidget(row, 2, sc);
}

void Macros::showContextMenu(QPoint pt) {
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
  contextMenuRow = index.row();
  QMenu *menu = new QMenu(this);

  menu->addAction("&Insert row", this, SLOT(contextMenuInsertTriggered()));
  menu->addAction("&Delete row", this, SLOT(contextMenuDeleteTriggered()));

  menu->popup(ui->bodyTable->viewport()->mapToGlobal(pt));
}
void Macros::contextMenuInsertTriggered() {
  if (contextMenuRow >= 0 && contextMenuRow < ui->bodyTable->rowCount() - 1) {
    addStep(contextMenuRow);
  }
}

void Macros::contextMenuDeleteTriggered() {
  if (contextMenuRow >= 0 && contextMenuRow < ui->bodyTable->rowCount() - 1) {
    ui->bodyTable->removeRow(contextMenuRow);
  }
}

int Macros::findWidgetRow(QWidget *w) {
  for(int i=0; i < ui->bodyTable->rowCount(); i++) {
    if (ui->bodyTable->cellWidget(i, 0) == w) {
      return i;
    }
  }
  return -1;
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

void Macros::cmdIndexChanged(int /* idx */) {
  auto *cb = qobject_cast<QComboBox *>(QObject::sender());
  if (!cb) {
    return;
  }
  int command = cb->currentIndex();
  int row = findWidgetRow(cb);
  if (row >= 0) {
    fillCommandParameters(row, command);
  }
  changed = true;
  ui->addButton->setEnabled(true);
}

void Macros::userChanged() {
  changed = true;
  ui->addButton->setEnabled(true);
}

void Macros::on_closeButton_clicked() { this->close(); }
