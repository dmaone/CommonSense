/***************************************************************************//**
* \file CYBLE_bas.c
* \version 3.53
* 
* \brief
*  Contains the source code for BLE Battery Service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "BLE_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_BasApplCallback = NULL;

#ifdef CYBLE_BAS_SERVER

const CYBLE_BASS_T cyBle_bass[0x01u] = {
    {
        0x0029u, /* Handle of the BAS service */ 
        0x002Bu, /* Handle of the Battery Level characteristic */ 
        0x002Cu, /* Handle of the Characteristic Presentation Format descriptor */ 
        0x002Du, /* Handle of the Client Characteristic Configuration descriptor */ 
    },
};


#endif /* CYBLE_BAS_SERVER */

#ifdef CYBLE_BAS_CLIENT

CYBLE_BASC_T cyBle_basc[CYBLE_BASC_SERVICE_COUNT];
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_bascReqHandle;

#endif /* (CYBLE_BAS_CLIENT) */


/******************************************************************************
* Function Name: CyBle_BasInit
***************************************************************************//**
* 
*  This function initializes the BAS Service.
*
******************************************************************************/
void CyBle_BasInit(void)
{

#ifdef CYBLE_BAS_CLIENT

    uint8 locServIndex;
    for(locServIndex = 0u; locServIndex < CYBLE_BASC_SERVICE_COUNT; locServIndex++)
    {
        if(cyBle_serverInfo[CYBLE_SRVI_BAS + locServIndex].range.startHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            (void)memset(&cyBle_basc[locServIndex], 0, sizeof(cyBle_basc[locServIndex]));
        }
    }
    cyBle_bascReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;

#endif /* CYBLE_BAS_CLIENT */
}


/******************************************************************************
* Function Name: CyBle_BasRegisterAttrCallback
***************************************************************************//**
* 
*  Registers a callback function for service specific attribute operations.
*  Service specific write requests from peer device will not be handled with
*  unregistered callback function.
* 
*  \param callbackFunc: An application layer event callback function to receive 
*                    battery service events from the BLE Component. The
*                    definition of CYBLE_CALLBACK_T for Battery Service is,\n                 
*                    typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
*                                                         void *eventParam)                                                       
*                    * eventCode indicates the event that triggered this 
*                       callback (e.g. CYBLE_EVT_BASS_NOTIFICATION_ENABLED)
*                    * eventParam contains the parameters corresponding to the
*                       current event (e.g., pointer to CYBLE_BAS_CHAR_VALUE_T
*                       structure that contains details of the characteristic 
*                       for which notification enabled event was triggered)
* 
* \sideeffect The *eventParams in the callback function should not be used 
*                by the application once the callback function execution is 
*                finished. Otherwise this data may become corrupted.
* 
******************************************************************************/
void CyBle_BasRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_BasApplCallback = callbackFunc;
}

#ifdef CYBLE_BAS_SERVER


