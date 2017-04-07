#include "LayerConditions.h"
#include <QLabel>
#include <QComboBox>

using namespace std;

LayerConditionWatcher::LayerConditionWatcher(DeviceConfig *config,
        int conditionIndex, QCheckBox *fn1Check, QCheckBox *fn2Check,
        QCheckBox *fn3Check, QCheckBox *fn4Check, QComboBox *layerCombo,
        QObject *parent):
    QObject(parent),
    config(config),
    conditionIndex(conditionIndex),
    fn1Check(fn1Check),
    fn2Check(fn2Check),
    fn3Check(fn3Check),
    fn4Check(fn4Check),
    layerCombo(layerCombo)
{
    connect(fn1Check, SIGNAL(toggled(bool)), SLOT(changed(void)));
    connect(fn2Check, SIGNAL(toggled(bool)), SLOT(changed(void)));
    connect(fn3Check, SIGNAL(toggled(bool)), SLOT(changed(void)));
    connect(fn4Check, SIGNAL(toggled(bool)), SLOT(changed(void)));
    connect(layerCombo, SIGNAL(currentIndexChanged(int)), SLOT(changed(void)));
}

LayerCondition LayerConditionWatcher::toLayerCondition(void)
{
    return LayerCondition(fn1Check->isChecked(), fn2Check->isChecked(),
            fn3Check->isChecked(), fn4Check->isChecked(), layerCombo->currentIndex());
}

void LayerConditionWatcher::changed(void)
{
    config->setLayerCondition(conditionIndex, toLayerCondition());
}

LayerConditions::LayerConditions(DeviceConfig *config, QWidget *parent) : QWidget(parent), config(config)
{
    grid = new QGridLayout;
    setLayout(grid);
    init();
}

void LayerConditions::init(void)
{
    vector<LayerCondition> cnds = config->layerConditions();
    // Destroy
    QLayoutItem *item;
    while ((item = grid->takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }

    for (size_t i = 0; i < layerConditionWatchers.size(); i++)
        delete layerConditionWatchers[i];
    layerConditionWatchers.clear();

    int count = cnds.size();
    for (int i = 0; i < NUM_FNS; i++)
    {
        QLabel *fnLabel = new QLabel(QString("<b>Fn%1</b>").arg(i+1));
        grid->addWidget(fnLabel, 0, i + 2);
    }
    for (int i = 0; i < count; i++)
    {
        QLabel *rowLabel = new QLabel(QString("<b>%1</b>").arg(i+1));
        grid->addWidget(rowLabel, i + 1, 1);

        QCheckBox *fn1Check = new QCheckBox;
        fn1Check->setChecked(cnds[i].fn1Set());

        QCheckBox *fn2Check = new QCheckBox;
        fn2Check->setChecked(cnds[i].fn2Set());

        QCheckBox *fn3Check = new QCheckBox;
        fn3Check->setChecked(cnds[i].fn3Set());

        QCheckBox *fn4Check = new QCheckBox;
        fn3Check->setChecked(cnds[i].fn4Set());

        QLabel *arrowLabel = new QLabel(QChar(0x2192));

        QComboBox *layerCombo = new QComboBox();
        for (int j = 0; j < config->numLayers; j++)
            layerCombo->addItem(j == 0 ? "Base Layer" : QString("Layer %1").arg(j), j);
        layerCombo->setCurrentIndex(cnds[i].layer());

        grid->addWidget(fn1Check, i + 1, 2);
        grid->addWidget(fn2Check, i + 1, 3);
        grid->addWidget(fn3Check, i + 1, 4);
        grid->addWidget(fn4Check, i + 1, 5);
        grid->addWidget(arrowLabel, i + 1, 6);
        grid->addWidget(layerCombo, i + 1, 7);

        layerConditionWatchers.push_back(new LayerConditionWatcher(config,
                    i, fn1Check, fn2Check, fn3Check, fn4Check, layerCombo, this));
    }

    grid->setRowStretch(grid->rowCount(), 1);
    grid->setColumnStretch(0, 4);
    for (int i = 1; i < grid->columnCount(); i++)
        grid->setColumnStretch(i, 1);
    grid->setColumnStretch(grid->columnCount(), 4);
    setWindowTitle("Layer Mods");

}
