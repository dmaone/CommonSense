/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define MATRIX_COLS 15
#define MATRIX_ROWS 15
#define MATRIX_LAYERS 4

// Specific to active matrix. There are technically 16 rows and column pins,
// but firmware doesn't support scancode 255 and one needs to be extra sure this
// scancode is not accidentally generated. Since there are no known microswitch
// matrices of > 13 columns, this is not even a problem, but..
#define MAX_PHYSICAL_ROWS 15

// Switch type: see globals.h for options
#define SWITCH_TYPE MICROSWITCH

// For a key with Base "A" + L1 "B": pressing the key and switching layers
// will release "A" and press "B". Default behavior is holding "A" till keyUp.
// #define REEVALUATE_ON_LAYER_CHANGE

/*
 * if SELF_POWERED is defined - the device is self/battery-powered.
 * What does this mean:
 * \USB:VBUS\ pin MUST be assigned to one of the SIO pins (P12[0]-P12[7])
 * That pin must be connected to a point which is connected to VBUS
 * but deenergized when USB is disconnected.
 * NOTE: having a diode between device power rail and USB is not enough.
 * You must add a resistor between VBUS and the ground on USB side.
 * I experimented with 10kOhm - but that's 0.5mA which will make achieving
 * USB standby compliance pretty hard.
 * I think 100kOhm will work just fine - although you _may_ have to
 * increase POWER_CHECK_DELAY in PSoC_USB.c
 *
 * Also, if your power source is <3.6V (No, Li-ion/LiPoly are not <3.6V!)
 * you MUST set USB_POWER_MODE to USB_3V_OPERATION.
 * If you don't - USB will likely not work.
 * If you do and power from 5V though - you'll most likely fry USB regulator.
 */
#define BUS_POWERED
#define USB_POWER_MODE USB_5V_OPERATION