/******************************************************************************
* Function Name: CyBle_BassSetCharacteristicValue
***************************************************************************//**
* 
*  Sets a characteristic value of the service in the local database.
* 
*  \param serviceIndex: The index of the service instance.
*  \param charIndex:    The index of the service characteristic of type 
*                       CYBLE_BAS_CHAR_INDEX_T.
*  \param attrSize:     The size of the characteristic value attribute. A 
*                       battery level characteristic has 1 byte length.
*  \param attrValue:    The pointer to the characteristic value data that 
*                       should be stored to the GATT database.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_BassSetCharacteristicValue(uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((serviceIndex >= CYBLE_BASS_SERVICE_COUNT) || (charIndex >= CYBLE_BAS_CHAR_COUNT) ||
       (attrSize != CYBLE_BAS_BATTERY_LEVEL_LEN) || (*attrValue > CYBLE_BAS_MAX_BATTERY_LEVEL_VALUE))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Store data in database */
        locHandleValuePair.attrHandle = cyBle_bass[serviceIndex].batteryLevelHandle;
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
* Function Name: CyBle_BassGetCharacteristicValue
***************************************************************************//**
* 
*  Gets a characteristic value of the Battery service, which is identified by 
*  charIndex.
* 
*  \param serviceIndex: The index of the service instance. e.g. If two Battery 
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and the
*                       second by serviceIndex of 1.
*  \param charIndex:    The index of a service characteristic of type 
*                       CYBLE_BAS_CHAR_INDEX_T.
*  \param attrSize:     The size of the characteristic value attribute. A 
*                       battery level characteristic has a 1 byte length.
*  \param attrValue:    The pointer to the location where characteristic value 
*                       data should be stored.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_BassGetCharacteristicValue(uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((serviceIndex >= CYBLE_BASS_SERVICE_COUNT) || (charIndex >= CYBLE_BAS_CHAR_COUNT) ||
       (attrSize != CYBLE_BAS_BATTERY_LEVEL_LEN) )
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Read characteristic value from database */
        locHandleValuePair.attrHandle = cyBle_bass[serviceIndex].batteryLevelHandle;
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


/******************************************************************************
* Function Name: CyBle_BassGetCharacteristicDescriptor
***************************************************************************//**
* 
*  Gets a characteristic descriptor of a specified characteristic of the 
*   Battery service from the local GATT database.
* 
*  \param serviceIndex: The index of the service instance. e.g. If two Battery 
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and the
*                       second by serviceIndex of 1.
*  \param charIndex:    The index of a service characteristic of type 
*                       CYBLE_BAS_CHAR_INDEX_T.
*  \param descrIndex:   The index of a service characteristic descriptor of
*                       type CYBLE_BAS_DESCR_INDEX_T.
*  \param attrSize:     The size of the characteristic descriptor attribute.
*  \param attrValue:    The pointer to the location where characteristic  
*                       descriptor value data should be stored.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_BassGetCharacteristicDescriptor(uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex,
    CYBLE_BAS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((serviceIndex >= CYBLE_BASS_SERVICE_COUNT) || (charIndex >= CYBLE_BAS_CHAR_COUNT) ||
       (descrIndex >= CYBLE_BAS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Get data from database */
        if(descrIndex == CYBLE_BAS_BATTERY_LEVEL_CCCD)
        {
            locHandleValuePair.attrHandle = cyBle_bass[serviceIndex].cccdHandle;
        }
        else
        {
            locHandleValuePair.attrHandle = cyBle_bass[serviceIndex].cpfdHandle;
        }
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


/******************************************************************************
* Function Name: CyBle_BassWriteEventHandler
***************************************************************************//**
* 
*  Handles the Write Request Event for Battery service.
* 
*  \param void *eventParam: The pointer to the data structure specified by the
*                           event.
* 
* \return
*  Return value is of type CYBLE_GATT_ERR_CODE_T.
*   * CYBLE_GATT_ERR_NONE - Write request is handled successfully.
*   * CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED - Notification isn't supported.
*   * CYBLE_GATT_ERR_UNLIKELY_ERROR - Internal error while writing attribute
*                                     value.
* 
******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_BassWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    uint8 locServIndex = 0u;
    CYBLE_BAS_CHAR_VALUE_T locCharIndex;
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;

    if(NULL != CyBle_BasApplCallback)
    {
        do
        {
            /* Client Characteristic Configuration descriptor write request */
            if(eventParam->handleValPair.attrHandle == cyBle_bass[locServIndex].cccdHandle)
            {
                /* Verify that optional notification property is enabled for Battery Level characteristic */
                if(CYBLE_IS_NOTIFICATION_SUPPORTED(cyBle_bass[locServIndex].batteryLevelHandle))
                {
                    gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u, 
                        &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                    if(gattErr == CYBLE_GATT_ERR_NONE)
                    {
                        locCharIndex.connHandle = eventParam->connHandle;
                        locCharIndex.serviceIndex = locServIndex;
                        locCharIndex.charIndex = CYBLE_BAS_BATTERY_LEVEL;
                        locCharIndex.value = NULL;
                        
                        if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                        {
                            CyBle_BasApplCallback((uint32)CYBLE_EVT_BASS_NOTIFICATION_ENABLED, &locCharIndex);
                        }
                        else
                        {
                            CyBle_BasApplCallback((uint32)CYBLE_EVT_BASS_NOTIFICATION_DISABLED, &locCharIndex);
                        }
                    #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && \
                        (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                        /* Set flag to store bonding data to flash */
                        if(cyBle_peerBonding == CYBLE_GAP_BONDING)
                        {
                            cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
                        }
                    #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
                    }
                }
                else
                {
                    gattErr = CYBLE_GATT_ERR_REQUEST_NOT_SUPPORTED;
                }
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                break;
            }
        locServIndex++;
        }while(locServIndex < CYBLE_BASS_SERVICE_COUNT);
    }
    return (gattErr);
}


