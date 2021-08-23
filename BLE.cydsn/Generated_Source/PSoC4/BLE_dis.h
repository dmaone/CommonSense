/***************************************************************************//**
* \file CYBLE_dis.h
* \version 3.66
* 
* \brief
*  Contains the function prototypes and constants for Device Information
*  Service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_DIS_H)
#define CY_BLE_CYBLE_DIS_H

#include "BLE_gatt.h"

/**
 \addtogroup group_service_api_DIS_definitions
 @{
*/

/***************************************
* Data Struct Definition
***************************************/

/** DIS characteristic index */
typedef enum
{
    CYBLE_DIS_MANUFACTURER_NAME,                    /**< Manufacturer Name String characteristic index */
    CYBLE_DIS_MODEL_NUMBER,                         /**< Model Number String characteristic index */
    CYBLE_DIS_SERIAL_NUMBER,                        /**< Serial Number String characteristic index*/
    CYBLE_DIS_HARDWARE_REV,                         /**< Hardware Revision String characteristic index */
    CYBLE_DIS_FIRMWARE_REV,                         /**< Firmware Revision String characteristic index */
    CYBLE_DIS_SOFTWARE_REV,                         /**< Software Revision String characteristic index */
    CYBLE_DIS_SYSTEM_ID,                            /**< System ID characteristic index */
    CYBLE_DIS_REG_CERT_DATA,                        /**< IEEE 11073-20601 characteristic index */
    CYBLE_DIS_PNP_ID,                               /**< PnP ID characteristic index */
    CYBLE_DIS_CHAR_COUNT                            /**< Total count of DIS characteristics */

}CYBLE_DIS_CHAR_INDEX_T;

#ifdef CYBLE_DIS_SERVER

/** Structure with Device Information Service attribute handles */
typedef struct
{
    /** Device Information Service handle */
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;
    
    /** Device Information Service Characteristic handles */
    CYBLE_GATT_DB_ATTR_HANDLE_T charHandle[CYBLE_DIS_CHAR_COUNT];
} CYBLE_DISS_T;

#endif /* CYBLE_DIS_SERVER */

#ifdef CYBLE_DIS_CLIENT

/** Structure with discovered attributes information of Device Information Service */
typedef struct
{
    /** Characteristics handle + properties array */
    CYBLE_SRVR_CHAR_INFO_T charInfo[CYBLE_DIS_CHAR_COUNT];

} CYBLE_DISC_T;

#endif /* CYBLE_DIS_CLIENT */

/** Device Information Service Characteristic Value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                         /**< Peer device handle */
    CYBLE_DIS_CHAR_INDEX_T charIndex;                       /**< Index of service characteristic */
    CYBLE_GATT_VALUE_T *value;                              /**< Characteristic value */
} CYBLE_DIS_CHAR_VALUE_T;

/** @} */

/***************************************
* Function Prototypes
***************************************/

/** \addtogroup group_service_api_DIS_server_client 
@{ 
*/
void CyBle_DisRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);
/** @} */

#ifdef CYBLE_DIS_SERVER
/**
 \addtogroup group_service_api_DIS_server
 @{
*/

CYBLE_API_RESULT_T CyBle_DissSetCharacteristicValue(CYBLE_DIS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_DissGetCharacteristicValue(CYBLE_DIS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

/** @} */
#endif /* CYBLE_DIS_SERVER */

#ifdef CYBLE_DIS_CLIENT
/**
 \addtogroup group_service_api_DIS_client
 @{
*/

CYBLE_API_RESULT_T CyBle_DiscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, CYBLE_DIS_CHAR_INDEX_T charIndex);

/** @} */
#endif /* (CYBLE_DIS_CLIENT) */


/***************************************
* Private Function Prototypes
***************************************/

/** \cond IGNORE */
void CyBle_DisInit(void);

#ifdef CYBLE_DIS_CLIENT

void CyBle_DiscDiscoverCharacteristicsEventHandler(CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_DiscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_DiscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);

#endif /* (CYBLE_DIS_CLIENT) */

/***************************************
* Macro Functions
***************************************/

#ifdef CYBLE_DIS_CLIENT

#define CyBle_DiscCheckCharHandle(handle)\
    do {\
        if((handle).valueHandle == CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE)\
        {\
            (handle).valueHandle = discCharInfo->valueHandle;\
            (handle).properties = discCharInfo->properties;\
        }\
        else\
        {\
            CyBle_ApplCallback((uint32)CYBLE_EVT_GATTC_CHAR_DUPLICATION,\
                                      &(discCharInfo->uuid.uuid16));\
        }\
    } while(0)

#define CyBle_DiscGetCharacteristicValueHandle(charIndex)    \
    (((charIndex) >= CYBLE_DIS_CHAR_COUNT) ? \
            CYBLE_GATT_INVALID_ATTR_HANDLE_VALUE : \
            cyBle_disc.charInfo[charIndex].valueHandle)

#endif /* (CYBLE_DIS_CLIENT) */
/** \endcond */


/***************************************
* External data references
***************************************/

#ifdef CYBLE_DIS_SERVER

extern const CYBLE_DISS_T cyBle_diss;

#endif /* CYBLE_DIS_SERVER */

#ifdef CYBLE_DIS_CLIENT

extern CYBLE_DISC_T cyBle_disc;

#endif /* (CYBLE_DIS_CLIENT) */


#endif /* CY_BLE_CYBLE_DIS_H  */


/* [] END OF FILE */
