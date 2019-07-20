/*******************************************************************************
* File Name: debug.c
*
* Version: 1.0
*
* Description:
*  This file contains functions for printf functionality.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
* 
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "common.h"



/* For GCC compiler revise _write() function for printf functionality */
int _write(int file, char *ptr, int len)
{
//    LED_GRN_Write(LED_ON);
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {
        UART_DEB_UartPutChar(*ptr++);
    }
//    LED_GRN_Write(LED_OFF);
    return len;
}

void ShowValue(CYBLE_GATT_VALUE_T *value)
{
    int16 i;
    
    for(i = 0; i < value->len; i++)
    {
        DBG_PRINTF("%2.2x ", value->val[i]);
    }
    DBG_PRINTF("\r\n");
}


void Set32ByPtr(uint8 ptr[], uint32 value)
{
    ptr[0u] = (uint8) value;
    ptr[1u] = (uint8) (value >> 8u);    
    ptr[2u] = (uint8) (value >> 16u);    
    ptr[3u] = (uint8) (value >> 24u);
}


/*******************************************************************************
* Function Name: ShowError()
********************************************************************************
*
* Summary:
*   Shows error condition: Turn On all leds - white colour will indicate error.
*
*******************************************************************************/
void ShowError(void)
{
//    LED_RED_Write(LED_ON);
//    LED_GRN_Write(LED_ON);
//    LED_BLU_Write(LED_ON);
    /* Halt CPU in Debug mode */
    CYASSERT(0u != 0u);
}


/* [] END OF FILE */
