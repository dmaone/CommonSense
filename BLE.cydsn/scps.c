/*******************************************************************************
* File Name: scps.c
*
* Version: 1.0
*
* Description:
*  This file contains SCPS callback handler function.
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
#include "scps.h"

uint16 requestScanRefresh = 0u;
uint16 scanInterval = 0u;
uint16 scanWindow = 0u;


/*******************************************************************************
* Function Name: ScpsCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from 
*   SCPS Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
********************************************************************************/
void ScpsCallBack (uint32 event, void *eventParam)
{
    DBG_PRINTF("SCPS event: %lx, ", event);
    switch(event)
    {
        case CYBLE_EVT_SCPSS_NOTIFICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_SCPSS_NOTIFICATION_ENABLED \r\n");
            requestScanRefresh = ENABLED;
            break;
        case CYBLE_EVT_SCPSS_NOTIFICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_SCPSS_NOTIFICATION_DISABLED \r\n");
            requestScanRefresh = DISABLED;
            break;
        case CYBLE_EVT_SCPSS_SCAN_INT_WIN_CHAR_WRITE:
            scanInterval = CyBle_Get16ByPtr(((CYBLE_SCPS_CHAR_VALUE_T *)eventParam)->value->val);
            scanWindow = CyBle_Get16ByPtr(((CYBLE_SCPS_CHAR_VALUE_T *)eventParam)->value->val + sizeof(scanInterval));
            DBG_PRINTF("CYBLE_EVT_SCPSS_SCAN_INT_WIN_CHAR_WRITE scanInterval: %x, scanWindow: %x \r\n", scanInterval, scanWindow);
            break;
        case CYBLE_EVT_SCPSC_NOTIFICATION:
            break;
        case CYBLE_EVT_SCPSC_READ_DESCR_RESPONSE:
            break;
        case CYBLE_EVT_SCPSC_WRITE_DESCR_RESPONSE:
            break;
        default:
            DBG_PRINTF("Not supported event\r\n");
			break;
    }
}


/*******************************************************************************
* Function Name: ScpsInit()
********************************************************************************
*
* Summary:
*   Initializes the SCPS Service.
*
*******************************************************************************/
void ScpsInit(void)
{
    CYBLE_API_RESULT_T apiResult;
    uint16 cccdValue;
    
    /* Register service specific callback function */
    CyBle_ScpsRegisterAttrCallback(ScpsCallBack);

    /* Read CCCD configurations from flash */
    apiResult = CyBle_ScpssGetCharacteristicDescriptor(CYBLE_SCPS_SCAN_REFRESH,
        CYBLE_SCPS_SCAN_REFRESH_CCCD, CYBLE_CCCD_LEN, (uint8 *)&cccdValue);
    if((apiResult == CYBLE_ERROR_OK) && (cccdValue != 0u))
    {
        requestScanRefresh |= ENABLED;
    }
}


/* [] END OF FILE */
