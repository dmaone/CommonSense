/***************************************************************************//**
* \file CYBLE_hids.h
* \version 3.51
* 
* \brief
*  Contains the function prototypes and constants for HID service.
* 
********************************************************************************
* \copyright
* Copyright 2014-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_BLE_CYBLE_HIDS_H)
#define CY_BLE_CYBLE_HIDS_H

#include "BLE_gatt.h"


/*************************************** 
* Conditional Compilation Parameters
***************************************/

#ifdef CYBLE_HIDS_SERVER

/* Maximum supported count of HID services and reports */
#define CYBLE_HIDSS_SERVICE_COUNT            (0x03u)
#define CYBLE_HIDSS_REPORT_COUNT             (0x02u)

typedef enum
{
    CYBLE_KEYBOARD_SERVICE_INDEX,
    CYBLE_CONSUMER_SERVICE_INDEX,
    CYBLE_SYSTEM_SERVICE_INDEX
}CYBLE_HIDSS_INDEXES;
	
#endif /* CYBLE_HIDS_SERVER */

#ifdef CYBLE_HIDS_CLIENT

/* Maximum supported count of HID services and reports */
#define CYBLE_HIDSC_SERVICE_COUNT            (0x00u)
#define CYBLE_HIDSC_REPORT_COUNT             (0x00u)
#define CYBLE_HIDSC_HOST_ROLE                ()

#endif /* CYBLE_HIDS_CLIENT */

/* Maximum 3 boot reports exist */
#define CYBLE_HIDS_BOOT_REPORT_COUNT         (0x03u)

/* Define maximum report count over both roles */
#ifdef CYBLE_HIDS_SERVER
    #ifdef CYBLE_HIDS_CLIENT /* Both roles */
        #if (CYBLE_HIDSC_REPORT_COUNT > CYBLE_HIDSS_REPORT_COUNT) 
            #define CYBLE_HIDS_REPORT_COUNT          CYBLE_HIDSC_REPORT_COUNT
        #else
            #define CYBLE_HIDS_REPORT_COUNT          CYBLE_HIDSS_REPORT_COUNT
        #endif /* CYBLE_HIDSC_REPORT_COUNT > CYBLE_HIDSS_REPORT_COUNT */
    #else /* Only server role */
        #define CYBLE_HIDS_REPORT_COUNT      CYBLE_HIDSS_REPORT_COUNT
    #endif /* CYBLE_HIDS_CLIENT */
#else   /* Only client role*/
    #define CYBLE_HIDS_REPORT_COUNT          CYBLE_HIDSC_REPORT_COUNT
#endif /* CYBLE_HIDS_SERVER */

/**
 \addtogroup group_service_api_HIDS_definitions
 @{
*/

/***************************************  
* Data Struct Definition
***************************************/

/** HIDS characteristic indexes */
typedef enum
{
    CYBLE_HIDS_PROTOCOL_MODE,                        /**< Protocol Mode Characteristic index */
    CYBLE_HIDS_INFORMATION,                          /**< HID Information Characteristic index */
    CYBLE_HIDS_CONTROL_POINT,                        /**< HID Control Point Characteristic index */
    CYBLE_HIDS_REPORT_MAP,                           /**< Report Map Characteristic index */
    CYBLE_HIDS_BOOT_KYBRD_IN_REP,                    /**< Boot Keyboard Input Report Characteristic index */
    CYBLE_HIDS_BOOT_KYBRD_OUT_REP,                   /**< Boot Keyboard Output Report Characteristic index */
    CYBLE_HIDS_BOOT_MOUSE_IN_REP,                    /**< Boot Mouse Input Report Characteristic index */
    CYBLE_HIDS_REPORT,                               /**< Report Characteristic index */
	/* Keyboard service Report characteristics indexes */
    CYBLE_KEYBOARD_REPORTIN0 = CYBLE_HIDS_REPORT,    /* ReportIn0 characteristic index */
    CYBLE_KEYBOARD_REPORTOUT0,                       /* ReportOut0 characteristic index */
    /* Consumer service Report characteristics indexes */
    CYBLE_CONSUMER_REPORTIN0 = CYBLE_HIDS_REPORT,    /* ReportIn0 characteristic index */
    /* System service Report characteristics indexes */
    CYBLE_SYSTEM_REPORTIN0 = CYBLE_HIDS_REPORT,      /* ReportIn0 characteristic index */
    CYBLE_HIDS_REPORT_END = CYBLE_HIDS_REPORT + CYBLE_HIDS_REPORT_COUNT - 1,    /**< Index of last Report Char */
    CYBLE_HIDS_CHAR_COUNT                              /**< Total count of characteristics */
}CYBLE_HIDS_CHAR_INDEX_T;