/******************************************************************************
* Function Name: CyBle_BassSendNotification
***************************************************************************//**
* 
*  This function updates the value of the Battery Level characteristic in the 
*  GATT database. If the client has configured a notification on the Battery
*  Level characteristic, the function additionally sends this value using a 
*  GATT Notification message.
*
*  On enabling notification successfully for a service characteristic it sends 
*  out a 'Handle Value Notification' which results in 
*  CYBLE_EVT_BASC_NOTIFICATION event at the GATT Client's end.
* 
*  \param connHandle:   The BLE peer device connection handle
*  \param serviceIndex: The index of the service instance. e.g. If two Battery 
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and the
*                       second by serviceIndex of 1.
*  \param charIndex:    The index of a service characteristic of type 
*                       CYBLE_BAS_CHAR_INDEX_T.
*  \param attrSize:     The size of the characteristic value attribute. A 
*                       battery level characteristic has 1 byte length.
*  \param attrValue:    The pointer to the characteristic value data that 
*                       should be sent to the Client device.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*   * CYBLE_ERROR_OK - The request handled successfully
*   * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed
*   * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted
*   * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established
*   * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed. 
*   * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client.
* 
******************************************************************************/
CYBLE_API_RESULT_T CyBle_BassSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;

    /* Store new data in database */
    apiResult = CyBle_BassSetCharacteristicValue(serviceIndex, charIndex, attrSize, attrValue);
    
    if(apiResult == CYBLE_ERROR_OK)  
    {
        /* Send Notification if it is enabled and connected */
        if(CYBLE_STATE_CONNECTED != CyBle_GetState())
        {
            apiResult = CYBLE_ERROR_INVALID_STATE;
        }
        else if((cyBle_bass[serviceIndex].cccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            || (!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_bass[serviceIndex].cccdHandle)))
        {
            apiResult = CYBLE_ERROR_NTF_DISABLED;
        }
        else
        {
            CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;
            
            /* Fill all fields of write request structure ... */
            ntfReqParam.attrHandle = cyBle_bass[serviceIndex].batteryLevelHandle;
            ntfReqParam.value.val = attrValue;
            ntfReqParam.value.len = attrSize;
            
            /* Send notification to client using previously filled structure */
            apiResult = CyBle_GattsNotification(connHandle, &ntfReqParam);
        }
    }
    
    return (apiResult);
}

#endif /* CYBLE_BAS_SERVER */

#ifdef CYBLE_BAS_CLIENT


/******************************************************************************
* Function Name: CyBle_BascDiscoverCharacteristicsEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP
*  event. Based on the service UUID, an appropriate data structure is populated
*  using the data received as part of the callback.
* 
*  \param discCharInfo:     The pointer to a characteristic information 
*                           structure.
*  \param discoveryService: The index of the service instance
* 
******************************************************************************/
void CyBle_BascDiscoverCharacteristicsEventHandler(uint16 discoveryService, CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    if(discCharInfo->uuid.uuid16 == CYBLE_UUID_CHAR_BATTERY_LEVEL)
    {
        CyBle_CheckStoreCharHandle(cyBle_basc[discoveryService].batteryLevel);
    }
}


/******************************************************************************
* Function Name: CyBle_BascDiscoverCharDescriptorsEventHandler
***************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event.
*  Based on the descriptor UUID, an appropriate data structure is populated 
*  using the data received as part of the callback.
* 
*  \param discDescrInfo:    The pointer to a descriptor information structure.
*  \param discoveryService: The index of the service instance.
* 
******************************************************************************/
void CyBle_BascDiscoverCharDescriptorsEventHandler(uint16 discoveryService,
    CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
    {
        CyBle_CheckStoreCharDescrHandle(cyBle_basc[discoveryService].cccdHandle);
    }
    else if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_FORMAT)
    {
        CyBle_CheckStoreCharDescrHandle(cyBle_basc[discoveryService].cpfdHandle);
    }
    else if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_REPORT_REFERENCE)
    {
        CyBle_CheckStoreCharDescrHandle(cyBle_basc[discoveryService].rrdHandle);
    }
    else    /* BAS doesn't support other descriptors */
    {
    }
}


