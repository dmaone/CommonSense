#ifndef THRESHOLDEDITOR_H
#define THRESHOLDEDITOR_H

#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include <QCheckBox>
#include "DeviceConfig.h"
#include "DeviceInterface.h"

namespace Ui {
class ThresholdEditor;
}

class ThresholdEditor : public QFrame
{
    Q_OBJECT

public:
    explicit ThresholdEditor(DeviceConfig *config, QWidget *parent = 0);
    ~ThresholdEditor();
    void show(void);

public slots:
    void applyThresholds(void);
    void resetThresholds(void);
    void updateLows(void);
    void updateHighs(void);
    void receiveScancode(uint8_t row, uint8_t col, DeviceInterface::KeyStatus status);

private:
    Ui::ThresholdEditor *ui;
    QGridLayout *grid;
    QWidget *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    QCheckBox *skip[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    QSpinBox *lo[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    QSpinBox *hi[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    DeviceConfig *deviceConfig;
    void initDisplay();
    void updateDisplaySize(uint8_t, uint8_t);

};

#endif // THRESHOLDEDITOR_H