/** HID Service Characteristic Descriptors indexes */
typedef enum
{
    CYBLE_HIDS_REPORT_CCCD,                         /**< Client Characteristic Configuration descriptor index */
    CYBLE_HIDS_REPORT_RRD,                          /**< Report Reference descriptor index */
    CYBLE_HIDS_REPORT_MAP_ERRD,                     /**< Report Map External Report Reference descriptor index */
    CYBLE_HIDS_DESCR_COUNT                          /**< Total count of descriptors */
}CYBLE_HIDS_DESCR_T;


#ifdef CYBLE_HIDS_SERVER

/** HID server Report Reference descriptor value - Report ID and Report Type */
typedef struct
{
    uint8 reportId;            /**< Non-zero value if there are more than one instance of the same Report Type */
    uint8 reportType;          /**< Type of Report characteristic  */
} CYBLE_HIDSS_REPORT_REF_T;


/** HID Information characteristic value */
typedef struct
{
    uint16 bcdHID;        /**< Version number of HIDSe USB HID Specification implemented by HID Device */
    uint8 bCountryCode;   /**< Identifies which country hardware is localized for */
    uint8 flags;          /**< Bit 0: RemoteWake - Indicates whether HID Device is capable of 
                                    sending wake-signal to HID Host.
                             Bit 1: NormallyConnectable - Indicates whether HID Device will be
                                    advertising when bonded but not connected. */
} CYBLE_HIDSS_INFORMATION_T;


/** HID Server Report characteristic */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T reportHandle;       /**< Handle of Report characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;         /**< Handle of Client Characteristic Configuration descriptor */
    CYBLE_GATT_DB_ATTR_HANDLE_T rrdHandle;          /**< Handle of Report Reference descriptor */
} CYBLE_HIDSS_REPORT_T;

/** Structure with HID Service attribute handles */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T serviceHandle;         /**< Handle of HID service */
    CYBLE_GATT_DB_ATTR_HANDLE_T protocolModeHandle;    /**< Handle of Protocol Mode Characteristic */
    uint8 reportCount;                                 /**< Number of report Characteristics */
    const CYBLE_HIDSS_REPORT_T *reportArray;           /**< Info about report Characteristics */
    CYBLE_HIDSS_REPORT_T bootReportArray[CYBLE_HIDS_BOOT_REPORT_COUNT]; /**< Info about Boot Report Characteristics */
    CYBLE_GATT_DB_ATTR_HANDLE_T reportMapHandle;       /**< Handle of Report Map Characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T reportMapErrdHandle;   /**< Handle of Report Map External Report Reference descr. */
    CYBLE_GATT_DB_ATTR_HANDLE_T informationHandle;     /**< Handle of HID Information Characteristic */
    CYBLE_GATT_DB_ATTR_HANDLE_T controlPointHandle;    /**< Handle of HID Control Point Characteristic */
} CYBLE_HIDSS_T;

#endif /* (CYBLE_HIDS_SERVER) */

#ifdef CYBLE_HIDS_CLIENT

