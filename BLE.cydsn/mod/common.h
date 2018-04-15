/*******************************************************************************
* File Name: common.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>
#include <stdbool.h>

#define ENABLED                     (1u)
#define DISABLED                    (0u)

/***************************************
* Conditional Compilation Parameters
***************************************/
#define DEBUG_UART_ENABLED          ENABLED


/***************************************
*           API Constants
***************************************/
#define LED_ON                      (0u)
#define LED_OFF                     (1u)

#define LED_TIMEOUT                 (10u)              /* Сounts in hundreds of seconds */


/***************************************
*        External Function Prototypes
***************************************/
void ShowValue(CYBLE_GATT_VALUE_T *value);
void Set32ByPtr(uint8 ptr[], uint32 value);
void ShowError(void);

/***************************************
*        Macros
***************************************/
#define DBG_PRINTF(...)          (printf(__VA_ARGS__))

/* [] END OF FILE */
#define MAX_BLE_KEYS 18

union {
  struct {
    uint8_t mods;
    uint8_t reserved;
    uint8_t keys[MAX_BLE_KEYS];
  } __attribute__((packed));
  uint8_t raw[MAX_BLE_KEYS + 2];
} keyboard_report;
uint8_t keyboard_report_usage;
