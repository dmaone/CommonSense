/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. 
*/
#include <QApplication>
#include "FlightController.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FlightController w;
    w.show();

    return a.exec();
}
