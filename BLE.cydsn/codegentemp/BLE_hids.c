/***************************************************************************//**
* \file CYBLE_hids.c
* \version 3.51
* 
* \brief
*  Contains the source code for the HID service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "BLE_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_HidsApplCallback;

#ifdef CYBLE_HIDS_SERVER

static const CYBLE_HIDSS_REPORT_T cyBle_hids1ReportArray[0x02u] = {
    {
        0x000Eu, /* Handle of the Report characteristic value */ 
        0x000Fu, /* Handle of the Client Characteristic Configuration descriptor */ 
        0x0010u, /* Handle of the Report Reference descriptor */ 
    },
    {
        0x0012u, /* Handle of the Report characteristic value */ 
        0x0013u, /* Handle of the Client Characteristic Configuration descriptor */ 
        0x0014u, /* Handle of the Report Reference descriptor */ 
    },
};

static const CYBLE_HIDSS_REPORT_T cyBle_hids2ReportArray[0x01u] = {
    {
        0x0038u, /* Handle of the Report characteristic value */ 
        0x0039u, /* Handle of the Client Characteristic Configuration descriptor */ 
        0x003Au, /* Handle of the Report Reference descriptor */ 
    },
};

static const CYBLE_HIDSS_REPORT_T cyBle_hids3ReportArray[0x01u] = {
    {
        0x0046u, /* Handle of the Report characteristic value */ 
        0x0047u, /* Handle of the Client Characteristic Configuration descriptor */ 
        0x0048u, /* Handle of the Report Reference descriptor */ 
    },
};

const CYBLE_HIDSS_T cyBle_hidss[0x03u] = {
    {
        0x000Cu, /* Handle of the HID service */ 
        0x001Cu, /* Handle of the Protocol Mode characteristic */ 
        0x02u, /* Number of report characteristics */ 
        cyBle_hids1ReportArray, /* Information about report characteristics */ 
        {
            {
                0x001Eu, /* Handle of the Report characteristic value */ 
                0x001Fu, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                0x0021u, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
        }, /* Information about boot report characteristics */ 
        0x0016u, /* Handle of the Report Map characteristic */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Map External Report Reference descriptor */ 
        0x0018u, /* Handle of the HID Information characteristic */ 
        0x001Au, /* Handle of the HID Control Point characteristic */ 
    },
    {
        0x0034u, /* Handle of the HID service */ 
        0x0036u, /* Handle of the Protocol Mode characteristic */ 
        0x01u, /* Number of report characteristics */ 
        cyBle_hids2ReportArray, /* Information about report characteristics */ 
        {
            {
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
        }, /* Information about boot report characteristics */ 
        0x003Cu, /* Handle of the Report Map characteristic */ 
        0x003Du, /* Handle of the Report Map External Report Reference descriptor */ 
        0x003Fu, /* Handle of the HID Information characteristic */ 
        0x0041u, /* Handle of the HID Control Point characteristic */ 
    },
    {
        0x0042u, /* Handle of the HID service */ 
        0x0044u, /* Handle of the Protocol Mode characteristic */ 
        0x01u, /* Number of report characteristics */ 
        cyBle_hids3ReportArray, /* Information about report characteristics */ 
        {
            {
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
            {
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report characteristic value */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Client Characteristic Configuration descriptor */ 
                CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Report Reference descriptor */ 
            },
        }, /* Information about boot report characteristics */ 
        0x004Au, /* Handle of the Report Map characteristic */ 
        0x004Bu, /* Handle of the Report Map External Report Reference descriptor */ 
        0x004Du, /* Handle of the HID Information characteristic */ 
        0x004Fu, /* Handle of the HID Control Point characteristic */ 
    },
};

/* Local function prototypes */
static uint32 CyBle_HidssCccdWriteEventHandler(uint8 serviceIndex, const CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_HIDS_SERVER */

#ifdef CYBLE_HIDS_CLIENT

    CYBLE_HIDSC_T cyBle_hidsc[CYBLE_HIDSC_SERVICE_COUNT];
    static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_hidscReqHandle;
    uint8 cyBle_hidscServiceCount;

#endif /* CYBLE_HIDS_CLIENT */


/****************************************************************************** 
* Function Name: CyBle_HidsInit
***************************************************************************//**
* 
*  This function initializes the HID Service.
* 
* 
******************************************************************************/
void CyBle_HidsInit(void)
{

#ifdef CYBLE_HIDS_CLIENT

    uint8 locServIndex;
    for(locServIndex = 0u; locServIndex < CYBLE_HIDSC_SERVICE_COUNT; locServIndex++)
    {
        if(cyBle_serverInfo[CYBLE_SRVI_HIDS + locServIndex].range.startHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            (void)memset(&cyBle_hidsc[locServIndex], 0, sizeof(cyBle_hidsc[locServIndex]));
        }
    }
    cyBle_hidscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    cyBle_hidscServiceCount = 0u;

#endif /* CYBLE_HIDS_CLIENT */
}


/****************************************************************************** 
* Function Name: CyBle_HidsRegisterAttrCallback
***************************************************************************//**
* 
*  Registers a callback function for service specific attribute operations.
*  Service specific write requests from peer device will not be handled with
*  unregistered callback function.
* 
*  \param callbackFunc: An application layer event callback function to receive 
*                    events from the BLE Component. The definition of 
*                    CYBLE_CALLBACK_T for HID Service is:\n
*                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
*                                                       void *eventParam)
*                    * eventCode indicates the event that triggered this 
*                      callback (e.g. CYBLE_EVT_HIDS_NOTIFICATION_ENABLED).
*                    * eventParam contains the parameters corresponding to the 
*                      current event. (e.g. pointer to CYBLE_HIDS_CHAR_VALUE_T
*                      structure that contains details of the characteristic for 
*                      which notification enabled event was triggered).
*
*
* \sideeffect The *eventParams in the callback function should not be used by 
*                the application once the callback function execution is finished.
*                Otherwise this data may become corrupted.
* 
******************************************************************************/
void CyBle_HidsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_HidsApplCallback = callbackFunc;
}

#ifdef CYBLE_HIDS_SERVER


/****************************************************************************** 
* Function Name: CyBle_HidssSetCharacteristicValue
***************************************************************************//**
* 
*  Sets local characteristic value of the specified HID Service characteristics.
* 
*  \param serviceIndex: The index of the service instance. e.g. If two HID Services are 
*                 supported in your design, then first service will be identified
*                 by serviceIndex of 0 and the second by serviceIndex of 1.
*  \param charIndex: The index of a service characteristic.
*              * CYBLE_HIDS_PROTOCOL_MODE - Protocol Mode characteristic
*              * CYBLE_HIDS_REPORT_MAP - Report Map characteristic
*              * CYBLE_HIDS_INFORMATION - HID Information characteristic
*              * CYBLE_HIDS_CONTROL_POINT - HID Control Point characteristic
*              * CYBLE_HIDS_BOOT_KYBRD_IN_REP - Boot Keyboard Input Report Characteristic
*              * CYBLE_HIDS_BOOT_KYBRD_OUT_REP - Boot Keyboard Output Report Characteristic
*              * CYBLE_HIDS_BOOT_MOUSE_IN_REP - Boot Mouse Input Report Characteristic
*              * CYBLE_HIDS_REPORT - Report Characteristic
* 
*  \param attrSize: The size of the characteristic value attribute.
* 
*  \param attrValue: The pointer to the characteristic value data that should be 
*               stored in the GATT database.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
*
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidssSetCharacteristicValue(uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((serviceIndex >= CYBLE_HIDSS_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        switch(charIndex)
        {
            case CYBLE_HIDS_PROTOCOL_MODE:
                charValueHandle = cyBle_hidss[serviceIndex].protocolModeHandle;
                break;
            case CYBLE_HIDS_REPORT_MAP:
                charValueHandle = cyBle_hidss[serviceIndex].reportMapHandle;
                break;
            case CYBLE_HIDS_INFORMATION:
                charValueHandle = cyBle_hidss[serviceIndex].informationHandle;
                break;
            case CYBLE_HIDS_CONTROL_POINT:
                charValueHandle = cyBle_hidss[serviceIndex].controlPointHandle;
                break;
            case CYBLE_HIDS_BOOT_KYBRD_IN_REP:
                charValueHandle = cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX].
                    reportHandle;
                break;
            case CYBLE_HIDS_BOOT_KYBRD_OUT_REP:
                charValueHandle = cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_OUT_REP_INDX].
                    reportHandle;
                break;
            case CYBLE_HIDS_BOOT_MOUSE_IN_REP:
                charValueHandle = cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX].
                    reportHandle;
                break;
            default:                    /* Report characteristic */
                /* Verify that requested report exists in particular service */
                if((charIndex - CYBLE_HIDS_REPORT) < cyBle_hidss[serviceIndex].reportCount)
                {
                    charValueHandle = cyBle_hidss[serviceIndex].reportArray[charIndex - CYBLE_HIDS_REPORT].reportHandle;
                }
                else
                {
                    charValueHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                }
                break;
        }
        if(charValueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Store data in database */
            locHandleValuePair.attrHandle = charValueHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;

            if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
    }
    return (apiResult);
}


/****************************************************************************** 
* Function Name: CyBle_HidssGetCharacteristicValue
***************************************************************************//**
* 
*  Gets local characteristic value of the specified HID Service characteristics.
* 
*  \param serviceIndex: The index of the service instance. e.g. If two HID Services are 
*                 supported in your design, then first service will be identified
*                 by serviceIndex of 0 and the second by serviceIndex of 1.
* 
*  \param charIndex: The index of the service characteristic. 
*              * CYBLE_HIDS_PROTOCOL_MODE - Protocol Mode characteristic
*              * CYBLE_HIDS_REPORT_MAP - Report Map characteristic
*              * CYBLE_HIDS_INFORMATION - HID Information characteristic
*              * CYBLE_HIDS_CONTROL_POINT - HID Control Point characteristic
*              * CYBLE_HIDS_BOOT_KYBRD_IN_REP - Boot Keyboard Input Report Characteristic
*              * CYBLE_HIDS_BOOT_KYBRD_OUT_REP - Boot Keyboard Output Report Characteristic
*              * CYBLE_HIDS_BOOT_MOUSE_IN_REP - Boot Mouse Input Report Characteristic
*              * CYBLE_HIDS_REPORT - Report Characteristic
* 
*  \param attrSize: The size of the characteristic value attribute.
* 
*  \param attrValue: The pointer to the location where characteristic value data
*               should be stored.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is absent
*
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidssGetCharacteristicValue(uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((serviceIndex >= CYBLE_HIDSS_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        switch(charIndex)
        {
            case CYBLE_HIDS_PROTOCOL_MODE:
                charValueHandle = cyBle_hidss[serviceIndex].protocolModeHandle;
                break;
            case CYBLE_HIDS_REPORT_MAP:
                charValueHandle = cyBle_hidss[serviceIndex].reportMapHandle;
                break;
            case CYBLE_HIDS_INFORMATION:
                charValueHandle = cyBle_hidss[serviceIndex].informationHandle;
                break;
            case CYBLE_HIDS_CONTROL_POINT:
                charValueHandle = cyBle_hidss[serviceIndex].controlPointHandle;
                break;
            case CYBLE_HIDS_BOOT_KYBRD_IN_REP:
                charValueHandle =
                    cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX].reportHandle;
                break;
            case CYBLE_HIDS_BOOT_KYBRD_OUT_REP:
                charValueHandle =
                    cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_OUT_REP_INDX].reportHandle;
                break;
            case CYBLE_HIDS_BOOT_MOUSE_IN_REP:
                charValueHandle =
                    cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX].reportHandle;
                break;
            default:                    /* Report characteristic */
                /* Verify that requested report exists in particular service */
                if((charIndex - CYBLE_HIDS_REPORT) < cyBle_hidss[serviceIndex].reportCount)
                {
                    charValueHandle = cyBle_hidss[serviceIndex].reportArray[charIndex - CYBLE_HIDS_REPORT].reportHandle;
                }
                else
                {
                    charValueHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                }
                break;
        }

        if(charValueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Read characteristic value from database */
            locHandleValuePair.attrHandle = charValueHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;
            if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
    }
    return (apiResult);
}


