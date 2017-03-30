#ifndef LAYERCONDITIONS_H
#define LAYERCONDITIONS_H

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include "DeviceConfig.h"

#define NUM_FNS 4

class LayerConditionWatcher: public QObject
{
    Q_OBJECT

public:
    LayerConditionWatcher(DeviceConfig *config, int conditionIndex,
            QCheckBox *fn1Check, QCheckBox *fn2Check, QCheckBox *fn3Check,
            QCheckBox *fn4Check, QComboBox *layerCombo, QObject *parent = NULL);
    LayerCondition toLayerCondition(void);

private:
    DeviceConfig *config;
    int conditionIndex;
    QCheckBox *fn1Check;
    QCheckBox *fn2Check;
    QCheckBox *fn3Check;
    QCheckBox *fn4Check;
    QComboBox *layerCombo;


private slots:
    void changed(void);
};

class LayerConditions : public QWidget
{
    Q_OBJECT
public:
    std::vector<LayerConditionWatcher *> layerConditionWatchers;
    LayerConditions(DeviceConfig *config, QWidget *parent = 0);
    void init(void);

signals:

public slots:

private:
    DeviceConfig *config;
    QGridLayout *grid;
};

#endif // LAYERCONDITIONS_H
