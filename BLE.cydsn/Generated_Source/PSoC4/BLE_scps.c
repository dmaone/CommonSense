/***************************************************************************//**
* \file CYBLE_scps.c
* \version 3.53
* 
* \brief
*  Contains the source code for the Scan Parameter service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "BLE_eventHandler.h"

static CYBLE_CALLBACK_T CyBle_ScpsApplCallback = NULL;

#ifdef CYBLE_SCPS_SERVER

const CYBLE_SCPSS_T cyBle_scpss =
{
    0x002Eu,    /* Handle of the SCPS service */
    0x0030u,    /* Handle of the Scan Interval Window  characteristic */
    0x0032u,    /* Handle of the Scan Refresh characteristic */
    0x0033u,    /* Handle of the Client Characteristic Configuration descriptor */
};

#endif /* CYBLE_SCPS_SERVER */

#ifdef CYBLE_SCPS_CLIENT

CYBLE_SCPSC_T cyBle_scpsc;
static CYBLE_GATT_DB_ATTR_HANDLE_T cyBle_scpscReqHandle;

#endif /* CYBLE_SCPS_SERVER */

/******************************************************************************
* Function Name: CyBle_ScpsInit
***************************************************************************//**
* 
*  This function initializes the SCPS Service.
*  
******************************************************************************/
void CyBle_ScpsInit(void)
{

#ifdef CYBLE_SCPS_CLIENT
    if(cyBle_serverInfo[CYBLE_SRVI_SCPS].range.startHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
    {
        (void)memset(&cyBle_scpsc, 0, sizeof(cyBle_scpsc));
    }
    cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
#endif /* CYBLE_SCPS_CLIENT */
}


/******************************************************************************
* Function Name: CyBle_ScpsRegisterAttrCallback
***************************************************************************//**
* 
*  Registers a callback function for service specific attribute operations.
*  Service specific write requests from peer device will not be handled with
*  unregistered callback function.
* 
*  \param callbackFunc:  An application layer event callback function to receive 
*                     events from the BLE Component. The definition of 
*                     CYBLE_CALLBACK_T for ScPS is: \n
*                     typedef void (* CYBLE_CALLBACK_T) (uint32 eventCode, 
*                                                       void *eventParam)
*                     * eventCode indicates the event that triggered this 
*                       callback.
*                     * eventParam contains the parameters corresponding to the 
*                       current event.
* 
******************************************************************************/
void CyBle_ScpsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc)
{
    CyBle_ScpsApplCallback = callbackFunc;
}

#ifdef CYBLE_SCPS_SERVER


