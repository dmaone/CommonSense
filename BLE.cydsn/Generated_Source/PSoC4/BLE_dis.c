/***************************************************************************//**
* \file CYBLE_dis.c
* \version 3.65
* 
* \brief
*  Contains the source code for the Device Information Service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "BLE_eventHandler.h"

#ifdef CYBLE_DIS_SERVER

const CYBLE_DISS_T cyBle_diss =
{
    0x0022u,    /* Handle of the DIS service */
    {
        0x0024u, /* Handle of the Manufacturer Name String characteristic */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Model Number String characteristic */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Serial Number String characteristic */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Hardware Revision String characteristic */ 
        0x0026u, /* Handle of the Firmware Revision String characteristic */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Software Revision String characteristic */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the System ID characteristic */ 
        CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the IEEE 11073-20601 Regulatory Certification Data List characteristic */ 
        0x0028u, /* Handle of the PnP ID characteristic */ 
    },
};

#endif /* CYBLE_DIS_SERVER */

#ifdef CYBLE_DIS_CLIENT

CYBLE_DISC_T cyBle_disc;
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_discReqHandle;
static CYBLE_CALLBACK_T CyBle_DisApplCallback = NULL;

#endif /* (CYBLE_DIS_CLIENT) */


/****************************************************************************** 
* Function Name: CyBle_DisInit
***************************************************************************//**
* 
*  This function initializes the DIS service.
*  
******************************************************************************/
void CyBle_DisInit(void)
{

#ifdef CYBLE_DIS_CLIENT

    if(cyBle_serverInfo[CYBLE_SRVI_DIS].range.startHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        (void)memset(&cyBle_disc, 0, sizeof(cyBle_disc));
    }
    cyBle_discReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_DIS_CLIENT */

}


/****************************************************************************** 
* Function Name: CyBle_DisRegisterAttrCallback
***************************************************************************//**
* 
*  Registers a callback function for service specific attribute operations.
*  Callback doesn't have events in server role.
* 
*  \param callbackFunc: An application layer event callback function to receive 
*                    events from the BLE Component. The definition of 
*                    CYBLE_CALLBACK_T for Device Information Service is:\n
*                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
*                                                       void *eventParam)
*                    * eventCode indicates the event that triggered this 
*                      callback.
*                    * eventParam contains the parameters corresponding to the 
*                      current event.
* 
******************************************************************************/
void CyBle_DisRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    
#ifdef CYBLE_DIS_CLIENT
    CyBle_DisApplCallback = callbackFunc;
#else
    if(callbackFunc != NULL) /* Callback doesn't have events in server role */
    {
    }
#endif /* CYBLE_DIS_CLIENT */
    
}

#ifdef CYBLE_DIS_SERVER


/******************************************************************************
* Function Name: CyBle_DissSetCharacteristicValue
***************************************************************************//**
* 
*  Sets a characteristic value of the service, which is identified by charIndex,
*  to the local database.
* 
* 
*  \param charIndex: The index of a service characteristic.
*  \param attrSize: The size of the characteristic value attribute.
*  \param *attrValue: The pointer to the characteristic value data that should be
*                   stored to the GATT database.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_DissSetCharacteristicValue(CYBLE_DIS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_DIS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Store data in database */
        locHandleValuePair.attrHandle = cyBle_diss.charHandle[charIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
            CyBle_GattsWriteAttributeValue(&locHandleValuePair, 0u, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}


/******************************************************************************
* Function Name: CyBle_DissGetCharacteristicValue
***************************************************************************//**
* 
*  Gets a characteristic value of the service, which is identified by charIndex,
*  from the GATT database.
* 
*  \param charIndex: The index of a service characteristic.
*  \param attrSize: The size of the characteristic value attribute.
*  \param *attrValue: The pointer to the location where characteristic value data
*               should be stored.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_DissGetCharacteristicValue(CYBLE_DIS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_DIS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Read characteristic value from database */
        locHandleValuePair.attrHandle = cyBle_diss.charHandle[charIndex];
        locHandleValuePair.value.len = attrSize;
        locHandleValuePair.value.val = attrValue;
        if(CYBLE_GATT_ERR_NONE !=
            CyBle_GattsReadAttributeValue(&locHandleValuePair, NULL, CYBLE_GATT_DB_LOCALLY_INITIATED))
        {
            apiResult = CYBLE_ERROR_INVALID_PARAMETER;
        }
    }
    return (apiResult);
}

#endif /* CYBLE_DIS_SERVER */

#ifdef CYBLE_DIS_CLIENT


