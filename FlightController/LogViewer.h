#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QPlainTextEdit>
#include "Events.h"
//#include <QWidget>

class LogViewer: public QPlainTextEdit
{
    Q_OBJECT

    public:
        LogViewer(QWidget *parent = NULL);
        bool event(DeviceMessage* e);

    public slots:
        void clearButtonClick(void);
        void copyAllButtonClick(void);
        void logMessage(QString msg);

};

#endif // LOGVIEWER_H