/******************************************************************************
* Function Name: CyBle_ScpssSetCharacteristicValue
***************************************************************************//**
* 
*  Sets a characteristic value of the Scan Parameters service, which is 
*  identified by charIndex.
* 
*  \param charIndex: The index of the service characteristic.
*      * CYBLE_SCPS_SCAN_INT_WIN - The Scan Interval Window characteristic 
*                                  index.
*      * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index
*  \param attrSize:  The size of the characteristic value attribute.
*  \param attrValue: The pointer to the characteristic value data that should 
*                    be stored to the GATT database.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*   * CYBLE_ERROR_OK - The request handled successfully.
*   * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*   * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - An optional characteristic is 
*                                               absent.
*
******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssSetCharacteristicValue(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;
    uint16 scanInterval;
    uint16 locScanWindow;

    if(charIndex >= CYBLE_SCPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(charIndex == CYBLE_SCPS_SCAN_INT_WIN)
        {
            scanInterval = CyBle_Get16ByPtr(attrValue);
            locScanWindow = CyBle_Get16ByPtr(attrValue + sizeof(scanInterval));
            if( (scanInterval < CYBLE_SCAN_INTERVAL_WINDOW_MIN) ||
                (scanInterval > CYBLE_SCAN_INTERVAL_WINDOW_MAX) ||
                (locScanWindow < CYBLE_SCAN_INTERVAL_WINDOW_MIN) || (locScanWindow > scanInterval) ||
                (attrSize > CYBLE_INTERVAL_WINDOW_CHAR_LEN) )
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.intervalWindowCharHandle;
            }
        }
        else    /* Scan Refresh characteristic */
        {
            if(attrSize > CYBLE_REFRESH_CHAR_LEN)
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.refreshCharHandle;
            }
        }
    }
    if(apiResult == CYBLE_ERROR_OK)
    {
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


/*******************************************************************************
* Function Name: CyBle_ScpssGetCharacteristicValue
****************************************************************************//**
* 
*  Gets a characteristic value of the Scan Parameters service, which is 
*  identified by charIndex.
* 
*  \param charIndex: The index of the service characteristic.
*      * CYBLE_SCPS_SCAN_INT_WIN - The Scan Interval Window characteristic 
*                                  index.
*      * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index
*  \param attrSize:  The size of the characteristic value attribute.
* 
*  \param attrValue: The pointer to the location where characteristic value  
*                    data should be stored.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional characteristic is 
*                                              absent.
*
*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssGetCharacteristicValue(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_DB_ATTR_HANDLE_T charValueHandle;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if(charIndex >= CYBLE_SCPS_CHAR_COUNT)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(charIndex == CYBLE_SCPS_SCAN_INT_WIN)
        {
            if(attrSize > CYBLE_INTERVAL_WINDOW_CHAR_LEN)
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.intervalWindowCharHandle;
            }
        }
        else    /* Scan Refresh characteristic */
        {
            if(attrSize > CYBLE_REFRESH_CHAR_LEN)
            {
                apiResult = CYBLE_ERROR_INVALID_PARAMETER;
            }
            else
            {
                charValueHandle = cyBle_scpss.refreshCharHandle;
            }
        }
    }
    if(apiResult == CYBLE_ERROR_OK)
    {
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


/*******************************************************************************
* Function Name: CyBle_ScpssGetCharacteristicDescriptor
****************************************************************************//**
* 
*  Gets a characteristic descriptor of the specified characteristic of the 
*  Scan Parameters service.
* 
*  \param charIndex: The index of the characteristic.
*      * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index.
*  \param descrIndex: The index of the descriptor.
*      * CYBLE_SCPS_SCAN_REFRESH_CCCD - The Client Characteristic Configuration
*                                       descriptor index of the Scan Refresh 
*                                       characteristic.
*  \param attrSize:  The size of the characteristic value attribute.
*  \param attrValue: The pointer to the location where the characteristic 
*                    descriptor value data should be stored.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*  * CYBLE_ERROR_OK - The request handled successfully.
*  * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*  * CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE - Optional descriptor is absent.
*
*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssGetCharacteristicDescriptor(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    CYBLE_SCPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATT_HANDLE_VALUE_PAIR_T locHandleValuePair;

    if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (descrIndex >= CYBLE_SCPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpss.refreshCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Get data from database */
            locHandleValuePair.attrHandle = cyBle_scpss.refreshCccdHandle;
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


/*******************************************************************************
* Function Name: CyBle_ScpssWriteEventHandler
****************************************************************************//**
* 
*  Handles the Write Request Event for the service.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
* \return
*  Return value is of type CYBLE_GATT_ERR_CODE_T.
*   * CYBLE_GATT_ERR_NONE - Write request handled successfully.
*   * CYBLE_GATT_ERR_UNLIKELY_ERROR - Internal error while writing attribute 
*                                     value.
* 
*******************************************************************************/
CYBLE_GATT_ERR_CODE_T CyBle_ScpssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam)
{
    CYBLE_GATT_ERR_CODE_T gattErr = CYBLE_GATT_ERR_NONE;
    CYBLE_SCPS_CHAR_VALUE_T locChar;

    if(CyBle_ScpsApplCallback != NULL)
    {
        if((eventParam->handleValPair.attrHandle == cyBle_scpss.refreshCccdHandle) ||
           (eventParam->handleValPair.attrHandle == cyBle_scpss.intervalWindowCharHandle))
        {
            locChar.connHandle = eventParam->connHandle;
            
            /* Store value to database */
            gattErr = CyBle_GattsWriteAttributeValue(&eventParam->handleValPair, 0u,
                        &eventParam->connHandle, CYBLE_GATT_DB_PEER_INITIATED);
            if(gattErr == CYBLE_GATT_ERR_NONE)
            {
                /* Client Characteristic Configuration descriptor write request */
                if(eventParam->handleValPair.attrHandle == cyBle_scpss.refreshCccdHandle)
                {
                    uint32 eventCode;
                    locChar.charIndex = CYBLE_SCPS_SCAN_REFRESH;
                    locChar.value = NULL;
                    
                    if(CYBLE_IS_NOTIFICATION_ENABLED_IN_PTR(eventParam->handleValPair.value.val))
                    {
                        eventCode = (uint32)CYBLE_EVT_SCPSS_NOTIFICATION_ENABLED;
                    }
                    else
                    {
                        eventCode = (uint32)CYBLE_EVT_SCPSS_NOTIFICATION_DISABLED;
                    }
                    CyBle_ScpsApplCallback(eventCode, &locChar);
                    
                #if((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
                    /* Set flag to store bonding data to flash */
                    if(cyBle_peerBonding == CYBLE_GAP_BONDING)
                    {
                        cyBle_pendingFlashWrite |= CYBLE_PENDING_CCCD_FLASH_WRITE_BIT;
                    }
                #endif /* (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES) */
                    
                }
                else /* Scan Interval Window characteristic write without response request */
                {
                    locChar.charIndex = CYBLE_SCPS_SCAN_INT_WIN;
                    locChar.value = &eventParam->handleValPair.value;
                    CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSS_SCAN_INT_WIN_CHAR_WRITE, &locChar);
                }
            }
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
    return (gattErr);
}


/*******************************************************************************
* Function Name: CyBle_ScpssSendNotification
****************************************************************************//**
* 
*  This function notifies the client that the server requires the Scan Interval
*  Window Characteristic to be written with the latest values upon notification.
*  
*  On enabling notification successfully for a service characteristic it sends 
*  out a 'Handle Value Notification' which results in 
*  CYBLE_EVT_SCPSC_NOTIFICATION event at the GATT Client's end.
* 
*  \param connHandle: The connection handle.
*  \param charIndex:  The index of the characteristic.
*      * CYBLE_SCPS_SCAN_REFRESH - The Scan Refresh characteristic index.
*  \param attrSize:   The size of the characteristic value attribute.
*  \param attrValue:  The pointer to the characteristic value data that should 
*                     be sent to the Client device.
* 
* \return
*  Return value is of type CYBLE_API_RESULT_T.
*   * CYBLE_ERROR_OK - The request handled successfully.
*   * CYBLE_ERROR_INVALID_PARAMETER - Validation of the input parameter failed.
*   * CYBLE_ERROR_INVALID_OPERATION - This operation is not permitted.
*   * CYBLE_ERROR_INVALID_STATE - Connection with the client is not established.
*   * CYBLE_ERROR_MEMORY_ALLOCATION_FAILED - Memory allocation failed.
*   * CYBLE_ERROR_NTF_DISABLED - Notification is not enabled by the client.
*
*
*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult = CYBLE_ERROR_OK;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T ntfReqParam;

    if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (attrSize != CYBLE_REFRESH_CHAR_LEN))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        /* Send Notification if it is enabled and connected */
        if( (cyBle_scpss.refreshCccdHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
            || (!CYBLE_IS_NOTIFICATION_ENABLED(cyBle_scpss.refreshCccdHandle)))
        {
            apiResult = CYBLE_ERROR_NTF_DISABLED;
        }
        else
        {
            if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
            {
                /* Fill all fields of write request structure ... */
                ntfReqParam.attrHandle = cyBle_scpss.refreshCharHandle;
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
    }
    return (apiResult);
}

#endif /* CYBLE_SCPS_SERVER */

#ifdef CYBLE_SCPS_CLIENT


/*******************************************************************************
* Function Name: CyBle_ScpscDiscoverCharacteristicsEventHandler
****************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_READ_BY_TYPE_RSP event.
*  Based on the service UUID, an appropriate data structure is populated using 
*  the data received as part of the callback.
* 
*  \param discCharInfo: The pointer to a characteristic information structure.
* 
* 
*******************************************************************************/
void CyBle_ScpscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo)
{
    switch(discCharInfo->uuid.uuid16)
    {
        case CYBLE_UUID_CHAR_SCAN_REFRESH:
            CyBle_CheckStoreCharHandle(cyBle_scpsc.refreshChar);
            break;
        case CYBLE_UUID_CHAR_SCAN_WINDOW:
            CyBle_CheckStoreCharHandle(cyBle_scpsc.intervalWindowChar);
            break;
        default:
            break;
    }
}



/*******************************************************************************
* Function Name: CyBle_ScpscDiscoverCharDescriptorsEventHandler
****************************************************************************//**
* 
*  This function is called on receiving a CYBLE_EVT_GATTC_FIND_INFO_RSP event. 
*  This event is generated when the server successfully sends the data for 
*  CYBLE_EVT_GATTC_FIND_INFO_REQ. Based on the service UUID, an appropriate data
*  structure is populated to the service with a service callback.
* 
*  \param discDescrInfo: The pointer to a descriptor information structure.
* 
* 
*******************************************************************************/
void CyBle_ScpscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo)
{
    if(discDescrInfo->uuid.uuid16 == CYBLE_UUID_CHAR_CLIENT_CONFIG)
    {
        CyBle_CheckStoreCharDescrHandle(cyBle_scpsc.refreshCccdHandle);
    }
}