/****************************************************************************** 
* Function Name: CyBle_HidssGetCharacteristicDescriptor
***************************************************************************//**
* 
*  Gets local characteristic descriptor of the specified HID Service 
*  characteristic.
* 
*  \param serviceIndex: The index of the service instance. e.g. If two HID Services are 
*                 supported in your design, then first service will be identified
*                 by serviceIndex of 0 and the second by serviceIndex of 1.
* 
*  \param charIndex: The index of the characteristic.
*              * CYBLE_HIDS_REPORT_MAP          - Report Map Characteristic
*              * CYBLE_HIDS_BOOT_KYBRD_IN_REP   - Boot Keyboard Input Report Characteristic
*              * CYBLE_HIDS_BOOT_KYBRD_OUT_REP  - Boot Keyboard Output Report Characteristic
*              * CYBLE_HIDS_BOOT_MOUSE_IN_REP   - Boot Mouse Input Report Characteristic
*              * CYBLE_HIDS_REPORT              - Report Characteristic
* 
*  \param descrIndex: The index of the descriptor.
*               * CYBLE_HIDS_REPORT_CCCD - Client Characteristic Configuration descriptor
*               * CYBLE_HIDS_REPORT_RRD - Report Reference descriptor
*               * CYBLE_HIDS_REPORT_MAP_ERRD - Report Map External Report Reference descriptor
* 
*  \param attrSize: The size of the descriptor value attribute.
* 
*  \param attrValue: The pointer to the location where characteristic descriptor
*               value data should be stored.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent
*
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidssGetCharacteristicDescriptor(uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex,
    CYBLE_HIDS_DESCR_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_DB_ATTR_HANDLE_T locDescrHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    const CYBLE_HIDSS_REPORT_T *locReport;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((serviceIndex >= CYBLE_HIDSS_SERVICE_COUNT) || (descrIndex >= CYBLE_HIDS_DESCR_COUNT) ||
       (charIndex >= CYBLE_HIDS_CHAR_COUNT) || (charIndex < CYBLE_HIDS_REPORT_MAP) || 
       (charIndex == CYBLE_HIDS_BOOT_KYBRD_OUT_REP))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(charIndex == CYBLE_HIDS_REPORT_MAP)
        {
            if(descrIndex == CYBLE_HIDS_REPORT_MAP_ERRD)
            {
                locDescrHandle = cyBle_hidss[serviceIndex].reportMapErrdHandle;
            }
            else
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
        else /* Report characteristics */
        {
            /* Get report structure */
            if((charIndex >= CYBLE_HIDS_BOOT_KYBRD_IN_REP) && (charIndex <= CYBLE_HIDS_BOOT_MOUSE_IN_REP))
            {
                locReport = &cyBle_hidss[serviceIndex].bootReportArray[charIndex - CYBLE_HIDS_BOOT_KYBRD_IN_REP];
            }
            else
            {
                locReport = &cyBle_hidss[serviceIndex].reportArray[charIndex - CYBLE_HIDS_REPORT];
            }
            /* Get descriptor handle from report structure */
            if(descrIndex == CYBLE_HIDS_REPORT_CCCD)
            {
                locDescrHandle = locReport->cccdHandle;
            }
            else if(descrIndex == CYBLE_HIDS_REPORT_RRD)
            {
                locDescrHandle = locReport->rrdHandle;
            }
            else /* External Report Reference descriptor doesn't exist for report characteristic */
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
    }
    if(apiResult != CYBLE_ERROR_INVALID_PARAMETER)
    {
        if(locDescrHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Read value from database */
            locHandleValuePair.attrHandle = locDescrHandle;
            locHandleValuePair.value.len = attrSize;
            locHandleValuePair.value.val = attrValue;
            if(CYBLE_GATT_ERR_NONE !=
                CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
    }
    return (apiResult);
}


/****************************************************************************** 
* Function Name: CyBle_HidssCccdWriteEventHandler
***************************************************************************//**
* 
*  Handles the Write Request Event of the Client Characteristic Configuration
*  descriptor.
* 
*  \param serviceIndex: The index of the service instance.
*  \param charIndex: The index of the characteristic.
*  \param eventParam: The pointer to the data structure specified by the event.
* 
* \return
*  uint32 eventCode: The event code to be send to application.
* 
******************************************************************************/
static uint32 CyBle_HidssCccdWriteEventHandler(uint8 serviceIndex, const CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    uint32 eventCode;

    if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
    {
        eventCode = (uint32)CYBLE_EVT_HIDSS_NOTIFICATION_ENABLED;
    }
    else
    {
        eventCode = (uint32)CYBLE_EVT_HIDSS_NOTIFICATION_DISABLED;
    }
    cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
    
#if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
    /* Set flag to store bonding data to flash */
    if(cyBle_peerBonding == CYBLE_GAP_BONDING)
    {
        cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
    }
#endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */

    return(eventCode);
}


/****************************************************************************** 
* Function Name: CyBle_HidssWriteEventHandler
***************************************************************************//**
* 
*  Handles the Write Request Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
* \return
*  Return value is of type CYBLE_GATT_ERR_CODE_T.
*   * CYBLE_GATT_ERR_NONE - Write is successful.
* 
******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_HidssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    uint8 locServIndex = 0u;
    uint8 reportIndex;
    CYBLE_HIDS_CHAR_INDEX_T locCharIndex;
    CYBLE_HIDS_CHAR_VALUE_T locCharValue;
    uint32 eventCode = 0u;

    if(NULL != CyBle_HidsApplCallback)
    {
        locCharValue.connHandle = eventParam->connHandle;
        locCharValue.value = NULL;
        do
        {
            locCharValue.serviceIndex = locServIndex;
            /* Protocol Mode characteristic write request */
            if(eventParam->handleValPair.attrHandle == cyBle_hidss[locServIndex].protocolModeHandle)
            {
                locCharValue.charIndex = CYBLE_HIDS_PROTOCOL_MODE;
                if(eventParam->handleValPair.value.len == CYBLE_HIDS_PM_CHAR_LEN)
                {
                    switch(eventParam->handleValPair.value.val[0u])
                    {
                        case CYBLE_HIDS_PROTOCOL_MODE_BOOT:
                            eventCode = (uint32)CYBLE_EVT_HIDSS_BOOT_MODE_ENTER;
                            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                            break;
                        case CYBLE_HIDS_PROTOCOL_MODE_REPORT:
                            eventCode = (uint32)CYBLE_EVT_HIDSS_REPORT_MODE_ENTER;
                            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                            break;
                        default:    /* Reserved for Future Use. */
                            break;
                    }
                }
            }
            /* Control Point characteristic write request */
            else if(eventParam->handleValPair.attrHandle == cyBle_hidss[locServIndex].controlPointHandle)
            {
                locCharValue.charIndex = CYBLE_HIDS_CONTROL_POINT;
                if(eventParam->handleValPair.value.len == CYBLE_HIDS_CP_CHAR_LEN)
                {
                    switch(eventParam->handleValPair.value.val[0u])
                    {
                        case CYBLE_HIDS_CP_SUSPEND:
                            eventCode = (uint32)CYBLE_EVT_HIDSS_SUSPEND;
                            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                            break;
                        case CYBLE_HIDS_CP_EXIT_SUSPEND:
                            eventCode = (uint32)CYBLE_EVT_HIDSS_EXIT_SUSPEND;
                            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                            break;
                        default:    /* Reserved for Future Use. */
                            break;
                    }
                }
            }
            else if(eventParam->handleValPair.attrHandle ==
                        cyBle_hidss[locServIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX].reportHandle)
            {
                locCharValue.charIndex = CYBLE_HIDS_BOOT_KYBRD_IN_REP;
                locCharValue.value = &eventParam->handleValPair.value;
                eventCode = (uint32)CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            }
            else if(eventParam->handleValPair.attrHandle ==
                        cyBle_hidss[locServIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX].cccdHandle)
            {
                locCharValue.charIndex = CYBLE_HIDS_BOOT_KYBRD_IN_REP;
                eventCode = CyBle_HidssCccdWriteEventHandler(locServIndex, eventParam);
            }
            else if(eventParam->handleValPair.attrHandle ==
                        cyBle_hidss[locServIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_OUT_REP_INDX].reportHandle)
            {
                locCharValue.charIndex = CYBLE_HIDS_BOOT_KYBRD_OUT_REP;
                locCharValue.value = &eventParam->handleValPair.value;
                eventCode = (uint32)CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            }
            else if(eventParam->handleValPair.attrHandle ==
                        cyBle_hidss[locServIndex].bootReportArray[CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX].reportHandle)
            {
                locCharValue.charIndex = CYBLE_HIDS_BOOT_MOUSE_IN_REP;
                locCharValue.value = &eventParam->handleValPair.value;
                eventCode = (uint32)CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            }
            else if(eventParam->handleValPair.attrHandle ==
                        cyBle_hidss[locServIndex].bootReportArray[CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX].cccdHandle)
            {
                locCharValue.charIndex = CYBLE_HIDS_BOOT_MOUSE_IN_REP;
                eventCode = CyBle_HidssCccdWriteEventHandler(locServIndex, eventParam);
            }
            else
            {
                uint8 locReqHandle = 0u;
                
                locCharIndex = CYBLE_HIDS_REPORT;
                for(reportIndex = 0u; (reportIndex < cyBle_hidss[locServIndex].reportCount) && (locReqHandle == 0u); 
                        reportIndex++)
                {
                    if(eventParam->handleValPair.attrHandle == 
                        cyBle_hidss[locServIndex].reportArray[reportIndex].reportHandle)
                    {
                        locCharValue.charIndex = locCharIndex;
                        locCharValue.value = &eventParam->handleValPair.value;
                        eventCode = (uint32)CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE;
                        cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                        locReqHandle = 1u;
                    }
                    if(eventParam->handleValPair.attrHandle == 
                        cyBle_hidss[locServIndex].reportArray[reportIndex].cccdHandle)
                    {
                        locCharValue.charIndex = locCharIndex;
                        eventCode = CyBle_HidssCccdWriteEventHandler(locServIndex, eventParam);
                        locReqHandle = 1u;
                    }
                    locCharIndex++;
                }
            }
            locServIndex++;
            /* Store data to database if event is handled */
            if((cyBle_eventHandlerFlag & CYBLE_CALLBACK) == 0u)
            {
                gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                    &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                if(gattErr == CYBLE_GATT_ERR_NONE)
                {                
                    CyBle_HidsApplCallback(eventCode, &locCharValue);
                }
                break;
            }
        }while(locServIndex < CYBLE_HIDSS_SERVICE_COUNT);
    }
    return (gattErr);
}


