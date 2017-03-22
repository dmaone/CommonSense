#pragma once

#include <QTCore>
#include <QGridLayout>
#include <QLCDNumber>
#include <stdint.h>
#include "Events.h"
#include "../c2/c2_protocol.h"
#include "DeviceConfig.h"

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
    void sendCommand(c2command, uint8_t);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *);

private:
    Ui::MatrixMonitor *ui;
    uint8_t debug;
    uint8_t displayMode;
    QGridLayout *grid;
    QLCDNumber *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    DeviceConfig *deviceConfig;
    void initDisplay(void);
    void updateDisplaySize(uint8_t, uint8_t);
    void enableTelemetry(uint8_t);

private slots:
    void runButtonClick(void);
    void closeButtonClick(void);
    void loButtonClick(void);
    void hiButtonClick(void);
    void displayModeChanged(QString);

};
