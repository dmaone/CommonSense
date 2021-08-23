/***************************************************************************//**
* \file CYBLE_bas.h
* \version 3.66
* 
* \brief
*  Contains the function prototypes and constants for Battery Service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_BAS_H)
#define CY_BLE_CYBLE_BAS_H

#include "BLE_gatt.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#ifdef CYBLE_BAS_SERVER

/* Maximum supported Battery services */
#define CYBLE_BASS_SERVICE_COUNT             (0x01u)

typedef enum
{
    CYBLE_BATTERY_SERVICE_SERVICE_INDEX
}CYBLE_BASS_INDEXES;
    
#endif /* CYBLE_BAS_SERVER */

#ifdef CYBLE_BAS_CLIENT

/* Maximum supported Battery services */
#define CYBLE_BASC_SERVICE_COUNT             (0x00u)

#endif /* (CYBLE_BAS_CLIENT) */


/***************************************
* Data Struct Definition
***************************************/

/**
 \addtogroup group_service_api_BAS_definitions
 @{
*/

/** BAS Characteristic indexes */
typedef enum
{
    CYBLE_BAS_BATTERY_LEVEL,                            /**< Battery Level characteristic index */
    CYBLE_BAS_CHAR_COUNT                                /**< Total count of characteristics */
}CYBLE_BAS_CHAR_INDEX_T;

/** BAS Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_BAS_BATTERY_LEVEL_CCCD,                       /**< Client Characteristic Configuration descriptor index */
    CYBLE_BAS_BATTERY_LEVEL_CPFD,                       /**< Characteristic Presentation Format descriptor index */
    CYBLE_BAS_DESCR_COUNT                               /**< Total count of descriptors */
}CYBLE_BAS_DESCR_INDEX_T;

#ifdef CYBLE_BAS_SERVER

/** Structure with Battery Service attribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;              /**< Battery Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T batteryLevelHandle;         /**< Battery Level characteristic handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T cpfdHandle;                 /**< Characteristic Presentation Format Descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;                 /**< Client Characteristic Configuration descriptor handle */
} CYBLE_BASS_T;

/** \cond IGNORE */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /**< Peer device handle */
    uint8 serviceIndex;                                     /**< Service instance */
    CYBLE_BAS_CHAR_INDEX_T charIndex;                       /**< Index of a service characteristic */
} CYBLE_BASS_NOTIF_PAR_T;
/** \endcond */

#endif /* CYBLE_BAS_SERVER */

#ifdef CYBLE_BAS_CLIENT

/** Structure with discovered attributes information of Battery Service */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /**< Peer device handle */
    CYBLE_SRVR_CHAR_INFO_T batteryLevel;                    /**< Battery Level characteristic info */
    CYBLE_GATT_DB_ATTR_HANDLE_T cpfdHandle;                 /**< Characteristic Presentation Format descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;                 /**< Client Characteristic Configuration descriptor handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T rrdHandle;                  /**< Report Reference descriptor handle */
} CYBLE_BASC_T;

#endif /* (CYBLE_BAS_CLIENT) */

/** Battery Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /**< Peer device handle */
    uint8 serviceIndex;                                     /**< Service instance */
    CYBLE_BAS_CHAR_INDEX_T charIndex;                       /**< Index of a service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /**< Characteristic value */
} CYBLE_BAS_CHAR_VALUE_T;

/** Battery Service Characteristic Descriptor Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /**< Peer device handle */
    uint8 serviceIndex;                                     /**< Service instance */
    CYBLE_BAS_CHAR_INDEX_T charIndex;                       /**< Index of service characteristic */
    CYBLE_BAS_DESCR_INDEX_T descrIndex;                     /**< Index of service characteristic descriptor */
    CYBLE_GATT_VALUE_T *value;                              /**< Descriptor value */
} CYBLE_BAS_DESCR_VALUE_T;

/** @} */

/***************************************
* API Constants
***************************************/

/* Battery Level characteristic length */
#define CYBLE_BAS_BATTERY_LEVEL_LEN             (0x01u)
/* Maximum Battery Level value */
#define CYBLE_BAS_MAX_BATTERY_LEVEL_VALUE       (100u)


/***************************************
* Function Prototypes
***************************************/

/** \addtogroup group_service_api_BAS_server_client 
@{ 
*/
void CyBle_BasRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);
/** @} */

#ifdef CYBLE_BAS_SERVER
/**
 \addtogroup group_service_api_BAS_server
 @{
*/

CYBLE_API_RESULT_T CyBle_BassSetCharacteristicValue(uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BassGetCharacteristicValue(uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BassGetCharacteristicDescriptor(uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex,
    CYBLE_BAS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BassSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    uint8 serviceIndex, CYBLE_BAS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

/** @} */
#endif /* CYBLE_BAS_SERVER */

#ifdef CYBLE_BAS_CLIENT
/**
 \addtogroup group_service_api_BAS_client
 @{
*/

CYBLE_API_RESULT_T CyBle_BascGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_BAS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_BascSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_BAS_CHAR_INDEX_T charIndex, CYBLE_BAS_DESCR_INDEX_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_BascGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_BAS_CHAR_INDEX_T charIndex, CYBLE_BAS_DESCR_INDEX_T descrIndex);

/** @} */
#endif /* (CYBLE_BAS_CLIENT) */


/***************************************
* Private Function Prototypes
***************************************/

/** \cond IGNORE */
void CyBle_BasInit(void);

#ifdef CYBLE_BAS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_BassWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

#endif /* CYBLE_BAS_SERVER */

#ifdef CYBLE_BAS_CLIENT

void CyBle_BascDiscoverCharacteristicsEventHandler(uint16 discoveryService,
    CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_BascDiscoverCharDescriptorsEventHandler(uint16 discoveryService,
    CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_BascNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_BascReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_BascWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_BascErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_BAS_CLIENT) */
/** \endcond */

/***************************************
* Macro Functions
***************************************/

#ifdef CYBLE_BAS_CLIENT
#define CyBle_BascGetCharacteristicValueHandle(serviceIndex, charIndex)    \
    ((((serviceIndex) >= CYBLE_BASC_SERVICE_COUNT) || ((charIndex) > CYBLE_BAS_BATTERY_LEVEL)) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_basc[serviceIndex].batteryLevel.valueHandle)

#define CyBle_BascGetCharacteristicDescriptorHandle(serviceIndex, charIndex, descrIndex)    \
    ((((serviceIndex) >= CYBLE_BASC_SERVICE_COUNT) || ((charIndex) > CYBLE_BAS_BATTERY_LEVEL) || \
      ((descrIndex) >= CYBLE_BAS_DESCR_COUNT)) ? CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
         ((descrIndex) == CYBLE_BAS_BATTERY_LEVEL_CCCD) ? \
             cyBle_basc[serviceIndex].cccdHandle : \
             cyBle_basc[serviceIndex].cpfdHandle)

#endif /* (CYBLE_BAS_CLIENT) */


/***************************************
* External data references 
***************************************/

#ifdef CYBLE_BAS_SERVER

extern const CYBLE_BASS_T cyBle_bass[CYBLE_BASS_SERVICE_COUNT];

#endif /* CYBLE_BAS_SERVER */

#ifdef CYBLE_BAS_CLIENT

extern CYBLE_BASC_T cyBle_basc[CYBLE_BASC_SERVICE_COUNT];

#endif /* (CYBLE_BAS_CLIENT) */


#endif /* CY_BLE_CYBLE_BAS_H  */


/* [] END OF FILE */