/*******************************************************************************
* Function Name: CyBle_ScpscSetCharacteristicValue
****************************************************************************//**
* 
*  Sets a characteristic value of the Scan Parameters Service, which is 
*  identified by charIndex. 
*  
*  This function call can result in generation of the following events based on 
*  the response from the server device:
*  * CYBLE_EVT_GATTC_WRITE_RSP;
*  * CYBLE_EVT_GATTC_ERROR_RSP.
* 
*  The CYBLE_EVT_SCPSS_SCAN_INT_WIN_CHAR_WRITE event is received by the peer 
*  device on invoking this function.
* 
*  \param connHandle: The connection handle.
*  \param charIndex:  The index of the service characteristic.
*  \param attrSize:   The size of the characteristic value attribute.
*  \param attrValue:  The pointer to the characteristic value data that should be 
*                     sent to the server device.
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
*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_SCPS_CHAR_INDEX_T charIndex,
                                                        uint8 attrSize, uint8 * attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_CMD_REQ_T writeCmdParam;

    if(charIndex != CYBLE_SCPS_SCAN_INT_WIN)
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpsc.intervalWindowChar.valueHandle != CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            writeCmdParam.attrHandle = cyBle_scpsc.intervalWindowChar.valueHandle;
            writeCmdParam.value.val = attrValue;
            writeCmdParam.value.len = attrSize;

            apiResult = CyBle_GattcWriteWithoutResponse(connHandle, &writeCmdParam);
        }
        else
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
    }

    return (apiResult);
}


/*******************************************************************************
* Function Name: CyBle_ScpscSetCharacteristicDescriptor
****************************************************************************//**
* 
*  Sets characteristic descriptor of specified characteristic of the Scan 
*  Parameters Service.
*  
*  Internally, Write Request is sent to the GATT Server and on successful 
*  execution of the request on the Server side the following events can be 
*  generated: 
*  * CYBLE_EVT_SCPSS_NOTIFICATION_ENABLED;
*  * CYBLE_EVT_SCPSS_NOTIFICATION_DISABLED.
* 
*  \param connHandle: The connection handle.
*  \param charIndex:  The index of the service characteristic.
*  \param descrIndex: The index of the service characteristic descriptor.
*  \param attrSize:   The size of the descriptor value attribute.
*  \param attrValue:  The pointer to the characteristic descriptor value data  
*                     that should be sent to the server device.
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
*   If the SCPS service-specific callback is registered 
*      (with CyBle_ScpsRegisterAttrCallback):
*  * CYBLE_EVT_SCPSC_WRITE_DESCR_RESPONSE - In case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index, descr index etc.) are 
*                                provided with event parameter structure
*                                of type CYBLE_SCPS_DESCR_VALUE_T.
*  .
*   Otherwise (if the SCPS service-specific callback is not registered):
*  * CYBLE_EVT_GATTC_WRITE_RSP - In case if the requested attribute is 
*                                successfully wrote on the peer device.
*  * CYBLE_EVT_GATTC_ERROR_RSP - In case if there some trouble with the 
*                                requested attribute on the peer device,
*                                the details are provided with event parameters 
*                                structure (CYBLE_GATTC_ERR_RSP_PARAM_T).
*
*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, CYBLE_SCPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue)
{
    CYBLE_API_RESULT_T apiResult;
    CYBLE_GATTC_WRITE_REQ_T writeReqParam;


    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (descrIndex >= CYBLE_SCPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpsc.refreshChar.valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            /* Fill all fields of write request structure ... */
            writeReqParam.attrHandle = cyBle_scpsc.refreshCccdHandle;
            writeReqParam.value.val = attrValue;
            writeReqParam.value.len = attrSize;

            /* ... and send request to server device. */
            apiResult = CyBle_GattcWriteCharacteristicDescriptors(connHandle, &writeReqParam);
            
            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_scpscReqHandle = writeReqParam.attrHandle;
            }
        }
    }

    return (apiResult);
}


