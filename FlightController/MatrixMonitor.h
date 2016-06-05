#pragma once

#include <QTCore>
#include <QGridLayout>
#include <QLCDNumber>
#include <stdint.h>
#include "Events.h"

namespace Ui {
class MatrixMonitor;
}

class MatrixMonitor : public QFrame
{
    Q_OBJECT

public:
    explicit MatrixMonitor(uint8_t cols, uint8_t rows, QWidget *parent = 0);
    ~MatrixMonitor();

signals:
    void sendCommand(uint8_t cmd, uint8_t msg);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *);

private:
    Ui::MatrixMonitor *ui;
    uint8_t rows;
    uint8_t cols;
    uint8_t debug;
    uint8_t displayMode;
    QGridLayout *grid;
    QLCDNumber *display[32][16];
    void initDisplay(void);
    void enableOutput(uint8_t);

private slots:
    void voltagesButtonClick(void);
    void closeButtonClick(void);
    void freezeButtonClick(void);
    void displayModeChanged(QString);

};
