#pragma once

#include <QTCore>
#include <QGridLayout>
#include <QLCDNumber>
#include <stdint.h>
#include "Events.h"
#include "../c2/c2_protocol.h"
#include "../c2/nvram.h"

namespace Ui {
class MatrixMonitor;
}

class MatrixMonitor : public QFrame
{
    Q_OBJECT

public:
    explicit MatrixMonitor(QWidget *parent = 0);
    ~MatrixMonitor();
    void show(void);

signals:
    void sendCommand(uint8_t, uint8_t);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *);

private:
    Ui::MatrixMonitor *ui;
    uint8_t debug;
    uint8_t displayMode;
    QGridLayout *grid;
    QLCDNumber *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    psoc_eeprom_t* deviceConfig;
    void initDisplay(void);
    void updateDisplaySize(uint8_t, uint8_t);
    void enableOutput(uint8_t);

private slots:
    void voltagesButtonClick(void);
    void closeButtonClick(void);
    void thresholdsButtonClick(void);
    void displayModeChanged(QString);

};
