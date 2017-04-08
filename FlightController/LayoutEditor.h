#ifndef LAYOUTEDITOR_H
#define LAYOUTEDITOR_H

#include <QFrame>
#include <QGridLayout>
#include <QComboBox>
#include <stdint.h>
#include "../c2/c2_protocol.h"
#include "DeviceConfig.h"
#include "DeviceInterface.h"

#define LAYOUTS_DIR_KEY "settings_dir"

namespace Ui {
class LayoutEditor;
}

class LayoutEditor : public QFrame
{
    Q_OBJECT

public:
    explicit LayoutEditor(DeviceConfig *config, QWidget *parent = 0);
    ~LayoutEditor();
    void show(void);

public slots:
    void importLayout(void);
    void exportLayout(void);
    void applyLayout(void);
    void resetLayout(void);
    void switchLayer(void);
    void receiveScancode(uint8_t row, uint8_t col, DeviceInterface::KeyStatus status);

private:
    Ui::LayoutEditor *ui;
    QGridLayout *grid;
    QComboBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    DeviceConfig *deviceConfig;
    uint8_t currentLayer;
    void initDisplay(void);
    void sizeDisplay(uint8_t, uint8_t);
    void setDisplay();
};

#endif // LAYOUTEDITOR_H