/****************************************************************************** 
* Function Name: CyBle_HidssSendNotification
***************************************************************************//**
* 
*  Sends specified HID Service characteristic notification to the Client device.
* 
*  CYBLE_EVT_HIDSC_NOTIFICATION event is received by the peer device, on  
*  invoking this function.
*
*  On enabling notification successfully for a service characteristic, it sends
*  out a 'Handle Value Notification' which results in 
*  CYBLE_EVT_HIDSC_NOTIFICATION event at the GATT Client's end.
* 
*  \param connHandle:   BLE peer device connection handle.
*  \param serviceIndex: The index of the HID service instance. e.g. If two HID 
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and the 
*                       second by serviceIndex of 1.
*  \param charIndex:    The index of the service characteristic.
*  \param attrSize:     The size of the characteristic value attribute.
*  \param attrValue:    Pointer to the characteristic value data that should be
*                       sent to the Client device.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*   * CYBLE_ERROR_OK - The request handled successfully.
*   * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*   * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted.
*   * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is 
*                                               absent.
*   * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established.
*   * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed. 
* 
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    const CYBLE_HIDSS_REPORT_T *locReport;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((serviceIndex >= CYBLE_HIDSS_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(charIndex == CYBLE_HIDS_BOOT_KYBRD_IN_REP)
        {
            locReport = &cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX];
        }
        else if(charIndex == CYBLE_HIDS_BOOT_MOUSE_IN_REP)
        {
            locReport = &cyBle_hidss[serviceIndex].bootReportArray[CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX];
        }
        else if((charIndex >= CYBLE_HIDS_REPORT) && (charIndex <= CYBLE_HIDS_REPORT_END))
        {
            /* Verify that requested report exists in particular service */
            if((charIndex - CYBLE_HIDS_REPORT) < cyBle_hidss[serviceIndex].reportCount)
            {
                locReport = &cyBle_hidss[serviceIndex].reportArray[charIndex - CYBLE_HIDS_REPORT];
            }
            else
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
        if(apiResult == CYBLE_ERROR_OK)
        {
            if(locReport->reportHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            {
                apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
            }
            else
            {
                /* Send Notification if it is enabled and connected */
                if(CYBLE_IS_NOTIFICATION_ENABLED(locReport->cccdHandle))
                {
                    if(cyBle_state == CYBLE_STATE_CONNECTED)
                    {
                        /* Fill all fields of write request structure ... */
                        ntfReqParam.attrHandle = locReport->reportHandle;
                        ntfReqParam.value.val = attrValue;
                        ntfReqParam.value.len = attrSize;

                        /* Send notification to client using previously filled structure */
                        apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
                    }
                    else
                    {
                        apiResult = CYBLE_ERROR_INVALID_STATE;
                    }
                }
                else    /* Store data in database */
                {
                    locHandleValuePair.attrHandle = locReport->reportHandle;
                    locHandleValuePair.value.len = attrSize;
                    locHandleValuePair.value.val = attrValue;
                    if(CYBLE_GATT_ERR_NONE !=
                        CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
                    {
                        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
                    }
                }
            }
        }
    }
    return (apiResult);
}


