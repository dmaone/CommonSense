#pragma once

#include <QtCore>
#include <QGridLayout>
#include <QLCDNumber>
#include <QLabel>
#include <stdint.h>
#include "Events.h"
#include "../c2/c2_protocol.h"
#include "DeviceConfig.h"

namespace Ui {
class MatrixMonitor;
}

typedef struct {
    uint8_t min;
    uint8_t max;
    uint32_t sum;
    uint32_t sampleCount;
} MonitoredCell;

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
    MonitoredCell cells[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    QLabel *statsDisplay[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    DeviceConfig *deviceConfig;
    uint8_t _warmupRows;

    void initDisplay(void);
    void updateDisplaySize(uint8_t, uint8_t);
    void enableTelemetry(uint8_t);
    void _resetCells();
    void _updateStatCell(uint8_t row, uint8_t col, uint8_t level);
    void _updateStatCellDisplay(uint8_t row, uint8_t col);

private slots:
    void runButtonClick(void);
    void closeButtonClick(void);
    void loButtonClick(void);
    void hiButtonClick(void);
    void displayModeChanged(QString);

};
