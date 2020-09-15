/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "Events.h"
#include <QtCore>

const QEvent::Type DeviceMessage::ET =
    static_cast<QEvent::Type>(QEvent::registerEventType());


