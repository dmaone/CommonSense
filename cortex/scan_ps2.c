/*
 *
 * Copyright (C) 2018 DMA <dma@ya.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * NOTES
   * Counter is out of phase with shift register on purpose, to avoid read race condition.
   * Glitch filter isn't strictly needed, but I think I've seen some glitches, so..
 *
 * SPECS USED:
   * Spec1: "Keyboard/Auxiliary Device Controller - October 1990"
     http://www.mcamafia.de/pdf/ibm_hitrc07.pdf
   * Spec2: http://pcbheaven.com/wikipages/The_PS2_protocol/
   * Spec3: http://www-ug.eecg.toronto.edu/msl/nios_devices/datasheets/PS2%20Keyboard%20Protocol.htm
 */
#include <project.h>

#include "gpio.h" // For LED masks
#include "scan.h"

#define PS2_PACKET_LENGTH 11
#define CLK_WAIT_STEP_US 1
#define MAX_CLK_WAIT_US 125
#define MAX_SEND_US 800

// AT commands - AT keyboards supposed to understand those
#define PS2_CMD_SET_LEDS 0xED
#define PS2_CMD_SET_TYPEMATIC_RATE 0xF3
#define PS2_CMD_RESET 0xFF

// PS/2 commands - only keyboards supporting scancode set 3 are supposed to understand those. Spec3 says not all do!
#define PS2_CMD_SET_SCANCODE_SET 0xF0
#define PS2_CMD_SET_KEYS_MAKE_BREAK 0xF8

#define PS2_SLOWEST_TYPEMATIC 0x7F

uint8_t local_led_status;

/*
 * Scancode translation tables. Translation is to USB scancodes (roughly - see FlightController/ScancodeList.cpp)
 * Since keyboard can't be 16x16 - scancodes F0..FF cannot be represented and must be remapped.
 * So, F0..F7 -> A8..AF, and F7..FF are unmapped, beware.
 * FF is a special code meaning "ignore" - used for handling "shift" and "numlock" modes.
 * Apparently, PrtSc is E0 12 E0 7C, shifted is E0 7C, alted is 84. USB HID code is 46
 * Break is famously E1 14 77 E1 F0 14 F0 77, Ctrl'd is E0 7E (with normal break sequence E0 F0 7E) -> 48
 * 5D is 0x31 and 0x32
 * Missing mappings:
  * Media select: E0 50
  * Mail: E0 48
  * Calculator: E0 2B
  * My Computer: E0 40
  * WWW Search: E0 10
  * WWW Home: E0 3A
  * WWW Back: E0 38
  * WWW Forward: E0 30
  * WWW Stop: E0 28
  * WWW Refresh: E0 20
  * WWW Favorites: E0 18
  * 

 */


// Normal table
static uint8_t PS2_SCANCODE2_TABLE[256] = {
 // 0     1     2     3      4     5     6     7      8     9     A     B      C     D     E     F
 0x00, 0x42, 0x00, 0x3e,  0x3c, 0x3a, 0x3b, 0x45,  0x68, 0x43, 0x41, 0x3f,  0x3d, 0x2b, 0x35, 0x67,
 0x69, 0xe2, 0xe1, 0x88,  0xe0, 0x14, 0x1e, 0x00,  0x6a, 0x00, 0x1d, 0x16,  0x04, 0x1a, 0x1f, 0x00,
 0x6b, 0x06, 0x1b, 0x07,  0x08, 0x21, 0x20, 0x8c,  0x6c, 0x2c, 0x19, 0x09,  0x17, 0x15, 0x22, 0x00,
 0x6d, 0x11, 0x05, 0x0b,  0x0a, 0x1c, 0x23, 0x00,  0x6e, 0x00, 0x10, 0x0d,  0x18, 0x24, 0x25, 0x00,

 //40    41    42    43     44    45    46    47     48    49    4A    4B     4C    4D    4E    4F
 0x6f, 0x36, 0x0e, 0x0c,  0x12, 0x27, 0x26, 0x00,  0x70, 0x37, 0x38, 0x0f,  0x33, 0x13, 0x2d, 0x00,
 0x71, 0x87, 0x34, 0x00,  0x2f, 0x2e, 0x00, 0x72,  0x39, 0xe5, 0x28, 0x30,  0x00, 0x31, 0x00, 0x94,
 0x00, 0x64, 0x93, 0x92,  0x8a, 0x00, 0x2a, 0x8b,  0x00, 0x59, 0x89, 0x5c,  0x5f, 0x85, 0x00, 0x00,
 0x62, 0x63, 0x5a, 0x5d,  0x5e, 0x60, 0x29, 0x53,  0x44, 0x57, 0x5b, 0x56,  0x55, 0x61, 0x47, 0x00,

 //80    81    82    83     84    85    86    87     88    89    8A    8B     8C    8D    8E    8F
 0x00, 0x00, 0x00, 0x40,  0x57, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,

 //C0    C1    C2    C3     C4    C5    C6    C7     C8    C9    CA    CB     CC    CD    CE    CF
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x91, 0x90, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
 //F0    F1    F2    F3     F4    F5    F6    F7     F8    F9    FA    FB     FC    FD    FE    FF
};

