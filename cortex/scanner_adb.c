/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <project.h>

#include "scan.h"

#define ADDR_KEYBOARD 0x20

typedef union {
  struct {
    uint8_t key0;
    uint8_t key1;
  };
  uint16_t raw;
} adb_pdu_t;

uint8_t local_led_status;

static inline void place_bit0() {
  ADB_Data_Write(0);
  CyDelayUs(65);
  ADB_Data_Write(1);
  CyDelayUs(35);
}

static inline void place_bit1() {
  ADB_Data_Write(0);
  CyDelayUs(35);
  ADB_Data_Write(1);
  CyDelayUs(65);
}

static inline uint16_t wait_data_lo(uint16_t us) {
  do {
    if (!ADB_Data_Read()) {
      break;
    }
    CyDelayUs(1);
  } while ( --us );
  return us;
}

static inline uint16_t wait_data_hi(uint16_t us) {
  do {
    if (ADB_Data_Read()) {
      break;
    }
    CyDelayUs(1);
  } while ( --us );
  return us;
}

static inline void place_byte(uint8_t data) {
  for (uint8_t i=0; i < 8; i++) {
    if (data & (0x80 >> i)) {
      place_bit1();
    } else {
      place_bit0();
    }
  }
}

static inline void attention(void)
{
  ADB_Data_Write(0);
  CyDelayUs(800-35); // bit1 holds lo for 35 more
  place_bit1();
}

void adb_host_init() {
// Protocol violation - we're supposed to scan the bus first 
// by querying register 3
// and memorize the device addresses they assigned themselves.
//    uint16_t res;
//    res = adb_host_dev_recv(ADDR_KEYB, 0x3);
}

void adb_host_listen(uint8_t cmd, uint8_t data_h, uint8_t data_l)
{
    attention();
    place_byte(cmd);
    place_bit0();               // Stopbit(0)
    CyDelayUs(200);             // Tlt/Stop to Start
    place_bit1();               // Startbit(1)
    place_byte(data_h); 
    place_byte(data_l);
    place_bit0();               // Stopbit(0);
}

uint16_t adb_host_dev_recv(uint8_t device, uint8_t reg) {
  uint16_t data = 0;
  attention();
  place_byte(device|0x0C|reg);     // Addr:Keyboard(0010)/Mouse(0011), Cmd:Talk(11), Register0(00)
  place_bit0();               // Stopbit(0)
  if (!wait_data_hi(500)) {    // Service Request(310us Adjustable Keyboard): just ignored
    return -30;             // something wrong
  }
  if (!wait_data_lo(500)) {   // Tlt/Stop to Start(140-260us)
    return 0;               // No data to send
  }
  
  uint8_t n = 17; // start bit + 16 data bits
  do {
    uint8_t lo = (uint8_t) wait_data_hi(130);
    if (!lo)
      goto error;
    
    uint8_t hi = (uint8_t) wait_data_lo(lo);
    if (!hi)
      goto error;
    
    hi = lo - hi;
    lo = 130 - lo;

    data <<= 1;
    if (lo < hi) {
      data |= 1;
    }
    else if (n == 17) {
      return -20;
    }
  }
  while ( --n );

  // Stop bit can't be checked normally since it could have service request lenghtening
  // and its high state never goes low.
  if (!wait_data_hi(351) || wait_data_lo(91)) {
    return -21;
  }
  return data;

error:
  return -n;
}

void sync_leds(void) {
  CyDelayUs(12);
  adb_host_listen(0x2A, 0, (~led_status) & 0x07);
  local_led_status = led_status;
}

void scan_init(uint8_t debouncing_period) {
  scan_common_init(debouncing_period);
  ADB_Data_Write(1);
  CyDelayUs(1000);
  adb_host_init();
  CyDelayUs(100);
  // Enable keyboard left/right modifier distinction
  // Addr:Keyboard(0010), Cmd:Listen(10), Register3(11)
  // upper byte: reserved bits 0000, device address 0010
  // lower byte: device handler 00000011
  adb_host_listen(0x2B,0x02,0x03);
}

void scan_reset(void) {
  scan_common_reset();
}

void scan_start(void) {
  sync_leds();
}

void scan_nap(void) {
}

void scan_wake(void) {
}

void scan_tick(void) {
  CyDelayUs(12); //delay for preventing overload of poor ADB keyboard controller
  adb_pdu_t codes;
  codes.raw = adb_host_dev_recv(ADDR_KEYBOARD, 0);
  if (codes.key0 == codes.key1) {
    switch (codes.key0) {
      case 0x7f:
        // intentional fallthru
      case 0xff:
        // power key. Subtract one not to clash with COMMONSENSE_NO_KEY
        append_scancode(codes.key0 & KEY_UP_MASK, (codes.key0 & SCANCODE_MASK) - 1);
        break;
      default:
        break;
    }
  } else if (codes.key1 == 0xFF) {
    xprintf("ADB Error: received %x", codes.raw);
    return;
  } else {
    xprintf("%02x %02x", codes.key0, codes.key1);
    append_scancode(codes.key1 & KEY_UP_MASK, (codes.key1 & SCANCODE_MASK));
    if (codes.key0 != 0xFF) {
      append_scancode(codes.key0 & KEY_UP_MASK, (codes.key0 & SCANCODE_MASK));
    }
  }
  if (local_led_status != led_status) {
    sync_leds();
  }
  scan_common_tick();
}