/****************************************************************************** 
* Function Name: CyBle_HidssOnDeviceConnected
***************************************************************************//**
* 
*  Handles the connection establishment request.
* 
******************************************************************************/
void CyBle_HidssOnDeviceConnected(void)
{
    /* The Protocol Mode characteristic value shall be reset to the default value
       following a connection establishment */
    uint8 defaultProtocol = CYBLE_HIDS_PROTOCOL_DEFAULT;
    uint8 locServIndex = 0u;
    do
    {
        if(cyBle_hidss[locServIndex].protocolModeHandle !=
           CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Store default protocol value in database */
            CYBLE_GATT_DB_ATTR_SET_GEN_VALUE(cyBle_hidss[locServIndex].protocolModeHandle,
                                                    &defaultProtocol, sizeof(defaultProtocol));
        }
        ++locServIndex;
    }while(locServIndex < CYBLE_HIDSS_SERVICE_COUNT);
}


#endif /* CYBLE_HIDS_SERVER */

#ifdef CYBLE_HIDS_CLIENT


/****************************************************************************** 
* Function Name: CyBle_HidscInclDiscoveryEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event
*  Based on the service UUID, an appropriate data structure is populated using 
*  the data received as part of the callback.
* 
*  \param discCharInfo:     The pointer to a characteristic information 
*                           structure.
*  \param discoveryService: The index of the service instance.
* 
* 
******************************************************************************/
void CyBle_HidscInclDiscoveryEventHandler(uint16 discoveryService, const CYBLE_DISC_INCL_INFO_T *discInclInfo)
{
    cyBle_hidsc[discoveryService].includeHandle = discInclInfo->inclDefHandle;
}