// E0 table
static uint8_t PS2_SCANCODE2_E0_TABLE[256] = {
 // 0     1     2     3      4     5     6     7      8     9     A     B      C     D     E     F
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0xe6, 0xff, 0xe9,  0xe4, 0xae, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0xe3,
 0x00, 0xeb, 0x00, 0x00,  0xe8, 0x00, 0x00, 0xe7,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x65,
 0x00, 0x00, 0xea, 0x00,  0x00, 0x00, 0x00, 0xa5,  0x00, 0x00, 0x00, 0xaf,  0x00, 0x00, 0x00, 0xa6,

 //40    41    42    43     44    45    46    47     48    49    4A    4B     4C    4D    4E    4F
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x54, 0x00,  0x00, 0xad, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0xff, 0x58, 0x00,  0x00, 0x00, 0xa7, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x4d, 0x00, 0x50,  0x4a, 0x00, 0x00, 0x00,
 0x49, 0x4c, 0x51, 0x00,  0x4f, 0x52, 0x00, 0x00,  0x00, 0x00, 0x4e, 0x00,  0x46, 0x4b, 0x48, 0x00,

 //80    81    82    83     84    85    86    87     88    89    8A    8B     8C    8D    8E    8F
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,

 //C0    C1    C2    C3     C4    C5    C6    C7     C8    C9    CA    CB     CC    CD    CE    CF
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
 //F0    F1    F2    F3     F4    F5    F6    F7     F8    F9    FA    FB     FC    FD    FE    FF
};

#define USBHID_PAUSE 0x48
static uint8_t PS2_SCANCODE_SET_2_PAUSE[8] = {0xe1, 0x14, 0x77, 0xe1, 0xf0, 0x14, 0xf0, 0x77};

CY_ISR_PROTO(PS2CLK_ISR);

#define QUEUE_SIZE 64
// ^ MUST BE POWER OF 2!

#define BUF_NEXT(X) ((X + 1) & (QUEUE_SIZE - 1))
volatile static uint8_t event_queue[QUEUE_SIZE];
static uint8_t q_begin;
volatile static uint8_t q_end;
volatile static bool request_to_send = false;
static enum {
  BUS_INIT,
  BUS_DETECTED,
  BUS_RESETTING,
  BUS_CONFIGURING,
  BUS_XT,
  BUS_AT,
  BUS_PS2
} bus_state = BUS_INIT;

inline void PS2_BitCounter_Reload() {
  PS2_BitCounter_WriteCounter(PS2_BitCounter_ReadPeriod()); // Because there's no PS2_BitCounter_Reload() :(
  PS2_BitCounter_Start();
}

inline uint16_t ps2_clk_wait(uint8_t what) {
  CyDelayUs(CLK_WAIT_STEP_US);
  uint16_t waited = CLK_WAIT_STEP_US;
  while (PS2_CLK_Read() != what) {
    CyDelayUs(CLK_WAIT_STEP_US);
    waited += CLK_WAIT_STEP_US;
    if (waited > MAX_CLK_WAIT_US) {
      return MAX_SEND_US;
    }
  }
  return waited;
}

void ps2_detect() {
  // Detects if keyboard is physically present
  if (bus_state >= BUS_DETECTED) {
    return;
  }
  // Pull-up resistors are part of the keyboard.
  // If both CLK and DATA are low - likely, keyboard isn't even connected.
  if (PS2_CLK_Read() || PS2_Data_Read()) {
    bus_state = BUS_DETECTED;
  }
}

inline void ps2_inhibit() {
  // NOTE: hardware auto-inhibits CLK after receiving a byte.
  // But we must drive that initial reset, so..
  PS2_CLK_En_Write(0); // Pin CLK low to prevent keyboard from sending
}

inline void ps2_listen() {
  PS2_CLK_En_Write(1); // Release CLK so keyboard can send
}