/******************************************************************************
* Function Name: CyBle_BascGetCharacteristicValue
***************************************************************************//**
* 
*  This function is used to read the characteristic value from a server which
*  is identified by charIndex.
* 
*  This function call can result in generation of the following events based on 
*  the response from the server device.
*  * CYBLE_EVT_BASC_READ_CHAR_RESPONSE.
*  * CYBLE_EVT_GATTC_ERROR_RSP.
* 
*  \param connHandle:   The BLE peer device connection handle.
*  \param serviceIndex: Index of the service instance. e.g. If two Battery 
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and 
*                       the second by serviceIndex of 1.
*  \param charIndex:    The index of a service characteristic of type 
*                       CYBLE_BAS_CHAR_INDEX_T.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The read request was sent successfully. 
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - The peer device doesn't have
*                                              the particular characteristic.
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*  * CYBLE_ERROR_INVALID_STATE - Connection with the server is not established.
*  * CYBLE_ERROR_INVALID_OPERATION - Operation is invalid for this.
*                                    characteristic.
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*   If the BAS service-specific callback is registered 
*      (with CyBle_BasRegisterAttrCallback):
*  * CYBLE_EVT_BASC_READ_CHAR_RESPONSE - in case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index , value, etc.) are 
*                                provided with event parameter structure
*                                of type CYBLE_BAS_CHAR_VALUE_T.
*  .
*   Otherwise (if the BAS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_READ_RSP - in case if the requested attribute is 
*                               successfully read on the peer device,
*                               the details (handle, value, etc.) are 
*                               provided with event parameters 
*                               structure (CYBLE_GATTC_READ_RSP_PARAM_T).
*  * CYBLE_EVT_GATTC_ERROR_RSP - in case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_BascGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_BAS_CHAR_INDEX_T charIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((serviceIndex >= CYBLE_BASC_SERVICE_COUNT) || (charIndex > CYBLE_BAS_BATTERY_LEVEL))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(cyBle_basc[serviceIndex].batteryLevel.valueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        apiResult = CyBle_GattcReadCharacteristicValue(connHandle, 
                                                       cyBle_basc[serviceIndex].batteryLevel.valueHandle);
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_bascReqHandle = cyBle_basc[serviceIndex].batteryLevel.valueHandle;
        }
    }
    else
    {
        apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
    }
    
    return (apiResult);
}


/******************************************************************************
* Function Name: CyBle_BascSetCharacteristicDescriptor
***************************************************************************//**
* 
*  Sends a request to set characteristic descriptor of specified Battery Service
*  characteristic on the server device.
*
*  Internally, Write Request is sent to the GATT Server and on successful 
*  execution of the request on the Server side the following events can be 
*  generated: 
*  * CYBLE_EVT_BASS_NOTIFICATION_ENABLED.
*  * CYBLE_EVT_BASS_NOTIFICATION_DISABLED.
* 
*  \param connHandle:   The BLE peer device connection handle.
*  \param serviceIndex: Index of the service instance. e.g. If two Battery  
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and 
*                       the second by serviceIndex of 1.
*  \param charIndex:    The index of a service characteristic of type 
*                       CYBLE_BAS_CHAR_INDEX_T.
*  \param descrIndex:   The index of a service characteristic descriptor of
*                       type CYBLE_BAS_DESCR_INDEX_T.
*  \param attrSize:     The size of the characteristic descriptor attribute.
*  \param attrValue:    Pointer to the characteristic descriptor value data 
*                       that should be sent to the server device.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request was sent successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
*  * CYBLE_ERROR_INVALID_STATE - The state is not valid.
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
*                                    the specified attribute.
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*   If the BAS service-specific callback is registered 
*      (with CyBle_BasRegisterAttrCallback):
*  * CYBLE_EVT_BASC_WRITE_DESCR_RESPONSE - in case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index, descr index etc.) are 
*                                provided with event parameter structure
*                                of type CYBLE_BAS_DESCR_VALUE_T.
*  .
*   Otherwise (if the BAS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_WRITE_RSP - in case if the requested attribute is 
*                                successfully wrote on the peer device.
*  * CYBLE_EVT_GATTC_ERROR_RSP - in case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_BascSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_BAS_CHAR_INDEX_T charIndex, CYBLE_BAS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((serviceIndex >= CYBLE_BASC_SERVICE_COUNT) 
         || (charIndex > CYBLE_BAS_BATTERY_LEVEL)
         || (descrIndex >= CYBLE_BAS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else if(descrIndex != CYBLE_BAS_BATTERY_LEVEL_CCCD)
    {
        apiResult = CYBLE_ERROR_INVALID_OPERATION;
    }
    else
    {
     /* Fill all fields of write request structure ... */
        writeReqParam.attrHandle = cyBle_basc[serviceIndex].cccdHandle;
        writeReqParam.value.val = attrValue;
        writeReqParam.value.len = attrSize;

        /* ... and send request to server device. */
        apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
        
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_bascReqHandle = writeReqParam.attrHandle;
        }
    }
    
    return (apiResult);
}