/*******************************************************************************
* Function Name: CyBle_ScpscGetCharacteristicDescriptor
****************************************************************************//**
* 
*  Gets characteristic descriptor of specified characteristic of the Scan 
*  Parameters Service.
*  
*  This function call can result in generation of the following events based on
*  the response from the server device:
*  * CYBLE_EVT_SCPSC_READ_DESCR_RESPONSE;
*  * CYBLE_EVT_GATTC_ERROR_RSP.
* 
*  \param connHandle: The connection handle.
*  \param charIndex:  The index of a Service Characteristic.
*  \param descrIndex: The index of a Service Characteristic Descriptor.
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
*  If the SCPS service-specific callback is registered 
*      (with CyBle_ScpsRegisterAttrCallback):
*  * CYBLE_EVT_SCPSC_READ_DESCR_RESPONSE - In case if the requested attribute is
*                                successfully wrote on the peer device,
*                                the details (char index, descr index, value, etc.) 
*                                are provided with event parameter structure
*                                of type CYBLE_SCPS_DESCR_VALUE_T. 
*  .
*  Otherwise (if the SCPS service-specific callback is not registered):
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
*******************************************************************************/
CYBLE_API_RESULT_T CyBle_ScpscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, CYBLE_SCPS_DESCR_INDEX_T descrIndex)
{
    CYBLE_API_RESULT_T apiResult;

    if(CyBle_GetClientState() != CYBLE_CLIENT_STATE_DISCOVERED)
    {
        apiResult = CYBLE_ERROR_INVALID_STATE;
    }
    else if((charIndex != CYBLE_SCPS_SCAN_REFRESH) || (descrIndex >= CYBLE_SCPS_DESCR_COUNT))
    {
        apiResult = CYBLE_ERROR_INVALID_PARAMETER;
    }
    else
    {
        if(cyBle_scpsc.refreshChar.valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)
        {
            apiResult = CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE;
        }
        else
        {
            apiResult = CyBle_GattcReadCharacteristicDescriptors(connHandle, cyBle_scpsc.refreshCccdHandle);

            /* Save handle to support service specific read response from device */
            if(apiResult == CYBLE_ERROR_OK)
            {
                cyBle_scpscReqHandle = cyBle_scpsc.refreshCccdHandle;
            }
        }
    }

    return (apiResult);
}


