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
    enum Mode {
        Off, Voltages, States
    };
    Q_ENUM(Mode)
    void setMode(Mode);

signals:
    void notifyDevice(QByteArray msg);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::MatrixMonitor *ui;
    uint8_t rows;
    uint8_t cols;
    //TODO fix crash on close by moving to pointers to objects?
    // crash is in QScopedArrayPointer deleter.
    QScopedArrayPointer<QLCDNumber> display[32][16];
    QScopedPointer<QGridLayout> grid;


private slots:
    void voltagesButtonClick(void);
    void statesButtonClick(void);
    void closeButtonClick(void);
    void freezeButtonClick(void);

};