/******************************************************************************
* Function Name: CyBle_BascGetCharacteristicDescriptor
***************************************************************************//**
* 
*  Sends a request to get characteristic descriptor of specified Battery Service
*  characteristic from the server device. This function call can result in 
*  generation of the following events based on the response from the server 
*  device.
*  * CYBLE_EVT_BASC_READ_DESCR_RESPONSE.
*  * CYBLE_EVT_GATTC_ERROR_RSP.
* 
*  \param connHandle:   The BLE peer device connection handle.
*  \param serviceIndex: Index of the service instance. e.g. If two Battery  
*                       Services are supported in your design, then first 
*                       service will be identified by serviceIndex of 0 and the
*                       second by serviceIndex of 1.
*  \param charIndex:    The index of a Battery service characteristic of type 
*                       CYBLE_BAS_CHAR_INDEX_T.
*  \param descrIndex:   The index of a Battery service characteristic 
*                       descriptor of type CYBLE_BAS_DESCR_INDEX_T.
* 
* \return
*  * CYBLE_ERROR_OK - The request was sent successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameters failed.
*  * CYBLE_ERROR_INVALID_STATE - The state is not valid.
*  * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*  * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted on 
*                                    the specified attribute.
*
* \events
*  In case of successful execution (return value = CYBLE_ERROR_OK)
*  the next events can appear: \n
*  If the BAS service-specific callback is registered 
*      (with CyBle_BasRegisterAttrCallback):
*  * CYBLE_EVT_BASC_READ_DESCR_RESPONSE - in case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index, descr index, value, etc.) 
*                                are provided with event parameter structure
*                                of type CYBLE_BAS_DESCR_VALUE_T. 
*  .
*  Otherwise (if the BAS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_READ_RSP - in case if the requested attribute is 
*                               successfully read on the peer device,
*                               the details (handle, value, etc.) are 
*                               provided with event parameters 
*                               structure (CYBLE_GATTC_READ_RSP_PARAM_T).
*  * CYBLE_EVT_GATTC_ERROR_RSP - in case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_BascGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_BAS_CHAR_INDEX_T charIndex, CYBLE_BAS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATT_DB_ATTR_HANDLE_T locDescrHandle;
    
    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((serviceIndex >= CYBLE_BASC_SERVICE_COUNT) 
         || (charIndex > CYBLE_BAS_BATTERY_LEVEL)
         || (descrIndex >= CYBLE_BAS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(descrIndex == CYBLE_BAS_BATTERY_LEVEL_CCCD)
        {
            locDescrHandle = cyBle_basc[serviceIndex].cccdHandle;
        }
        else /* CYBLE_BAS_BATTERY_LEVEL_CPFD */
        {
            locDescrHandle = cyBle_basc[serviceIndex].cpfdHandle;
        }
        
        apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, locDescrHandle);
        
        /* Save handle to support service specific read response from device */
        if(apiResult == CYBLE_ERROR_OK)
        {
            cyBle_bascReqHandle = locDescrHandle;
        }
    }

    return (apiResult);
}


/******************************************************************************
* Function Name: CyBle_BascNotificationEventHandler
***************************************************************************//**
* 
*  Handles the Notification Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
******************************************************************************/
void CyBle_BascNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    uint8 i;
    CYBLE_BAS_CHAR_VALUE_T locCharValue;

    if(NULL != CyBle_BasApplCallback)
    {
        for(i = 0u; i < CYBLE_BASC_SERVICE_COUNT; i++)
        {
            if(cyBle_basc[i].batteryLevel.valueHandle == eventParam->handleValPair.attrHandle)
            {
                locCharValue.connHandle = eventParam->connHandle;
                locCharValue.serviceIndex = i;
                locCharValue.charIndex = CYBLE_BAS_BATTERY_LEVEL;
                locCharValue.value = &eventParam->handleValPair.value;
                CyBle_BasApplCallback((uint32)CYBLE_EVT_BASC_NOTIFICATION, &locCharValue);
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                break;
            }
        }
    }
}