bool ps2_send(uint8_t byte) {
  // Send a byte to keyboard
  uint8_t ack = 1; // wrong answer - should be zero if ACK is received.
  if (PS2_CLK_En_Read()) {
    if (PS2_BitCounter_ReadCounter() < PS2_PACKET_LENGTH) {
      request_to_send = true;
      while(request_to_send) {
        CyDelayUs(100); // Wait for the end of transmission
      }
    }
  }
  PS2_BitCounter_Stop();
  uint8_t enableInterrupts = CyEnterCriticalSection();
  ps2_inhibit();
  CyDelayUs(60);
  CyPins_ClearPin(PS2_Data_0); // Offer data
  CyDelayUs(5); // Not strictly needed, but makes oscillograms more legible
  ps2_listen();
  ps2_clk_wait(1);
  // Wait for keyboard to generate clock pulses. It can take more than 50us.
  uint16_t elapsed_us = ps2_clk_wait(0);
  while (elapsed_us == MAX_SEND_US) {
    elapsed_us = ps2_clk_wait(0);
  }
  bool parity = true;
  // Send data
  for (uint8_t i = 0; i < 8; i++) {
    if (byte & (1 << i)) {
      parity = !parity;
      CyPins_SetPin(PS2_Data_0);
    } else {
      CyPins_ClearPin(PS2_Data_0);
    }
    elapsed_us += ps2_clk_wait(1);
    elapsed_us += ps2_clk_wait(0);
    if (elapsed_us >= MAX_SEND_US) {
      break;
    }
  }
  // Send parity
  if (elapsed_us < MAX_SEND_US) {
    if (parity) {
      CyPins_SetPin(PS2_Data_0);
    } else {
      CyPins_ClearPin(PS2_Data_0);
    }
    elapsed_us += ps2_clk_wait(1);
    elapsed_us += ps2_clk_wait(0);
  }
  uint16_t elapsed_release = elapsed_us;
  CyPins_SetPin(PS2_Data_0); // Release the bus - this must be guaranteed!
  if (elapsed_us < MAX_SEND_US) {
    // Wait for stop bit to be read
    elapsed_us += ps2_clk_wait(1);
  }
  if (elapsed_us < MAX_SEND_US) {
    elapsed_us += ps2_clk_wait(0);
    ack = PS2_Data_Read(); // Read the ACK bit after CLK is pulled down
    elapsed_us += ps2_clk_wait(1);  // Spec1 p15 diagram hints we must wait!
  }

  ps2_inhibit();

  CyExitCriticalSection(enableInterrupts);
  PS2_BitCounter_Reload();
  ps2_listen();
  if (ack) {
    xprintf("Sent %x. Elapsed us: %d/%d, ack? %d", byte, elapsed_release, elapsed_us, ack);
  }
  return (elapsed_us < MAX_SEND_US) && !ack;

}

void ps2_reset() {
  if (bus_state == BUS_RESETTING) {
    return;
  }
  if (ps2_send(PS2_CMD_RESET)) {
    bus_state = BUS_RESETTING;
  }
  bus_state = BUS_RESETTING;
}

void sync_leds(void) {
  // Remap USB LEDs to PS/2 LEDs.
  uint8_t leds_translated = 0;
  if (led_status & LED_SCRLOCK_MASK) {
    leds_translated += 0x01;
  }
  if (led_status & LED_NUMLOCK_MASK) {
    leds_translated += 0x02;
  }
  if (led_status & LED_CAPSLOCK_MASK) {
    leds_translated += 0x04;
  }

  ps2_send(PS2_CMD_SET_LEDS);
  ps2_send(leds_translated);
  local_led_status = led_status;
}

void scan_init(uint8_t debouncing_period) {
  scan_common_init(debouncing_period);
  PS2_BitCounter_Start();
  PS2_Buffer_Start();
  ps2_listen();
  CyDelayUs(10); // CRITICAL: Ensure BitCounter TC has time to turn off
  PS2CLK_IRQ_ClearPending();  // CRITICAL: prevent IRQ immediately firing!
  PS2CLK_IRQ_StartEx(PS2CLK_ISR);
  ps2_detect();
}

void scan_reset(void) {
  scan_common_reset();
}

void scan_start(void) {
  scan_common_start(1); // No point sanity-checking.
}

void scan_nap(void) {
}

void scan_wake(void) {
}

void send_keypress(uint8_t sc) {
  scan_register_event((sc & 0x80) ? KEY_UP_MASK : 0, sc & 0x7f);
}