/** HID Client Report characteristic */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T cccdHandle;         /**< Handle of Client Characteristic Configuration Descriptor */
    CYBLE_GATT_DB_ATTR_HANDLE_T rrdHandle;          /**< Handle of Report Reference Descriptor */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;        /**< Handle of Report Characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;          /**< End handle of Characteristic */
    uint8  properties;                              /**< Properties for value field */
} CYBLE_HIDSC_REPORT_T;

/** HID client Report map characteristic */
typedef struct
{
    CYBLE_GATT_DB_ATTR_HANDLE_T errdHandle;         /**< Handle of Report Map External Report Reference descriptor */
    CYBLE_GATT_DB_ATTR_HANDLE_T valueHandle;        /**< Handle of Report characteristic value */
    CYBLE_GATT_DB_ATTR_HANDLE_T endHandle;          /**< End handle of characteristic */
    uint8  properties;                              /**< Properties for value field */
} CYBLE_HIDSC_REPORT_MAP_T;

/** Structure with discovered attributes information of HID Service */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;                 /**< Peer device handle */
    CYBLE_SRVR_CHAR_INFO_T protocolMode;            /**< Protocol Mode Characteristic handle and properties */    
    CYBLE_HIDSC_REPORT_T bootReport[CYBLE_HIDS_BOOT_REPORT_COUNT];  /**< Boot Report Characteristic info */ 
    CYBLE_HIDSC_REPORT_MAP_T reportMap;             /**< Report Map Characteristic handle and descriptors */   
    CYBLE_SRVR_CHAR_INFO_T information;             /**< Information Characteristic handle and properties */ 
    CYBLE_SRVR_CHAR_INFO_T controlPoint;            /**< Control Point Characteristic handle and properties */ 
    CYBLE_HIDSC_REPORT_T report[CYBLE_HIDSC_REPORT_COUNT];          /**< Report Characteristic info */ 
    uint8 reportCount;                              /**< Number of report Characteristics */
    CYBLE_GATT_DB_ATTR_HANDLE_T includeHandle;      /**< Included declaration handle */
} CYBLE_HIDSC_T;

/** Characteristic Value Write Sub-Procedure supported by HID Service */
typedef enum
{
    CYBLE_HIDSC_WRITE_WITHOUT_RESPONSE,              /**< Write Without Response */
    CYBLE_HIDSC_WRITE_CHAR_VALUE                     /**< Write Characteristic Value */
}CYBLE_HIDSC_CHAR_WRITE_T;

/** Characteristic Value Read Sub-Procedure supported by HID Service */
typedef enum
{
    CYBLE_HIDSC_READ_CHAR_VALUE,                    /**< Read Characteristic Value */
    CYBLE_HIDSC_READ_LONG_CHAR_VALUE                /**< Read Long Characteristic Values */
}CYBLE_HIDSC_CHAR_READ_T;

#endif /* (CYBLE_HIDS_CLIENT) */

/** HID Service Characteristic value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;         /**< Peer device handle */
    uint8 serviceIndex;                     /**< Index of HID Service */
    CYBLE_HIDS_CHAR_INDEX_T charIndex;      /**< Index of HID Service Characteristic */
    CYBLE_GATT_VALUE_T *value;              /**< Pointer to Characteristic value */
} CYBLE_HIDS_CHAR_VALUE_T;

/** HID Service Characteristic descriptor value parameter structure */
typedef struct
{
    CYBLE_CONN_HANDLE_T connHandle;         /**< Peer device handle */
    uint8 serviceIndex;                     /**< Index of HID Service */
    CYBLE_HIDS_CHAR_INDEX_T charIndex;      /**< Index of HID Service Characteristic */
    CYBLE_HIDS_DESCR_T descrIndex;          /**< Service Characteristic Descriptor index */
    CYBLE_GATT_VALUE_T *value;              /**< Pointer to value of Service Characteristic Descriptor value */
} CYBLE_HIDS_DESCR_VALUE_T;

/** @} */

/***************************************  
* API Constants
***************************************/