/****************************************************************************** 
* Function Name: CyBle_HidscDiscoverCharacteristicsEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event
*  Based on the service UUID, an appropriate data structure is populated using 
*  the data received as part of the callback.
* 
*  \param discCharInfo:     The pointer to a characteristic information 
*                           structure.
*  \param discoveryService: The index of the service instance.
* 
******************************************************************************/
void CyBle_HidscDiscoverCharacteristicsEventHandler(uint16 discoveryService, CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    static CYBLE_GATT_DB_ATTR_HANDLE_T *hidsLastEndHandle = NULL;
    static uint16 discoveryLastServ = 0u;    

    /* Update last characteristic endHandle to declaration handle of this characteristic */
    if(hidsLastEndHandle != NULL)
    {
        if(discoveryLastServ == discoveryService)
        {
            *hidsLastEndHandle = discCharInfo->charDeclHandle - 1u;
        }
        hidsLastEndHandle = NULL;
    }
    switch(discCharInfo->uuid.uuid16)
    {
        case CYBLE_UUID_CHAR_HIDS_PROTOCOL_MODE:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].protocolMode);
            break;
        case CYBLE_UUID_CHAR_HIDS_INFORMATION:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].information);
            break;
        case CYBLE_UUID_CHAR_HIDS_REPORT_MAP:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].reportMap);
            hidsLastEndHandle = &cyBle_hidsc[discoveryService].reportMap.endHandle;
            break;
        case CYBLE_UUID_CHAR_HIDS_CONTROL_POINT:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].controlPoint);
            break;
        case CYBLE_UUID_CHAR_HIDS_REPORT:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].
                report[cyBle_hidsc[discoveryService].reportCount]);
            hidsLastEndHandle = &cyBle_hidsc[discoveryService].report[cyBle_hidsc[discoveryService].reportCount].endHandle;
            if(cyBle_hidsc[discoveryService].reportCount < CYBLE_HIDSC_REPORT_COUNT)
            {
                cyBle_hidsc[discoveryService].reportCount++;
            }
            break;
        case CYBLE_UUID_CHAR_HIDS_BOOT_KEYBOARD_IN_RPT:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].bootReport[CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX]);
            hidsLastEndHandle = &cyBle_hidsc[discoveryService].bootReport[CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX].endHandle;
            break;
        case CYBLE_UUID_CHAR_HIDS_BOOT_KEYBOARD_OUT_RPT:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].bootReport[CYBLE_HIDS_BOOT_KYBRD_OUT_REP_INDX]);
            hidsLastEndHandle = &cyBle_hidsc[discoveryService].bootReport[CYBLE_HIDS_BOOT_KYBRD_OUT_REP_INDX].endHandle;
            break;
        case CYBLE_UUID_CHAR_HIDS_BOOT_MOUSE_IN_RPT:
            CyBle_CheckStoreCharHandle(cyBle_hidsc[discoveryService].bootReport[CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX]);
            hidsLastEndHandle = &cyBle_hidsc[discoveryService].bootReport[CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX].endHandle;
            break;
        default:
            break;
    }
    /* Init characteristic endHandle to Service endHandle.
       Characteristic endHandle will be updated to the declaration
       Handler of the following characteristic,
       in the following characteristic discovery procedure. */
    if(hidsLastEndHandle != NULL)
    {
        *hidsLastEndHandle = cyBle_serverInfo[cyBle_disCount].range.endHandle;
        /* Init service index of discovered characteristic */
        discoveryLastServ = discoveryService;
    }

}


/****************************************************************************** 
* Function Name: CyBle_HidscDiscoverCharDescriptorsEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event. 
*  This event is generated when the server successfully sends the data for 
*  CYBLE_EVT_GATTC_FIND_INFO_REQ. Based on the service UUID, an appropriate 
*  data structure is populated to the service with a service callback.
* 
*  \param discDescrInfo:    The pointer to a descriptor information structure.
*  \param discoveryService: The index of the service instance.
* 
******************************************************************************/
void CyBle_HidscDiscoverCharDescriptorsEventHandler(uint16 discoveryService, uint16 discoveryChar,
    CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(discoveryChar == (uint16) CYBLE_SCDI_HIDS_REPORT_MAP)
    {
        if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_EXTERNAL_REPORT_REF)
        {
            CyBle_CheckStoreCharDescrHandle(cyBle_hidsc[discoveryService].reportMap.errdHandle);
        }
    }
            /* descriptors for boot report characteristics */
    else if(discoveryChar < (uint16) CYBLE_SCDI_HIDS_REPORT)
    {
        if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
        {
            CyBle_CheckStoreCharDescrHandle(cyBle_hidsc[discoveryService].
                bootReport[discoveryChar - CYBLE_SCDI_HIDS_BOOT_KYBRD_IN_REP].cccdHandle);
        }
        else if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_REPORT_REFERENCE)
        {
            CyBle_CheckStoreCharDescrHandle(cyBle_hidsc[discoveryService].
                bootReport[discoveryChar - CYBLE_SCDI_HIDS_BOOT_KYBRD_IN_REP].rrdHandle);
        }
        else    /* Report Characteristic doesn't support other descriptors */
        {
        }
    }
    else    /* Descriptors for report characteristics */
    {
        if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
        {
            CyBle_CheckStoreCharDescrHandle(cyBle_hidsc[discoveryService].
                report[discoveryChar - CYBLE_SCDI_HIDS_REPORT].cccdHandle);
        }
        else if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_REPORT_REFERENCE)
        {
            CyBle_CheckStoreCharDescrHandle(cyBle_hidsc[discoveryService].
                report[discoveryChar - CYBLE_SCDI_HIDS_REPORT].rrdHandle);
        }
        else    /* Report Characteristic doesn't support other descriptors */
        {
        }
    }
}


/****************************************************************************** 
* Function Name: CyBle_HidscSetCharacteristicValue
***************************************************************************//**
* 
*  Sends a request to set characteristic value of the specified HID Service, 
*  which is identified by serviceIndex and reportIndex, on the server device. 
*  This function call can result in generation of the following events based on
*  the response from the server device:
*  * CYBLE_EVT_HIDSC_WRITE_CHAR_RESPONSE.
*  * CYBLE_EVT_GATTC_ERROR_RSP.
* 
*  \param connHandle:   The connection handle.
*  \param subProcedure: Characteristic value write sub-procedure.
*                         * CYBLE_HIDSC_WRITE_WITHOUT_RESPONSE;
*                         * CYBLE_HIDSC_WRITE_CHAR_VALUE.
*  \param serviceIndex: The index of the service instance. e.g. If two HID 
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and
*                       the second by serviceIndex of 1.
*  \param charIndex:    The index of a service characteristic.
*  \param attrSize:     The size of the characteristic value attribute.
*  \param attrValue:    The pointer to the characteristic value data that 
*                       should be sent to the server device.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request was sent successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
*                                              the particular characteristic.
*  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this
*                                    characteristic.
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*   If the HIDS service-specific callback is registered 
*      (with CyBle_HidsRegisterAttrCallback):
*  * CYBLE_EVT_HIDSC_WRITE_CHAR_RESPONSE - In case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index, etc.) are 
*                                provided with event parameter structure
*                                of type CYBLE_HIDS_CHAR_VALUE_T.
*  .
*   Otherwise (if the HIDS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_WRITE_RSP - In case if the requested attribute is 
*                                successfully wrote on the peer device.
*  * CYBLE_EVT_GATTC_ERROR_RSP - In case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
    CYBLE_HIDSC_CHAR_WRITE_T subProcedure, uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex, 
    uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T writeReqParam;

    if((serviceIndex >= CYBLE_HIDSC_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        charValueHandle = CyBle_HidscGetCharacteristicValueHandle(serviceIndex, charIndex);

        if(charValueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            writeReqParam.attrHandle = charValueHandle;
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;

            /* Use WriteWithoutResponse subprocedure for the following characteristics: 
                 * CYBLE_HIDS_PROTOCOL_MODE 
                 * CYBLE_HIDS_CONTROL_POINT 
			 */
            if(subProcedure == CYBLE_HIDSC_WRITE_WITHOUT_RESPONSE)
            {
                apiResult = CyBle_GattcWriteWithoutResponse(connHandle, &writeReqParam);
            }
            else
            {
                apiResult = CyBle_GattcWriteCharacteristicValue(connHandle, &writeReqParam);
                /* Save handle to support service specific write response from device */
                if(apiResult == CYBLE_ERROR_OK)
                {
                    cyBle_hidscReqHandle = charValueHandle;
                }
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }

    }

    return (apiResult);
}


