#ifndef DELAYS_H
#define DELAYS_H

#include <QWidget>
#include <QSpinBox>
#include <QGridLayout>

#include "DeviceConfig.h"

class DelayWatcher: public QObject
{
    Q_OBJECT

public:
    DelayWatcher(DeviceConfig *config, int delayIndex,
            QSpinBox *box, QObject *parent = NULL);
    uint16_t getValue(void);

private:
    DeviceConfig *config;
    int delayIndex;
    QSpinBox *box;

private slots:
    void changed(int delay_ms);
};

class Delays : public QWidget
{
    Q_OBJECT
public:
    std::vector<DelayWatcher *> delayWatchers;
    explicit Delays(DeviceConfig *config, QWidget *parent = 0);
    ~Delays();
    void init(void);

signals:

public slots:
private:
    DeviceConfig *_config;
    QGridLayout *_grid;
    void _deinit(void);

};

#endif // DELAYS_H