/******************************************************************************
* Function Name: CyBle_DiscHandleDiscoverCharacteristics
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event.
*  Based on the service UUID, an appropriate data structure is populated using the
*  data received as part of the callback.
* 
*  \param *discCharInfo:  the pointer to a characteristic information structure.
* 
******************************************************************************/
void CyBle_DiscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    switch(discCharInfo->uuid.uuid16)
    {
        case CYBLE_UUID_CHAR_MANUFACTURER_NAME:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_MANUFACTURER_NAME]);
            break;
        case CYBLE_UUID_CHAR_MODEL_NUMBER:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_MODEL_NUMBER]);
            break;
        case CYBLE_UUID_CHAR_SERIAL_NUMBER:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_SERIAL_NUMBER]);
            break;
        case CYBLE_UUID_CHAR_HARDWARE_REV:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_HARDWARE_REV]);
            break;
        case CYBLE_UUID_CHAR_FIRMWARE_REV:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_FIRMWARE_REV]);
            break;
        case CYBLE_UUID_CHAR_SOFTWARE_REV:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_SOFTWARE_REV]);
            break;
        case CYBLE_UUID_CHAR_SYSTEM_ID:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_SYSTEM_ID]);
            break;
        case CYBLE_UUID_CHAR_REG_CERT_DATA:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_REG_CERT_DATA]);
            break;
        case CYBLE_UUID_CHAR_PNP_ID:
            CyBle_DiscCheckCharHandle(cyBle_disc.charInfo[CYBLE_DIS_PNP_ID]);
            break;
        default:
            break;
    }
}


/******************************************************************************
* Function Name: CyBle_DiscGetCharacteristicValue
***************************************************************************//**
* 
*  This function is used to read the characteristic Value from a server
*  which is identified by charIndex.
* 
*  The Read Response returns the characteristic value in the Attribute Value
*  parameter. The Read Response only contains the characteristic value that is 
*  less than or equal to (MTU - 1) octets in length. If the characteristic value 
*  is greater than (MTU - 1) octets in length, a Read Long Characteristic Value
*  procedure may be used if the rest of the characteristic value is required.
* 
*  This function call can result in generation of the following events based on
*  the response from the server device.
*  
*  * CYBLE_EVT_DISC_READ_CHAR_RESPONSE
*  * CYBLE_EVT_GATTC_ERROR_RSP
* 
*  \param connHandle: The connection handle.
*  \param charIndex: The index of the service characteristic.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The read request was sent successfully  
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed
*  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this 
*                                     characteristic
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*   If the DIS service-specific callback is registered 
*      (with CyBle_DisRegisterAttrCallback):
*  * CYBLE_EVT_DISC_READ_CHAR_RESPONSE - in case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index , value, etc.) are 
*                                provided with event parameter structure
*                                of type CYBLE_DIS_CHAR_VALUE_T.
*  .
*   Otherwise (if the DIS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_READ_RSP - in case if the requested attribute is 
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
CYBLE_API_RESULT_T CyBle_DiscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_DIS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(charIndex >= CYBLE_DIS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, cyBle_disc.charInfo[charIndex].valueHandle);
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_discReqHandle = cyBle_disc.charInfo[charIndex].valueHandle;
        }
    }
    return (apiResult);
}


/******************************************************************************
* Function Name: CyBle_DiscReadResponseEventHandler
***************************************************************************//**
* 
*  Handles the Read Response Event.
* 
*  \param *eventParam: the pointer to the data structure specified by the event.
*  
******************************************************************************/
void CyBle_DiscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    CYBLE_DIS_CHAR_INDEX_T locCharIndex;

    if((NULL != CyBle_DisApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_discReqHandle))
    {
        for(locCharIndex = CYBLE_DIS_MANUFACTURER_NAME; locCharIndex < CYBLE_DIS_CHAR_COUNT; locCharIndex++)
        {
            if(cyBle_discReqHandle == cyBle_disc.charInfo[locCharIndex].valueHandle)
            {
                CYBLE_DIS_CHAR_VALUE_T locCharValue;
                
                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.charIndex = locCharIndex;
                locCharValue.value = &eventParam->value;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                cyBle_discReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_DisApplCallback((uint32)CYBLE_EVT_DISC_READ_CHAR_RESPONSE, &locCharValue);
                break;
            }
        }
    }
}


/******************************************************************************
* Function Name: CyBle_DiscErrorResponseEventHandler
***************************************************************************//**
* 
*  Handles the Error Response Event.
* 
*  \param *eventParam: the pointer to the data structure specified by the event.
* 
******************************************************************************/
void CyBle_DiscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((eventParam != NULL) && (eventParam->attrHandle == cyBle_discReqHandle))
    {
        cyBle_discReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}


#endif /* (CYBLE_DIS_CLIENT) */


/* [] END OF FILE */