/****************************************************************************** 
* Function Name: CyBle_HidscGetCharacteristicValue
***************************************************************************//**
* 
*  This function is used to read the characteristic value from a server
*  which is identified by charIndex.
* 
*  The Read Response returns the characteristic value in the Attribute Value
*  parameter.
*  
*  The Read Response only contains the characteristic value that is less than or
*  equal to (MTU - 1) octets in length. If the characteristic value is greater
*  than (MTU - 1) octets in length, the Read Long Characteristic Value procedure
*  may be used if the rest of the characteristic Value is required.
* 
*  \param connHandle:   The connection handle.
*  \param subProcedure: The characteristic value read sub-procedure.
*                           * CYBLE_HIDSC_READ_CHAR_VALUE;
*                           * CYBLE_HIDSC_READ_LONG_CHAR_VALUE.
*  \param serviceIndex: The index of the service instance.
*  \param charIndex:    The index of the service characteristic.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The read request was sent successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
*                                              the particular characteristic.
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
*  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
*                                    characteristic.
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*   If the HIDS service-specific callback is registered 
*      (with CyBle_HidsRegisterAttrCallback):
*  * CYBLE_EVT_HIDSC_READ_CHAR_RESPONSE - In case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index , value, etc.) are 
*                                provided with event parameter structure
*                                of type CYBLE_HIDS_CHAR_VALUE_T.
*  .
*   Otherwise (if the HIDS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_READ_RSP - In case if the requested attribute is 
*                               successfully read on the peer device,
*                               the details (handle, value, etc.) are 
*                               provided with event parameters 
*                               structure (CYBLE_GATTC_READ_RSP_PARAM_T).
*  * CYBLE_EVT_GATTC_READ_BLOB_RSP - In case if the requested attribute is 
*                                successfully read on the peer device,
*                                the details (handle, value, etc.) are 
*                                provided with event parameters 
*                                structure (CYBLE_GATTC_READ_RSP_PARAM_T).
*  * CYBLE_EVT_GATTC_ERROR_RSP - in case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
    CYBLE_HIDSC_CHAR_READ_T subProcedure, uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATTC_READ_BLOB_REQ_T charReadLongPar;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((serviceIndex >= CYBLE_HIDSC_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        charValueHandle = CyBle_HidscGetCharacteristicValueHandle(serviceIndex, charIndex);

        if(charValueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Use Read Long Characteristic procedure for Report map characteristic */
            if(subProcedure == CYBLE_HIDSC_READ_LONG_CHAR_VALUE)
            {
                charReadLongPar.attrHandle = charValueHandle;
                charReadLongPar.offset = 0u;
                apiResult = CyBle_GattcReadLongCharacteristicValues(connHandle, &charReadLongPar);
            }
            else
            {
                apiResult = CyBle_GattcReadCharacteristicValue(connHandle, charValueHandle);
            }
            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_hidscReqHandle = charValueHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }

    }
    return (apiResult);
}


/****************************************************************************** 
* Function Name: CyBle_HidscSetCharacteristicDescriptor
***************************************************************************//**
* 
*  This function is used to write the characteristic descriptor to the server,
*  which is identified by charIndex. 
* 
*  Internally, Write Request is sent to the GATT Server and on successful 
*  execution of the request on the Server side the following events can be 
*  generated: 
*  * CYBLE_EVT_HIDSS_NOTIFICATION_ENABLED;
*  * CYBLE_EVT_HIDSS_NOTIFICATION_DISABLED.
* 
*  \param connHandle:   The BLE peer device connection handle.
*  \param serviceIndex: The index of the service instance. e.g. If two HID  
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and 
*                       the second by serviceIndex of 1.
*  \param charIndex:    The index of the HID service characteristic.
*  \param descrIndex:   The index of the HID service characteristic descriptor.
*  \param attrSize:     The size of the characteristic value attribute.
*  \param attrValue:    The pointer to the characteristic descriptor value data
*                       that should be sent to the server device.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request was sent successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
*  * CYBLE_ERROR_INVALID_STATE - The state is not valid.
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
*                                              the particular characteristic.
*  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
*                                    the specified attribute.
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*   If the HIDS service-specific callback is registered 
*      (with CyBle_HidsRegisterAttrCallback):
*  * CYBLE_EVT_HIDSC_WRITE_DESCR_RESPONSE - In case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index, descr index etc.) are 
*                                provided with event parameter structure
*                                of type CYBLE_HIDS_DESCR_VALUE_T.
*  .
*   Otherwise (if the HIDS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_WRITE_RSP - In case if the requested attribute is 
*                                successfully wrote on the peer device.
*  * CYBLE_EVT_GATTC_ERROR_RSP - In case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_HIDS_CHAR_INDEX_T charIndex, CYBLE_HIDS_DESCR_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_DB_ATTR_HANDLE_T locDescrHandle;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((serviceIndex >= CYBLE_HIDSC_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT) ||
       (descrIndex >= CYBLE_HIDS_DESCR_COUNT) || (charIndex < CYBLE_HIDS_REPORT_MAP))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        locDescrHandle = CyBle_HidscGetCharacteristicDescriptorHandle(serviceIndex, charIndex, descrIndex);

        if(locDescrHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            /* Fill all fields of write request structure ... */
            writeReqParam.attrHandle = locDescrHandle;
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;

            /* ... and send request to server device. */
            apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_hidscReqHandle = locDescrHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }
    return (apiResult);
}