/******************************************************************************
* Function Name: CyBle_BascReadResponseEventHandler
***************************************************************************//**
* 
*  Handles the Read Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
*  
******************************************************************************/
void CyBle_BascReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    uint8 locServIndex;
    uint8 locReqHandle = 0u;

    if((NULL != CyBle_BasApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bascReqHandle))
    {
        for(locServIndex = 0u; (locServIndex < CYBLE_BASC_SERVICE_COUNT) && (locReqHandle == 0u); locServIndex++)
        {
            if(cyBle_basc[locServIndex].batteryLevel.valueHandle == cyBle_bascReqHandle)
            {
                CYBLE_BAS_CHAR_VALUE_T batteryLevelValue;
                
                batteryLevelValue.connHandle = eventParam->connHandle;
                batteryLevelValue.serviceIndex = locServIndex;
                batteryLevelValue.charIndex = CYBLE_BAS_BATTERY_LEVEL;
                batteryLevelValue.value = &eventParam->value;
                
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                cyBle_bascReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_BasApplCallback((uint32)CYBLE_EVT_BASC_READ_CHAR_RESPONSE, &batteryLevelValue);
                locReqHandle = 1u;
            }
            else if( (cyBle_basc[locServIndex].cccdHandle == cyBle_bascReqHandle) ||
                     (cyBle_basc[locServIndex].cpfdHandle == cyBle_bascReqHandle) )
            {
                CYBLE_BAS_DESCR_VALUE_T locDescrValue;
                
                locDescrValue.connHandle = eventParam->connHandle;
                locDescrValue.serviceIndex = locServIndex;
                locDescrValue.charIndex = CYBLE_BAS_BATTERY_LEVEL;
                locDescrValue.descrIndex = ((cyBle_basc[locServIndex].cccdHandle == cyBle_bascReqHandle) ? 
                                             CYBLE_BAS_BATTERY_LEVEL_CCCD : CYBLE_BAS_BATTERY_LEVEL_CPFD);
                locDescrValue.value = &eventParam->value;
                
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                cyBle_bascReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                CyBle_BasApplCallback((uint32)CYBLE_EVT_BASC_READ_DESCR_RESPONSE, &locDescrValue);
                locReqHandle = 1u;
            }
            else /* Unsupported event code */
            {
            }
        }
    }
}


/******************************************************************************
* Function Name: CyBle_BascWriteResponseEventHandler
***************************************************************************//**
* 
*  Handles the Write Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
*  
******************************************************************************/
void CyBle_BascWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    uint8 locServIndex;
    
    if((NULL != CyBle_BasApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_bascReqHandle))
    {
        for(locServIndex = 0u; locServIndex < CYBLE_BASC_SERVICE_COUNT; locServIndex++)
        {
            if(cyBle_basc[locServIndex].cccdHandle == cyBle_bascReqHandle)
            {
                CYBLE_BAS_DESCR_VALUE_T locDescIndex;
                
                locDescIndex.connHandle = *eventParam;
                locDescIndex.serviceIndex = locServIndex;
                locDescIndex.charIndex = CYBLE_BAS_BATTERY_LEVEL;
                locDescIndex.descrIndex = CYBLE_BAS_BATTERY_LEVEL_CCCD;
                locDescIndex.value = NULL;
                
                cyBle_bascReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
                cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
                CyBle_BasApplCallback((uint32)CYBLE_EVT_BASC_WRITE_DESCR_RESPONSE, &locDescIndex);
                break;
            }
        }
    }
}


/******************************************************************************
* Function Name: CyBle_BascErrorResponseEventHandler
***************************************************************************//**
* 
*  Handles the Error Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
******************************************************************************/
void CyBle_BascErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((eventParam != NULL) && (eventParam->attrHandle == cyBle_bascReqHandle))
    {
        cyBle_bascReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}

#endif /* (CYBLE_BAS_CLIENT) */


/* [] END OF FILE */

