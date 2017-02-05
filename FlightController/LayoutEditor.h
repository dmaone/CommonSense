#ifndef LAYOUTEDITOR_H
#define LAYOUTEDITOR_H

#include <QFrame>
#include <QGridLayout>
#include <QComboBox>
#include <stdint.h>
#include "../c2/nvram.h"

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

signals:
    void logMessage(QString);

private:
    Ui::LayoutEditor *ui;
    QGridLayout *grid;
    QComboBox *display[ABSOLUTE_MAX_ROWS][ABSOLUTE_MAX_COLS];
    psoc_eeprom_t* deviceConfig;
    void initDisplay(uint8_t, uint8_t);
};

#endif // LAYOUTEDITOR_H