/* Boot Report indexes */
#define CYBLE_HIDS_BOOT_KYBRD_IN_REP_INDX               (0x00u)
#define CYBLE_HIDS_BOOT_KYBRD_OUT_REP_INDX              (0x01u)
#define CYBLE_HIDS_BOOT_MOUSE_IN_REP_INDX               (0x02u)

/* Report Type values */
#define CYBLE_HIDS_PROTOCOL_MODE_BOOT                   (0x00u)
#define CYBLE_HIDS_PROTOCOL_MODE_REPORT                 (0x01u)
#define CYBLE_HIDS_PROTOCOL_DEFAULT                     CYBLE_HIDS_PROTOCOL_MODE_REPORT

/* Report Type values */
#define CYBLE_HIDS_REPORT_TYPE_INPUT                    (0x01u)
#define CYBLE_HIDS_REPORT_TYPE_OUTPUT                   (0x02u)
#define CYBLE_HIDS_REPORT_TYPE_FEATURE                  (0x03u)

/* Control-point attributes */
#define CYBLE_HIDS_CP_SUSPEND                           (0x00u)
#define CYBLE_HIDS_CP_EXIT_SUSPEND                      (0x01u)

/* Information charasteristic value flags */
#define CYBLE_HIDS_INFO_FLAG_REMOTE_WAKE_MASK           (0x01u)
#define CYBLE_HIDS_INFO_FLAG_NORMALLY_CONNECTABLE_MASK  (0x02u)

#define CYBLE_HIDS_PM_CHAR_LEN                          (0x01u)
#define CYBLE_HIDS_CP_CHAR_LEN                          (0x01u)
#define CYBLE_HIDS_RRD_LEN                              (0x02u)

#define CYBLE_HIDSC_HOST_ROLE_BOOT                      (0x00u)
#define CYBLE_HIDSC_HOST_ROLE_REPORT                    (0x01u)
#define CYBLE_HIDSC_HOST_ROLE_BOOT_AND_REPORT           (0x02u)


/***************************************  
* Function Prototypes
***************************************/
/** 
 \addtogroup group_service_api_HIDS_server_client 
 @{ 
*/
void CyBle_HidsRegisterAttrCallback(CYBLE_CALLBACK_T callbackFunc);
/** @} */

#ifdef CYBLE_HIDS_SERVER
/**
 \addtogroup group_service_api_HIDS_server
 @{
*/

