#pragma once

#include <QTCore>
#include <QGridLayout>
#include <QLCDNumber>
#include <stdint.h>
#include "Events.h"
#include "../c2/c2_protocol.h"

namespace Ui {
class MatrixMonitor;
}

class MatrixMonitor : public QFrame
{
    Q_OBJECT

public:
    explicit MatrixMonitor(uint8_t rows, uint8_t cols, QWidget *parent = 0);
    ~MatrixMonitor();
    void updateDisplaySize(uint8_t, uint8_t);

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
    void initDisplay(void);
    void enableOutput(uint8_t);

private slots:
    void voltagesButtonClick(void);
    void closeButtonClick(void);
    void freezeButtonClick(void);
    void displayModeChanged(QString);

};