/******************************************************************************
* Function Name: CyBle_HidscGetCharacteristicDescriptor
***************************************************************************//**
* 
*  Gets a characteristic descriptor of the specified characteristic of the HID 
*  Service from the server device.
*  
*  This function call can result in generation of the following events based on
*  the response from the server device.
*  * CYBLE_EVT_HIDSC_READ_DESCR_RESPONSE;
*  * CYBLE_EVT_GATTC_ERROR_RSP.
* 
*  \param connHandle:   The connection handle.
*  \param serviceIndex: The index of the service instance. e.g. If two HID  
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and the
*                       second by serviceIndex of 1.
*  \param charIndex:    The index of the service characteristic.
*  \param descrIndex:   The index of the HID Service characteristic descriptor.
* 
* \return
*  * CYBLE_ERROR_OK - The request was sent successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
*  * CYBLE_ERROR_INVALID_STATE - The state is not valid.
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
*                                              the particular descriptor.
*  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
*                                    the specified attribute.
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*  If the HIDS service-specific callback is registered 
*      (with CyBle_HidsRegisterAttrCallback):
*  * CYBLE_EVT_HIDSC_READ_DESCR_RESPONSE - In case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index, descr index, value, etc.) 
*                                are provided with event parameter structure
*                                of type CYBLE_HIDS_DESCR_VALUE_T. 
*  .
*  Otherwise (if the HIDS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_READ_RSP - In case if the requested attribute is 
*                               successfully read on the peer device,
*                               the details (handle, value, etc.) are 
*                               provided with event parameters 
*                               structure (CYBLE_GATTC_READ_RSP_PARAM_T).
*  * CYBLE_EVT_GATTC_ERROR_RSP - In case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_HidscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_HIDS_CHAR_INDEX_T charIndex, CYBLE_HIDS_DESCR_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_DB_ATTR_HANDLE_T locDescrHandle;

    if((serviceIndex >= CYBLE_HIDSC_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT) ||
       (descrIndex >= CYBLE_HIDS_DESCR_COUNT) || (charIndex < CYBLE_HIDS_REPORT_MAP))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        locDescrHandle = CyBle_HidscGetCharacteristicDescriptorHandle(serviceIndex, charIndex, descrIndex);

        if(locDescrHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, locDescrHandle);
            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_hidscReqHandle = locDescrHandle;
            }
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }
    return (apiResult);
}


/******************************************************************************
* Function Name: CyBle_HidscNotificationEventHandler
***************************************************************************//**
* 
*  Handles the Notification Event.
* 
*  \param eventParam: Pointer to the data structure specified by the event.
* 
* 
******************************************************************************/
void CyBle_HidscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    uint8 locServIndex;
    CYBLE_HIDS_CHAR_INDEX_T locCharIndex;
    CYBLE_HIDS_CHAR_VALUE_T notifValue;

    if(NULL != CyBle_HidsApplCallback)
    {
        for(locServIndex = 0u; locServIndex < CYBLE_HIDSC_SERVICE_COUNT; locServIndex++)
        {
            for(locCharIndex = CYBLE_HIDS_PROTOCOL_MODE; locCharIndex < CYBLE_HIDS_CHAR_COUNT; locCharIndex++)
            {
                if(CyBle_HidscGetCharacteristicValueHandle(locServIndex, locCharIndex) == 
                                                                            eventParam->handleValPair.attrHandle)
                {
                    notifValue.connHandle = eventParam->connHandle;
                    notifValue.serviceIndex = locServIndex;
                    notifValue.charIndex = locCharIndex;
                    notifValue.value = &eventParam->handleValPair.value;
                    CyBle_HidsApplCallback((uint32)CYBLE_EVT_HIDSC_NOTIFICATION, &notifValue);
                    cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                    break;
                }
            }
        }
    }
}


