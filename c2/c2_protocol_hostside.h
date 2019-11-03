/*
 *
 * Copyright (C) 2016 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#pragma once

#include "c2_protocol.h"

#include <stdint.h>
#include <string>
#include <vector>


const std::vector<std::string> expModeNames{
    "Disabled",

    "Solenoid+Num+Caps", "Lock LEDs",
};

const std::vector<std::string> switchTypeNames{
  "Beamspring", "Buckling Spring", "ADB", "Sun", "MagValve", "UNKNOWN"
};
constexpr uint8_t switchTypeCount{5};
