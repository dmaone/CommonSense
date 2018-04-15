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

#define WDT_COUNTER                                   (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK                              (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE                          (CY_SYS_WDT_COUNTER1_INT) 
#define WDT_COUNTER_ENABLE                            (1u)
#define WDT_TIMEOUT                                   (32767u/10u) /* 100 ms @ 32.768kHz clock */


/***************************************
*        External Function Prototypes
***************************************/
void ShowValue(CYBLE_GATT_VALUE_T *value);
void Set32ByPtr(uint8 ptr[], uint32 value);
void ShowError(void);

/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */


/* [] END OF FILE */