void scan_tick(void) {
  static enum {
    INIT,
    READY,
    E0,
    E0_F0,
    F0,
    E1
  } state = INIT;
  static uint8_t pause_index = 0;
  /*
  if (systime == 500) {
    // NOTE: scancode set 3 can be weird - like, gateway AnyKey in scancode set 3 doesn't output diagonal arrows AT ALL.
    ps2_send(PS2_CMD_SET_SCANCODE_SET);
    ps2_send(0x03);
  }
  if (systime == 1500) {
    ps2_send(PS2_CMD_SET_KEYS_MAKE_BREAK);
  }
  */
  if (bus_state >= BUS_AT && (local_led_status != led_status)) {
    sync_leds();
  }
  while (q_begin != q_end) {
    q_begin = BUF_NEXT(q_begin);
    uint8_t scancode = event_queue[q_begin];
    // xprintf("rcvd %x@%d", scancode, systime);
    switch (state) {
      case INIT: { // Receiving first byte from keyboard that was silent before.
        if (bus_state < BUS_AT && scancode != 0xaa) {
          // If anything but "BAT success" - set slowest typematic rate there is
          bus_state = BUS_CONFIGURING;
          ps2_send(PS2_CMD_SET_TYPEMATIC_RATE);
          ps2_send(PS2_SLOWEST_TYPEMATIC);
          state = READY;
          // intentional fall through!
        }
      }
      case READY: {
        switch (scancode) {
          case 0xaa:
            // BAT success
            xprintf("BAT successful");
            bus_state = BUS_CONFIGURING;
            ps2_send(PS2_CMD_SET_TYPEMATIC_RATE);
            ps2_send(PS2_SLOWEST_TYPEMATIC);
            break;
          case 0xe0:
            state = E0;
            break;
          case 0xe1:
            state = E1;
            pause_index = 1;
            break;
          case 0xf0:
            state = F0;
            break;
          case 0xfa: // ACK
            if (bus_state == BUS_CONFIGURING) {
              xprintf("Keyboard configured!");
              bus_state = BUS_AT;
            }
            break;
          default: {
            uint8_t resolved = PS2_SCANCODE2_TABLE[scancode];
            if (resolved) {
              scan_register_event(0, resolved);
            } else {
              xprintf("Unknown scancode: %02X", scancode);
            }
          }
        }
        break;
      }
      case E0: { // Use alt table
        if (scancode == 0xf0) {
          state = E0_F0;
        } else {
          uint8_t resolved = PS2_SCANCODE2_E0_TABLE[scancode];
          switch(resolved) {
            case 0:
              xprintf("Unknown scancode: E0 %02X", scancode);
              break;
            case 0xff:
              break;
            default:
            scan_register_event(0, resolved);
          }
          state = READY;
        }
        break;
      }
      case F0: { // Break code
        uint8_t resolved = PS2_SCANCODE2_TABLE[scancode];
        if (resolved) {
          scan_register_event(KEY_UP_MASK, resolved);
        } else {
          xprintf("Unknown scancode: F0 %02X", scancode);
        }
        state = READY;
        break;
      }
      case E0_F0:{ // Break code, alt table
        uint8_t resolved = PS2_SCANCODE2_E0_TABLE[scancode];
        switch(resolved) {
          case 0:
            xprintf("Unknown scancode: E0 F0 %02X", scancode);
            break;
          case 0xff:
            break;
          default:
            scan_register_event(KEY_UP_MASK, resolved);
        }
        state = READY;
        break;
      }
      case E1: { // The infamous PAUSE
        if (scancode != PS2_SCANCODE_SET_2_PAUSE[pause_index]) {
          xprintf("Wrong pause sequence at %d: expected %02X, got %02X", pause_index, PS2_SCANCODE_SET_2_PAUSE[pause_index], scancode);
          state = READY;
          break;
        }
        if (++pause_index >= 8) {
          scan_register_event(0, USBHID_PAUSE);
          scan_register_event(KEY_UP_MASK, USBHID_PAUSE); // PAUSE doesn't have a break code in scancode sets 1 or 2!
          state = READY;
        }
        break;
      }
    }
  }
  scan_common_tick();
}

CY_ISR(PS2CLK_ISR) {
  uint16_t data = PS2_Buffer_ReadRegValue() >> 5;
  if (data & 0x01) {
    xprintf("Start bit is set! %x@%d", data, systime);
  } else if (!(data & (1 << 10))) {
    xprintf("Stop bit is not set! %x@%d", data, systime);
  } else if (__builtin_parity(data & 0x01fe) == ((data >> 9) & 0x01) ) {
    xprintf("Parity error: %d vs %d %x@%d", __builtin_parity(data & 0x01fe), ((data >> 9) & 0x01), data, systime);
  } else { // Valid data!
    uint8_t pos = BUF_NEXT(q_end);
    event_queue[pos] = (data >> 1) & 0xff;
    q_end = pos;
    if (request_to_send) {
      request_to_send = false;
    } else {
      ps2_listen();
    }
    return;
  }
  if (request_to_send) {
    // About to send data - scanner will be reset anyway
    request_to_send = false;
    return;
  }
  // Error bus_state - reset scanner
  CyDelayUs(60); // Make sure keyboard stops transmitting - CLK line is pinned to the ground already.
  PS2_BitCounter_Stop();
  PS2_BitCounter_Reload();
  ps2_listen();
}

#undef BUF_NEXT
#undef QUEUE_SIZE
