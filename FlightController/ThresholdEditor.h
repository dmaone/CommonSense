#ifndef THRESHOLDEDITOR_H
#define THRESHOLDEDITOR_H

#include <QFrame>
#include <QGridLayout>
#include <QSpinBox>
#include <stdint.h>
#include "../c2/nvram.h"

namespace Ui {
class ThresholdEditor;
}

class ThresholdEditor : public QFrame
{
    Q_OBJECT

public:
    explicit ThresholdEditor(QWidget *parent = 0);
    ~ThresholdEditor();
    void show(void);

public slots:
    void importThresholds(void);
    void applyThresholds(void);
    void resetThresholds(void);

signals:
    logMessage(QString);

private:
    Ui::ThresholdEditor *ui;
    QGridLayout *grid;
    QSpinBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    psoc_eeprom_t* deviceConfig;
    void initDisplay(uint8_t, uint8_t);
};

#endif // THRESHOLDEDITOR_H
