/***************************************************************************//**
* \file CYBLE_scps.h
* \version 3.61
* 
* \brief
*  Contains the function prototypes and constants for the Scan Parameter service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_SCPS_H)
#define CY_BLE_CYBLE_SCPS_H

#include "BLE.h"
#include "BLE_gatt.h"

/**
 \addtogroup group_service_api_SCPS_definitions
 @{
*/

/***************************************
* Data Struct Definition
***************************************/

/** ScPS Characteristic indexes */
typedef enum
{
    CYBLE_SCPS_SCAN_INT_WIN,                        /**< Scan Interval Window characteristic index */
    CYBLE_SCPS_SCAN_REFRESH,                        /**< Scan Refresh characteristic index */
    CYBLE_SCPS_CHAR_COUNT                           /**< Total count of characteristics */
}CYBLE_SCPS_CHAR_INDEX_T;

/** ScPS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_SCPS_SCAN_REFRESH_CCCD,                   /**< Client Characteristic Configuration descriptor index */
    CYBLE_SCPS_DESCR_COUNT                          /**< Total count of descriptors */
}CYBLE_SCPS_DESCR_INDEX_T;

#ifdef CYBLE_SCPS_SERVER

/** Structure with Scan Parameters Service attribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;            /**< Scan Parameter Service handle*/
    CYBLE_GATT_DB_ATTR_HANDLE_T intervalWindowCharHandle; /**< Handle of Scan Interval Window Characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T refreshCharHandle;        /**< Handle of Scan Refresh Characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T refreshCccdHandle;        /**< Handle of Client Characteristic Configuration Descriptor */
} CYBLE_SCPSS_T;

#endif /* CYBLE_SCPS_SERVER */


#ifdef CYBLE_SCPS_CLIENT

/** Structure with discovered attributes information of Scan Parameters Service */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                     /**< Peer device handle */
    CYBLE_SRVR_CHAR_INFO_T intervalWindowChar;          /**< Handle + properties of Scan Interval Window Characteristic */
    CYBLE_SRVR_CHAR_INFO_T refreshChar;                 /**< Handle + properties of Scan Refresh Characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T refreshCccdHandle;      /**< Handle of Client Characteristic Configuration Descriptor */
} CYBLE_SCPSC_T;

#endif /* CYBLE_SCPS_SERVER */

/** Scan Parameters Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /**< Peer device handle */
    CYBLE_SCPS_CHAR_INDEX_T charIndex;                      /**< Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /**< Characteristic value */
} CYBLE_SCPS_CHAR_VALUE_T;

/** Scan Parameters Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /**< Peer device handle */
    CYBLE_SCPS_CHAR_INDEX_T charIndex;                      /**< Index of service characteristic */
    CYBLE_SCPS_DESCR_INDEX_T descrIndex;                    /**< Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /**< Descriptor value */
} CYBLE_SCPS_DESCR_VALUE_T;

/** @} */

/***************************************
* API Constants
***************************************/

#define CYBLE_SCAN_REFRESH_ENABLED              (0x00u)
#define CYBLE_SCAN_REFRESH_RESERVED             (0xFFu)

#define CYBLE_REFRESH_CHAR_LEN                  (0x01u)
#define CYBLE_INTERVAL_WINDOW_CHAR_LEN          (0x04u)

#define CYBLE_SCAN_INTERVAL_WINDOW_MIN          (0x0004u)
#define CYBLE_SCAN_INTERVAL_WINDOW_MAX          (0x4000u)


/***************************************
* Macro Functions
***************************************/

#ifdef CYBLE_SCPS_CLIENT

#define CyBle_ScpscGetCharacteristicValueHandle(charIndex)\
        (((charIndex) >= CYBLE_SCPS_CHAR_COUNT) ?\
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE :\
         ((charIndex) == CYBLE_SCPS_SCAN_INT_WIN) ?\
            cyBle_scpsc.refreshChar.valueHandle :\
            cyBle_scpsc.intervalWindowChar.valueHandle)

#define CyBle_ScpscGetCharacteristicDescriptorHandle(charIndex, descrIndex)    \
    ((((charIndex) != CYBLE_SCPS_SCAN_REFRESH) || \
     ((descrIndex) >= CYBLE_SCPS_DESCR_COUNT)) ? CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_scpsc.refreshCccdHandle)

#endif /* (CYBLE_SCPS_CLIENT) */


/***************************************
* Function Prototypes
***************************************/

/** \addtogroup group_service_api_SCPS_server_client 
@{ 
*/
void CyBle_ScpsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);
/** @} */

#ifdef CYBLE_SCPS_SERVER
/**
 \addtogroup group_service_api_SCPS_server
 @{
*/

CYBLE_API_RESULT_T CyBle_ScpssSetCharacteristicValue(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_ScpssGetCharacteristicValue(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_ScpssGetCharacteristicDescriptor(CYBLE_SCPS_CHAR_INDEX_T charIndex,
    CYBLE_SCPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_ScpssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

/** @} */
#endif /* CYBLE_SCPS_SERVER */

#ifdef CYBLE_SCPS_CLIENT
/**
 \addtogroup group_service_api_SCPS_client
 @{
*/

CYBLE_API_RESULT_T CyBle_ScpscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_SCPS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 * attrValue);
CYBLE_API_RESULT_T CyBle_ScpscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, CYBLE_SCPS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_ScpscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle,
    CYBLE_SCPS_CHAR_INDEX_T charIndex, CYBLE_SCPS_DESCR_INDEX_T descrIndex);

/** @} */
#endif /* (CYBLE_SCPS_CLIENT) */


/***************************************
* Private Function Prototypes
***************************************/

/** \cond IGNORE */
void CyBle_ScpsInit(void);

#ifdef CYBLE_SCPS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_ScpssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_SCPS_SERVER */

#ifdef CYBLE_SCPS_CLIENT

void CyBle_ScpscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_ScpscDiscoverCharDescriptorsEventHandler(CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_ScpscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_ScpscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_ScpscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_ScpscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_SCPS_CLIENT) */
/** \endcond */


/***************************************
* External data references
***************************************/

#ifdef CYBLE_SCPS_SERVER

extern const CYBLE_SCPSS_T cyBle_scpss;

#endif /* CYBLE_SCPS_SERVER */

#ifdef CYBLE_SCPS_CLIENT

extern CYBLE_SCPSC_T cyBle_scpsc;

#endif /* CYBLE_SCPS_SERVER */


#endif /* CY_BLE_CYBLE_SCPS_H  */

/* [] END OF FILE */
