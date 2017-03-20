#include <QLabel>
#include <QMessageBox>
#include <QComboBox>
#include <QFileDialog>
#include <QTextStream>

#include "LayoutEditor.h"
#include "ui_LayoutEditor.h"
#include "DeviceInterface.h"
#include "singleton.h"
#include "ScancodeList.h"

LayoutEditor::LayoutEditor(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::LayoutEditor),
    grid(new QGridLayout())
{
    ui->setupUi(this);
    DeviceInterface &di = Singleton<DeviceInterface>::instance();
    // TODO deviceConfig = di.getConfigPtr();
}

void LayoutEditor::show(void)
{
    if (deviceConfig->matrixCols && deviceConfig->matrixCols)
    {
        initDisplay(deviceConfig->matrixRows, deviceConfig->matrixCols);
        connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importLayout()));
        connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportLayout()));
        connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applyLayout()));
        connect(ui->revertButton, SIGNAL(clicked()), this, SLOT(resetLayout()));
        QWidget::show();
    } else
        QMessageBox::critical(this, "Error", "Matrix not configured - cannot edit layout");
}

LayoutEditor::~LayoutEditor()
{
    delete ui;
}

void LayoutEditor::initDisplay(uint8_t rows, uint8_t cols)
{
    for (uint8_t i = 1; i<=deviceConfig->matrixCols; i++)
    {
        grid->addWidget(new QLabel(QString("%1").arg(i)), 0, i, 1, 1, Qt::AlignRight);
        grid->itemAtPosition(0, i)->widget()->setVisible(i<=cols);
        if (i <= deviceConfig->matrixRows) {
            grid->addWidget(new QLabel(QString("%1").arg(i)), i, 0, 1, 1, Qt::AlignRight);
            grid->itemAtPosition(i, 0)->widget()->setVisible(i<=rows);
        }
    }
    ScancodeList scancodes;
    for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
    {
        for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
        {
            QComboBox *l = new QComboBox();
            l->addItems(scancodes.list);
            l->setMaximumWidth(60);
            l->setMaximumHeight(25);
            display[i][j] = l;
            grid->addWidget(l, i+1, j+1, 1, 1);
        }
    }
    resetLayout();
    ui->Dashboard->setLayout(grid);
}

void LayoutEditor::importLayout()
{
    QFileDialog fd(this, "Choose one file to import from");
    fd.setNameFilter(tr("Layout files(*.l)"));
    fd.setDefaultSuffix(QString("l"));
    fd.setFileMode(QFileDialog::ExistingFile);
    if (fd.exec())
    {
        QStringList fns = fd.selectedFiles();
        QFile f(fns.at(0));
        f.open(QIODevice::ReadOnly);
        QTextStream ds(&f);
        for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
        {
            for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
            {
                qint32 buf;
                ds >> buf;
                display[i][j]->setCurrentIndex((uint8_t) buf);
            }
        }
        emit logMessage(QString("Imported layout from %1").arg(fns.at(0)));
    }
}

void LayoutEditor::exportLayout()
{
    QFileDialog fd(this, "Choose one file to export to");
    fd.setNameFilter(tr("layout files(*.l)"));
    fd.setDefaultSuffix(QString("l"));
    fd.setAcceptMode(QFileDialog::AcceptSave);
    if (fd.exec())
    {
        QStringList fns = fd.selectedFiles();
        QFile f(fns.at(0));
        f.open(QIODevice::WriteOnly);
        QTextStream ts (&f);
        ts.setIntegerBase(16);
        ts.setFieldAlignment(QTextStream::AlignRight);
        ts.setPadChar('0');
        for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
        {
            QByteArray buf;
            for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
            {
                ts << qSetFieldWidth(1) << (j ? ' ' : '\n');
                ts << "0x";
                ts.setFieldWidth(2);
                ts << (uint8_t)display[i][j]->currentIndex();

            }
        }
        ts << qSetFieldWidth(1) << '\n';
        emit logMessage(QString("Exported layout to %1").arg(fns.at(0)));
    }
}

void LayoutEditor::applyLayout()
{
    // Reverse mapping - scan by mapped, record raw.
    // Reverse recording - from the end of the storage towards the front.
    for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
    {
        for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
        {
            uint8_t kc = display[i][j]->currentIndex();
            /* TODO
            deviceConfig->storage[STORAGE_ADDRESS((deviceConfig->row_params[i].rowNumber-1)*deviceConfig->matrixCols + deviceConfig->col_params[j].colNumber-1)] = kc;
            */
        }
    }

}

void LayoutEditor::resetLayout()
{
    for (uint8_t i = 0; i<deviceConfig->matrixRows; i++)
    {
        for (uint8_t j = 0; j<deviceConfig->matrixCols; j++)
        {
            /* TODO
            uint8_t kc = deviceConfig->storage[STORAGE_ADDRESS(i*deviceConfig->matrixCols + j)];
            VERIFY we select proper cell!
            QComboBox *cell = display[i][j];
            cell->setCurrentIndex(kc);
             */
        }
    }
    emit logMessage(QString("Loaded layout from device"));
}