/*******************************************************************************
* Function Name: CyBle_ScpscNotificationEventHandler
****************************************************************************//**
* 
*  Handles the Notification Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
*******************************************************************************/
void CyBle_ScpscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam)
{
    CYBLE_SCPS_CHAR_VALUE_T locCharValue;

    if(NULL != CyBle_ScpsApplCallback)
    {
        if(cyBle_scpsc.refreshChar.valueHandle == eventParam->handleValPair.attrHandle)
        {
            locCharValue.connHandle = eventParam->connHandle;
            locCharValue.charIndex = CYBLE_SCPS_SCAN_REFRESH;
            locCharValue.value = &eventParam->handleValPair.value;
            CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSC_NOTIFICATION, &locCharValue);
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
        }
    }
}


/*******************************************************************************
* Function Name: CyBle_ScpscReadResponseEventHandler
****************************************************************************//**
* 
*  Handles the Read Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
*******************************************************************************/
void CyBle_ScpscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam)
{
    if((NULL != CyBle_ScpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_scpscReqHandle))
    {
        if(cyBle_scpsc.refreshCccdHandle == cyBle_scpscReqHandle)
        {
            CYBLE_SCPS_DESCR_VALUE_T locDescrValue;
                
            locDescrValue.connHandle = eventParam->connHandle;
            locDescrValue.charIndex = CYBLE_SCPS_SCAN_REFRESH;
            locDescrValue.descrIndex = CYBLE_SCPS_SCAN_REFRESH_CCCD;
            locDescrValue.value = &eventParam->value;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSC_READ_DESCR_RESPONSE, &locDescrValue);
        }
    }
}


