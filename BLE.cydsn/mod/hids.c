/*******************************************************************************
* File Name: hids.c
*
* Version: 1.0
*
* Description:
*  This file contains HIDS callback handler function.
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
#include "hids.h"

uint16 keyboardSimulation;
uint8 protocol = CYBLE_HIDS_PROTOCOL_MODE_REPORT;   /* Boot or Report protocol mode */
uint8 suspend = CYBLE_HIDS_CP_EXIT_SUSPEND;         /* Suspend to enter into deep sleep mode */


/*******************************************************************************
* Function Name: HidsCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from 
*   HID Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
********************************************************************************/
void HidsCallBack(uint32 event, void *eventParam)
{
    CYBLE_HIDS_CHAR_VALUE_T *locEventParam = (CYBLE_HIDS_CHAR_VALUE_T *)eventParam;

    DBG_PRINTF("HIDS event: %lx, ", event);

    switch(event)
    {
        case CYBLE_EVT_HIDSS_NOTIFICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_HIDSS_NOTIFICATION_ENABLED: serv=%x, char=%x\r\n", 
                locEventParam->serviceIndex,
                locEventParam->charIndex);
            if(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX == locEventParam->serviceIndex)
            {
                keyboardSimulation = ENABLED;
            }
            break;
        case CYBLE_EVT_HIDSS_NOTIFICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_HIDSS_NOTIFICATION_DISABLED: serv=%x, char=%x\r\n", 
                locEventParam->serviceIndex,
                locEventParam->charIndex);
            if(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX == locEventParam->serviceIndex)
            {
                keyboardSimulation = DISABLED;
            }
            break;
        case CYBLE_EVT_HIDSS_BOOT_MODE_ENTER:
            DBG_PRINTF("CYBLE_EVT_HIDSS_BOOT_MODE_ENTER \r\n");
            protocol = CYBLE_HIDS_PROTOCOL_MODE_BOOT;
            break;
        case CYBLE_EVT_HIDSS_REPORT_MODE_ENTER:
            DBG_PRINTF("CYBLE_EVT_HIDSS_REPORT_MODE_ENTER \r\n");
            protocol = CYBLE_HIDS_PROTOCOL_MODE_REPORT;
            break;
        case CYBLE_EVT_HIDSS_SUSPEND:
            DBG_PRINTF("CYBLE_EVT_HIDSS_SUSPEND \r\n");
            suspend = CYBLE_HIDS_CP_SUSPEND;
        #if (DEBUG_UART_ENABLED == ENABLED)
            /* Reduce power consumption, power down logic that is not required to wake up the system */
            UART_DEB_Stop();
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            break;
        case CYBLE_EVT_HIDSS_EXIT_SUSPEND:
        #if (DEBUG_UART_ENABLED == ENABLED)    
            /* Power up all circuitry previously shut down */
            UART_DEB_Start();
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            DBG_PRINTF("CYBLE_EVT_HIDSS_EXIT_SUSPEND \r\n");
            suspend = CYBLE_HIDS_CP_EXIT_SUSPEND;
            break;
        case CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE:
            if(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX == locEventParam->serviceIndex)
            {
                /* Write request to Keyboard Output Report characteristic. 
                *  Handle Boot and Report protocol. 
                */
                if( ((CYBLE_HIDS_PROTOCOL_MODE_REPORT == protocol) && 
                     (CYBLE_HUMAN_INTERFACE_DEVICE_REPORT_OUT == locEventParam->charIndex)) ||
                    ((CYBLE_HIDS_PROTOCOL_MODE_BOOT == protocol) && 
                     (CYBLE_HIDS_BOOT_KYBRD_OUT_REP == locEventParam->charIndex)) )
                {
                    if( (CAPS_LOCK_LED & locEventParam->value->val[0u]) != 0u)
                    {
                        LED_BLU_Write(LED_ON);
                    }
                    else
                    {
                        LED_BLU_Write(LED_OFF);
                    }
                }
            }
            DBG_PRINTF("CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE: serv=%x, char=%x, value=", 
                locEventParam->serviceIndex,
                locEventParam->charIndex);
                ShowValue(locEventParam->value);
            break;
        case CYBLE_EVT_HIDSC_NOTIFICATION:
            break;
        case CYBLE_EVT_HIDSC_READ_CHAR_RESPONSE:
            break;
        case CYBLE_EVT_HIDSC_WRITE_CHAR_RESPONSE:
            break;
        case CYBLE_EVT_HIDSC_READ_DESCR_RESPONSE:
            break;
        case CYBLE_EVT_HIDSC_WRITE_DESCR_RESPONSE:           
            break;
		default:
            DBG_PRINTF("Not supported event\r\n");
			break;
    }
}


/*******************************************************************************
* Function Name: HidsInit()
********************************************************************************
*
* Summary:
*   Initializes the HID service.
*
*******************************************************************************/
void HidsInit(void)
{
    CYBLE_API_RESULT_T apiResult;
    uint16 cccdValue;
    
    /* Register service specific callback function */
    CyBle_HidsRegisterAttrCallback(HidsCallBack);
    keyboardSimulation = DISABLED;
    /* Read CCCD configurations from flash */
    apiResult = CyBle_HidssGetCharacteristicDescriptor(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
        CYBLE_HUMAN_INTERFACE_DEVICE_REPORT_IN, CYBLE_HIDS_REPORT_CCCD, CYBLE_CCCD_LEN, (uint8 *)&cccdValue);
    if((apiResult == CYBLE_ERROR_OK) && (cccdValue != 0u))
    {
        keyboardSimulation |= ENABLED;
    }
    apiResult = CyBle_HidssGetCharacteristicDescriptor(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
        CYBLE_HIDS_BOOT_KYBRD_IN_REP, CYBLE_HIDS_REPORT_CCCD, CYBLE_CCCD_LEN, (uint8 *)&cccdValue);
    if((apiResult == CYBLE_ERROR_OK) && (cccdValue != 0u))
    {
        keyboardSimulation |= ENABLED;
    }
}


/* [] END OF FILE */