/******************************************************************************
* Function Name: CyBle_HidscReadResponseEventHandler
***************************************************************************//**
* 
*  Handles the Read Response Event.
* 
*  \param eventParam: Pointer to the data structure specified by the event.
* 
* 
******************************************************************************/
void CyBle_HidscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    uint8 locReqHandle = 0u;
    uint8 locServIndex;
    CYBLE_HIDS_CHAR_INDEX_T locCharIndex;

    if((NULL != CyBle_HidsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_hidscReqHandle))
    {
        for(locServIndex = 0u; (locServIndex < CYBLE_HIDSC_SERVICE_COUNT) && (locReqHandle == 0u); locServIndex++)
        {
            for(locCharIndex = CYBLE_HIDS_PROTOCOL_MODE; (locCharIndex < CYBLE_HIDS_CHAR_COUNT) && (locReqHandle == 0u); 
                locCharIndex++)
            {
                if(cyBle_hidscReqHandle == CyBle_HidscGetCharacteristicValueHandle(locServIndex, locCharIndex))
                {
                    CYBLE_HIDS_CHAR_VALUE_T locCharValue;
                    
                    locCharValue.connHandle = eventParam->connHandle;
                    locCharValue.serviceIndex = locServIndex;
                    locCharValue.charIndex = locCharIndex;
                    locCharValue.value = &eventParam->value;
                    cyBle_hidscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                    CyBle_HidsApplCallback((uint32)CYBLE_EVT_HIDSC_READ_CHAR_RESPONSE, &locCharValue);
                    locReqHandle = 1u;
                }
                if(locCharIndex >= CYBLE_HIDS_REPORT_MAP)
                {
                    CYBLE_HIDS_DESCR_T locDescIndex;

                    for(locDescIndex = CYBLE_HIDS_REPORT_CCCD; (locDescIndex < CYBLE_HIDS_DESCR_COUNT) &&
                       (locReqHandle == 0u); locDescIndex++)
                    {
                        if(cyBle_hidscReqHandle == CyBle_HidscGetCharacteristicDescriptorHandle(
                                                        locServIndex, locCharIndex, locDescIndex))
                        {
                            CYBLE_HIDS_DESCR_VALUE_T locDescrValue;
                            
                            locDescrValue.connHandle = eventParam->connHandle;
                            locDescrValue.serviceIndex = locServIndex;
                            locDescrValue.charIndex = locCharIndex;
                            locDescrValue.descrIndex = locDescIndex;
                            locDescrValue.value = &eventParam->value;
                            cyBle_hidscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                            CyBle_HidsApplCallback((uint32)CYBLE_EVT_HIDSC_READ_DESCR_RESPONSE, &locDescrValue);
                            locReqHandle = 1u;
                        }
                    }
                }
            }
        }
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
* Function Name: CyBle_HidscWriteResponseEventHandler
***************************************************************************//**
*  Handles the Write Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
* 
******************************************************************************/
void CyBle_HidscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    uint8 locServIndex;
    uint8 locReqHandle = 0u;

    CYBLE_HIDS_CHAR_INDEX_T locCharIndex;

    if((NULL != CyBle_HidsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_hidscReqHandle))
    {
        for(locServIndex = 0u; (locServIndex < CYBLE_HIDSC_SERVICE_COUNT) && (locReqHandle == 0u); locServIndex++)
        {
            for(locCharIndex = CYBLE_HIDS_PROTOCOL_MODE; (locCharIndex < CYBLE_HIDS_CHAR_COUNT) && (locReqHandle == 0u); 
                locCharIndex++)
            {
                if(cyBle_hidscReqHandle == CyBle_HidscGetCharacteristicValueHandle(locServIndex, locCharIndex))
                {
                    CYBLE_HIDS_CHAR_VALUE_T locCharValue;
                    
                    locCharValue.connHandle = *eventParam;
                    locCharValue.serviceIndex = locServIndex;
                    locCharValue.charIndex = locCharIndex;
                    locCharValue.value = NULL;
                    cyBle_hidscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                    CyBle_HidsApplCallback((uint32)CYBLE_EVT_HIDSC_WRITE_CHAR_RESPONSE, &locCharValue);
                    locReqHandle = 1u;
                }
                if(locCharIndex >= CYBLE_HIDS_REPORT_MAP)
                {
                    CYBLE_HIDS_DESCR_T locDescIndex;

                    for(locDescIndex = CYBLE_HIDS_REPORT_CCCD; (locDescIndex < CYBLE_HIDS_DESCR_COUNT) &&
                       (locReqHandle == 0u); locDescIndex++)
                    {
                        if(cyBle_hidscReqHandle == CyBle_HidscGetCharacteristicDescriptorHandle(
                                                        locServIndex, locCharIndex, locDescIndex))
                        {
                            CYBLE_HIDS_DESCR_VALUE_T locDescrValue;
                            
                            locDescrValue.connHandle = *eventParam;
                            locDescrValue.serviceIndex = locServIndex;
                            locDescrValue.charIndex = locCharIndex;
                            locDescrValue.descrIndex = locDescIndex;
                            locDescrValue.value = NULL;
                            cyBle_hidscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                            CyBle_HidsApplCallback((uint32)CYBLE_EVT_HIDSC_WRITE_DESCR_RESPONSE, &locDescrValue);
                            locReqHandle = 1u;
                        }
                    }
                }
            }
        }
        if(locReqHandle != 0u)
        {
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/******************************************************************************
* Function Name: CyBle_HidscGetCharacteristicValueHandle
***************************************************************************//**
* 
*  Returns the discovered peer device characteristic value handle.
* 
*  \param serviceIndex: The index of the service instance.
*  \param charIndex:    The index of a service characteristic.
* 
* \return
*  Returns characteristic value handle of type CYBLE_GATT_DB_ATTR_HANDLE_T or
*  CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE when a peer device doesn't have an 
*  optional characteristic.
* 
******************************************************************************/
CYBLE_GATT_DB_ATTR_HANDLE_T CyBle_HidscGetCharacteristicValueHandle(uint8 serviceIndex, 
    CYBLE_HIDS_CHAR_INDEX_T charIndex)
{
    CYBLE_GATT_DB_ATTR_HANDLE_T returnHandle;
    
    if((serviceIndex >= CYBLE_HIDSC_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT))
    {
        returnHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
    else
    {
        if(charIndex == CYBLE_HIDS_PROTOCOL_MODE) 
        {
            returnHandle = cyBle_hidsc[serviceIndex].protocolMode.valueHandle;
        }
        else if(charIndex == CYBLE_HIDS_INFORMATION)
        {
            returnHandle = cyBle_hidsc[serviceIndex].information.valueHandle;
        }
        else if(charIndex == CYBLE_HIDS_CONTROL_POINT)
        {
            returnHandle = cyBle_hidsc[serviceIndex].controlPoint.valueHandle;
        }
        else if(charIndex == CYBLE_HIDS_REPORT_MAP)
        {
            returnHandle = cyBle_hidsc[serviceIndex].reportMap.valueHandle;
        }
        else if(charIndex <= CYBLE_HIDS_BOOT_MOUSE_IN_REP)
        {
            returnHandle = cyBle_hidsc[serviceIndex].bootReport[charIndex - CYBLE_HIDS_BOOT_KYBRD_IN_REP].valueHandle;
        }
        else if((charIndex - CYBLE_HIDS_REPORT) < cyBle_hidsc[(serviceIndex)].reportCount)
        {
            returnHandle = cyBle_hidsc[serviceIndex].report[charIndex - CYBLE_HIDS_REPORT].valueHandle;
            }
        else
        {
            returnHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
    return(returnHandle);
}


/******************************************************************************
* Function Name: CyBle_HidscGetCharacteristicDescriptorHandle
***************************************************************************//**
* 
*  Returns the discovered peer device characteristic descriptor handle.
* 
*  \param serviceIndex: The index of the service instance
*  \param charIndex:    The index of a service characteristic.
*  \param descrIndex:   The index of the descriptor.
* 
* \return
*  Returns characteristic descriptor value handle of type 
*  CYBLE_GATT_DB_ATTR_HANDLE_T or CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE when a 
*  peer device doesn't have an optional characteristic.
* 
******************************************************************************/
CYBLE_GATT_DB_ATTR_HANDLE_T CyBle_HidscGetCharacteristicDescriptorHandle(uint8 serviceIndex, 
    CYBLE_HIDS_CHAR_INDEX_T charIndex, CYBLE_HIDS_DESCR_T descrIndex)
{
    CYBLE_GATT_DB_ATTR_HANDLE_T returnHandle;
    
    if((serviceIndex >= CYBLE_HIDSC_SERVICE_COUNT) || (charIndex >= CYBLE_HIDS_CHAR_COUNT) ||
       (charIndex < CYBLE_HIDS_REPORT_MAP))
    {
        returnHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
    else
    {
        if(charIndex == CYBLE_HIDS_REPORT_MAP)
        {
            if(descrIndex == CYBLE_HIDS_REPORT_MAP_ERRD)
            {
                returnHandle = cyBle_hidsc[serviceIndex].reportMap.errdHandle;
            }
            else
            {
                returnHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            }
        }
        else if(charIndex <= CYBLE_HIDS_BOOT_MOUSE_IN_REP)
        {
            if(descrIndex == CYBLE_HIDS_REPORT_CCCD)
            {
                returnHandle = cyBle_hidsc[serviceIndex].bootReport[charIndex - CYBLE_HIDS_BOOT_KYBRD_IN_REP].cccdHandle;
            }
            else if(descrIndex == CYBLE_HIDS_REPORT_RRD)
            {
                returnHandle = cyBle_hidsc[serviceIndex].bootReport[charIndex - CYBLE_HIDS_BOOT_KYBRD_IN_REP].rrdHandle;
            }
            else
            {
                returnHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            }
        }
        else if((charIndex - CYBLE_HIDS_REPORT) < cyBle_hidsc[serviceIndex].reportCount)
        {
            if(descrIndex == CYBLE_HIDS_REPORT_CCCD)
            {
                returnHandle = cyBle_hidsc[serviceIndex].report[charIndex - CYBLE_HIDS_REPORT].cccdHandle;
            }
            else if(descrIndex == CYBLE_HIDS_REPORT_RRD)
            {
                returnHandle = cyBle_hidsc[serviceIndex].report[charIndex - CYBLE_HIDS_REPORT].rrdHandle;
            }
            else
            {
                returnHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            }
        }
        else
        {
            returnHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
        }
    }
    return(returnHandle);
}


/******************************************************************************
* Function Name: CyBle_HidscErrorResponseEventHandler
***************************************************************************//**
* 
*  Handles the Error Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
* 
******************************************************************************/
void CyBle_HidscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((eventParam != NULL) && (eventParam->attrHandle == cyBle_hidscReqHandle))
    {
        cyBle_hidscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* CYBLE_HIDS_CLIENT */



/* [] END OF FILE */
