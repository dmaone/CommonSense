#include <QLabel>

#include "Delays.h"


DelayWatcher::DelayWatcher(DeviceConfig *config, int delayIndex, QSpinBox *box, QObject *parent):
    QObject(parent), config(config), delayIndex(delayIndex), box(box)
{
    connect(box, SIGNAL(valueChanged(int)), SLOT(changed(int)));
}

void DelayWatcher::changed(int delay_ms)
{
    config->setDelay(delayIndex, delay_ms);
}

Delays::Delays(DeviceConfig *config, QWidget *parent) : QWidget(parent), _config(config)
{
    _grid = new QGridLayout;
    setWindowTitle("Delays");
    setMinimumWidth(150);
}

Delays::~Delays()
{
    _deinit();
    delete _grid;
}


void Delays::_deinit()
{
    QLayoutItem *item;
    while ((item = _grid->takeAt(0)) != NULL)
    {
        delete item->widget();
        delete item;
    }

    for (size_t i = 0; i < delayWatchers.size(); i++)
        delete delayWatchers[i];
    delayWatchers.clear();
}

void Delays::init()
{
    _deinit();
    std::vector<uint16_t> delays = _config->delays();
    int count = delays.size();
    for (int i = 0; i < count; i++)
    {
        QLabel *rowLabel = new QLabel(i == 0 ? QString("Event delay") : QString("%1").arg(i));
        rowLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
        _grid->addWidget(rowLabel, i, 0);

        QSpinBox *delay = new QSpinBox;
        delay->setMaximum(UINT16_MAX);
        delay->setValue(delays[i]);
        _grid->addWidget(delay, i, 1);
        delayWatchers.push_back(new DelayWatcher(_config, i, delay, this));
    }
    setLayout(_grid);
    adjustSize();
}