/*******************************************************************************
* Function Name: CyBle_ScpscWriteResponseEventHandler
****************************************************************************//**
* 
*  Handles the Write Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
*******************************************************************************/
void CyBle_ScpscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam)
{
    if((NULL != CyBle_ScpsApplCallback) && (CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE != cyBle_scpscReqHandle))
    {
        if(cyBle_scpsc.refreshCccdHandle == cyBle_scpscReqHandle)
        {
            CYBLE_SCPS_DESCR_VALUE_T locDescrValue;
                
            locDescrValue.connHandle = *eventParam;
            locDescrValue.charIndex = CYBLE_SCPS_SCAN_REFRESH;
            locDescrValue.descrIndex = CYBLE_SCPS_SCAN_REFRESH_CCCD;
            locDescrValue.value = NULL;
            cyBle_eventHandlerFlag &= (uint8)~CYBLE_CALLBACK;
            cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
            CyBle_ScpsApplCallback((uint32)CYBLE_EVT_SCPSC_WRITE_DESCR_RESPONSE, &locDescrValue);
        }
    }
}


/*******************************************************************************
* Function Name: CyBle_ScpscErrorResponseEventHandler
****************************************************************************//**
* 
*  Handles the Error Response Event.
* 
*  \param eventParam: The pointer to the data structure specified by the event.
* 
*******************************************************************************/
void CyBle_ScpscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam)
{
    if((eventParam != NULL) && (eventParam->attrHandle == cyBle_scpscReqHandle))
    {
        cyBle_scpscReqHandle = CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE;
    }
}

#endif /* (CYBLE_SCPS_CLIENT) */

/* [] END OF FILE */
