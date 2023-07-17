/*
 *
 * Copyright (C) 2018-present DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

// Predefined hardware config customizations
#define HW_CUSTOM 0
#define HW_FSSK 1
#define HW_F122 2
#define HW_BEAMSPRING 3
#define HW_DISPLAYWRITER 4
#define HW_REDRAGON 5
#define HW_EPSON 6

#define HARDWARE_VARIANT HW_FSSK

// <DEFAULTS>
// NOTE: MATRIX_ROWS and MATRIX_COLS MUST NOT HAVE DEFAULTS!
//       They also MUST EXACTLY MATCH physical matrix size (esp. beamspring!)

// Switch type: see globals.h for options
#define SWITCH_TYPE BUCKLING_SPRING

// Switches hanging off dedicated GPIO pins. Pedals or something.
// Non-zero value requires FSW0 status register defined.
// See uSwitch_gated\TopDesign.cysch for implementation details.
#define NUM_PEDALS 0

#define MATRIX_LAYERS 4

// </DEFAULTS>

#if HARDWARE_VARIANT == HW_CUSTOM
// Define your own variant, not touching any standard configurations
#define MATRIX_COLS 16
#define MATRIX_ROWS 8

// Some vintage keyboards have inverted signalling - 74xx are better at sinking
// current (less heat dissipation). This switches ExpHdr to be normally high.
// NOTE: This does NOT invert solenoid - you need INVERT_FIRE for that.
#define INVERT_EXPHDR

// Pull down on "solenoid" "firing"
#define INVERT_FIRE

#elif HARDWARE_VARIANT == HW_FSSK
#define MATRIX_COLS 12
#define MATRIX_ROWS 8

#elif HARDWARE_VARIANT == HW_F122
#define MATRIX_COLS 16
#define MATRIX_ROWS 8

#elif HARDWARE_VARIANT == HW_BEAMSPRING
#define SWITCH_TYPE BEAMSPRING
#define MATRIX_COLS 23
#define MATRIX_ROWS 4
  
#elif HARDWARE_VARIANT == HW_DISPLAYWRITER
#define SWITCH_TYPE BEAMSPRING
#define MATRIX_COLS 12
#define MATRIX_ROWS 8

#elif HARDWARE_VARIANT == HW_REDRAGON
#define MATRIX_COLS 20
#define MATRIX_ROWS 8
#define INVERT_EXPHDR
#define INVERT_FIRE

#elif HARDWARE_VARIANT == HW_EPSON
#define MATRIX_COLS 23
#define MATRIX_ROWS 5

#else
#error "Unknown hardware variant"
#endif

#if MATRIX_COLS > 24
#error "Max. 24 columns supported!"
#endif

#if MATRIX_ROWS > 8
#error "Max. 8 rows supported!"
#endif


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


