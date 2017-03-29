#ifndef LAYOUTEDITOR_H
#define LAYOUTEDITOR_H

#include <QFrame>
#include <QGridLayout>
#include <QComboBox>
#include <stdint.h>
#include "../c2/c2_protocol.h"
#include "DeviceConfig.h"

namespace Ui {
class LayoutEditor;
}

class LayoutEditor : public QFrame
{
    Q_OBJECT

public:
    explicit LayoutEditor(QWidget *parent = 0);
    ~LayoutEditor();
    void show(void);

public slots:
    void importLayout(void);
    void exportLayout(void);
    void applyLayout(void);
    void resetLayout(void);
    void switchLayer(void);

private:
    Ui::LayoutEditor *ui;
    QGridLayout *grid;
    QComboBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    DeviceConfig *deviceConfig;
    uint8_t currentLayer;
    void initDisplay(uint8_t, uint8_t);
    void setDisplay();
};

#endif // LAYOUTEDITOR_H