CYBLE_API_RESULT_T CyBle_HidssSetCharacteristicValue(uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HidssGetCharacteristicValue(uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex,
    uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HidssGetCharacteristicDescriptor(uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex,
    CYBLE_HIDS_DESCR_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HidssSendNotification(CYBLE_CONN_HANDLE_T connHandle,
    uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex, uint8 attrSize, uint8 *attrValue);

/** @} */
#endif /* CYBLE_HIDS_SERVER */

#ifdef CYBLE_HIDS_CLIENT
/**
 \addtogroup group_service_api_HIDS_client
 @{
*/

CYBLE_API_RESULT_T CyBle_HidscSetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
    CYBLE_HIDSC_CHAR_WRITE_T subProcedure, uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex, 
    uint8 attrSize, uint8 * attrValue);
CYBLE_API_RESULT_T CyBle_HidscGetCharacteristicValue(CYBLE_CONN_HANDLE_T connHandle, 
    CYBLE_HIDSC_CHAR_READ_T subProcedure, uint8 serviceIndex, CYBLE_HIDS_CHAR_INDEX_T charIndex);
CYBLE_API_RESULT_T CyBle_HidscSetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_HIDS_CHAR_INDEX_T charIndex, CYBLE_HIDS_DESCR_T descrIndex, uint8 attrSize, uint8 *attrValue);
CYBLE_API_RESULT_T CyBle_HidscGetCharacteristicDescriptor(CYBLE_CONN_HANDLE_T connHandle, uint8 serviceIndex,
    CYBLE_HIDS_CHAR_INDEX_T charIndex, CYBLE_HIDS_DESCR_T descrIndex);

/** @} */
#endif /* CYBLE_HIDS_CLIENT */


/***************************************  
* Private Function Prototypes
***************************************/

/** \cond IGNORE */
void CyBle_HidsInit(void);

#ifdef CYBLE_HIDS_SERVER

CYBLE_GATT_ERR_CODE_T CyBle_HidssWriteEventHandler(CYBLE_GATTS_WRITE_REQ_PARAM_T *eventParam);

void CyBle_HidssOnDeviceConnected(void);

#endif /* CYBLE_HIDS_SERVER */

#ifdef CYBLE_HIDS_CLIENT

void CyBle_HidscDiscoverCharacteristicsEventHandler(uint16 discoveryService,
    CYBLE_DISC_CHAR_INFO_T *discCharInfo);
void CyBle_HidscDiscoverCharDescriptorsEventHandler(uint16 discoveryService, uint16 discoveryChar,
    CYBLE_DISC_DESCR_INFO_T *discDescrInfo);
void CyBle_HidscNotificationEventHandler(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *eventParam);
void CyBle_HidscReadResponseEventHandler(CYBLE_GATTC_READ_RSP_PARAM_T *eventParam);
void CyBle_HidscWriteResponseEventHandler(const CYBLE_CONN_HANDLE_T *eventParam);
void CyBle_HidscErrorResponseEventHandler(const CYBLE_GATTC_ERR_RSP_PARAM_T *eventParam);
void CyBle_HidscInclDiscoveryEventHandler(uint16 discoveryService, const CYBLE_DISC_INCL_INFO_T *discInclInfo);
CYBLE_GATT_DB_ATTR_HANDLE_T CyBle_HidscGetCharacteristicValueHandle(uint8 serviceIndex, 
    CYBLE_HIDS_CHAR_INDEX_T charIndex);
CYBLE_GATT_DB_ATTR_HANDLE_T CyBle_HidscGetCharacteristicDescriptorHandle(uint8 serviceIndex, 
    CYBLE_HIDS_CHAR_INDEX_T charIndex, CYBLE_HIDS_DESCR_T descrIndex);

#endif /* CYBLE_HIDS_CLIENT */
/** \endcond */


/***************************************  
* Macro Functions
***************************************/


#ifdef CYBLE_HIDS_SERVER

/****************************************************************************** 
* Function Name: CyBle_HIDS_GetProtocolMode
***************************************************************************//**
* 
*  This function returns the current Protocol Mode of the service, as defined by
*  the service index.
* 
*  \param serviceIndex: The sequence number of the HID Service
* 
* \return
*  uint8 protocolMode:
*   * CYBLE_HIDS_PROTOCOL_MODE_BOOT      0   Boot protocol mode
*   * CYBLE_HIDS_PROTOCOL_MODE_REPORT    1   Report Protocol mode
* 
******************************************************************************/
#define CyBle_HidssGetProtocolMode(serviceIndex)\
            ((uint8) *CYBLE_GATT_DB_ATTR_GET_ATTR_GEN_PTR(cyBle_hidss[(serviceIndex)].protocolModeHandle))

#endif /* (CYBLE_HIDS_SERVER) */


/***************************************  
* External data references
***************************************/

#ifdef CYBLE_HIDS_SERVER

extern const CYBLE_HIDSS_T cyBle_hidss[CYBLE_HIDSS_SERVICE_COUNT];
extern const uint16 CYBLE_bootReportUUID[CYBLE_HIDS_BOOT_REPORT_COUNT];

#endif /* CYBLE_HIDS_SERVER */

#ifdef CYBLE_HIDS_CLIENT

extern CYBLE_HIDSC_T cyBle_hidsc[CYBLE_HIDSC_SERVICE_COUNT];
extern uint8 cyBle_hidscServiceCount;

#endif /* CYBLE_HIDS_CLIENT */


#endif /* CY_BLE_CYBLE_HIDS_H  */


/* [] END OF FILE */
