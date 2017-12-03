/***************************************************************************//**
* \file .h
* \version 3.20
*
* \brief
*  This file provides private function prototypes and constants for the 
*  USBFS component. It is not intended to be used in the user project.
*
********************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USB_pvt_H)
#define CY_USBFS_USB_pvt_H

#include "USB.h"
   
#ifdef USB_ENABLE_AUDIO_CLASS
    #include "USB_audio.h"
#endif /* USB_ENABLE_AUDIO_CLASS */

#ifdef USB_ENABLE_CDC_CLASS
    #include "USB_cdc.h"
#endif /* USB_ENABLE_CDC_CLASS */

#if (USB_ENABLE_MIDI_CLASS)
    #include "USB_midi.h"
#endif /* (USB_ENABLE_MIDI_CLASS) */

#if (USB_ENABLE_MSC_CLASS)
    #include "USB_msc.h"
#endif /* (USB_ENABLE_MSC_CLASS) */

#if (USB_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        #include <CyDMA.h>
    #else
        #include <CyDmac.h>
        #if ((USB_EP_MANAGEMENT_DMA_AUTO) && (USB_EP_DMA_AUTO_OPT == 0u))
            #include "USB_EP_DMA_Done_isr.h"
            #include "USB_EP8_DMA_Done_SR.h"
            #include "USB_EP17_DMA_Done_SR.h"
        #endif /* ((USB_EP_MANAGEMENT_DMA_AUTO) && (USB_EP_DMA_AUTO_OPT == 0u)) */
    #endif /* (CY_PSOC4) */
#endif /* (USB_EP_MANAGEMENT_DMA) */

#if (USB_DMA1_ACTIVE)
    #include "USB_ep1_dma.h"
    #define USB_EP1_DMA_CH     (USB_ep1_dma_CHANNEL)
#endif /* (USB_DMA1_ACTIVE) */

#if (USB_DMA2_ACTIVE)
    #include "USB_ep2_dma.h"
    #define USB_EP2_DMA_CH     (USB_ep2_dma_CHANNEL)
#endif /* (USB_DMA2_ACTIVE) */

#if (USB_DMA3_ACTIVE)
    #include "USB_ep3_dma.h"
    #define USB_EP3_DMA_CH     (USB_ep3_dma_CHANNEL)
#endif /* (USB_DMA3_ACTIVE) */

#if (USB_DMA4_ACTIVE)
    #include "USB_ep4_dma.h"
    #define USB_EP4_DMA_CH     (USB_ep4_dma_CHANNEL)
#endif /* (USB_DMA4_ACTIVE) */

#if (USB_DMA5_ACTIVE)
    #include "USB_ep5_dma.h"
    #define USB_EP5_DMA_CH     (USB_ep5_dma_CHANNEL)
#endif /* (USB_DMA5_ACTIVE) */

#if (USB_DMA6_ACTIVE)
    #include "USB_ep6_dma.h"
    #define USB_EP6_DMA_CH     (USB_ep6_dma_CHANNEL)
#endif /* (USB_DMA6_ACTIVE) */

#if (USB_DMA7_ACTIVE)
    #include "USB_ep7_dma.h"
    #define USB_EP7_DMA_CH     (USB_ep7_dma_CHANNEL)
#endif /* (USB_DMA7_ACTIVE) */

#if (USB_DMA8_ACTIVE)
    #include "USB_ep8_dma.h"
    #define USB_EP8_DMA_CH     (USB_ep8_dma_CHANNEL)
#endif /* (USB_DMA8_ACTIVE) */


/***************************************
*     Private Variables
***************************************/

/* Generated external references for descriptors. */
extern const uint8 CYCODE USB_DEVICE0_DESCR[18u];
extern const uint8 CYCODE USB_DEVICE0_CONFIGURATION0_DESCR[41u];
extern const T_USB_LUT CYCODE USB_DEVICE0_CONFIGURATION0_INTERFACE0_TABLE[1u];
extern const T_USB_EP_SETTINGS_BLOCK CYCODE USB_DEVICE0_CONFIGURATION0_EP_SETTINGS_TABLE[2u];
extern const uint8 CYCODE USB_DEVICE0_CONFIGURATION0_INTERFACE_CLASS[1u];
extern const T_USB_LUT CYCODE USB_DEVICE0_CONFIGURATION0_TABLE[4u];
extern const T_USB_LUT CYCODE USB_DEVICE0_TABLE[3u];
extern const T_USB_LUT CYCODE USB_TABLE[1u];
extern const uint8 CYCODE USB_SN_STRING_DESCRIPTOR[10];
extern const uint8 CYCODE USB_STRING_DESCRIPTORS[83u];
extern T_USB_XFER_STATUS_BLOCK USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_RPT_SCB;
extern uint8 USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF[
            USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_BUF_SIZE];
extern T_USB_XFER_STATUS_BLOCK USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_SCB;
extern uint8 USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF[
            USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_BUF_SIZE];
extern const uint8 CYCODE USB_HIDREPORT_DESCRIPTOR1[40u];
extern const T_USB_TD CYCODE USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_IN_RPT_TABLE[1u];
extern const T_USB_TD CYCODE USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_OUT_RPT_TABLE[1u];
extern const T_USB_LUT CYCODE USB_DEVICE0_CONFIGURATION0_INTERFACE0_ALTERNATE0_HID_TABLE[5u];


extern const uint8 CYCODE USB_MSOS_DESCRIPTOR[USB_MSOS_DESCRIPTOR_LENGTH];
extern const uint8 CYCODE USB_MSOS_CONFIGURATION_DESCR[USB_MSOS_CONF_DESCR_LENGTH];
#if defined(USB_ENABLE_IDSN_STRING)
    extern uint8 USB_idSerialNumberStringDescriptor[USB_IDSN_DESCR_LENGTH];
#endif /* (USB_ENABLE_IDSN_STRING) */

extern volatile uint8 USB_interfaceNumber;
extern volatile uint8 USB_interfaceSetting[USB_MAX_INTERFACES_NUMBER];
extern volatile uint8 USB_interfaceSettingLast[USB_MAX_INTERFACES_NUMBER];
extern volatile uint8 USB_deviceAddress;
extern volatile uint8 USB_interfaceStatus[USB_MAX_INTERFACES_NUMBER];
extern const uint8 CYCODE *USB_interfaceClass;

extern volatile T_USB_EP_CTL_BLOCK USB_EP[USB_MAX_EP];
extern volatile T_USB_TD USB_currentTD;

#if (USB_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        extern const uint8 USB_DmaChan[USB_MAX_EP];
    #else
        extern uint8 USB_DmaChan[USB_MAX_EP];
        extern uint8 USB_DmaTd  [USB_MAX_EP];
    #endif /* (CY_PSOC4) */
#endif /* (USB_EP_MANAGEMENT_DMA) */

#if (USB_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    extern uint8  USB_DmaEpBurstCnt   [USB_MAX_EP];
    extern uint8  USB_DmaEpLastBurstEl[USB_MAX_EP];

    extern uint8  USB_DmaEpBurstCntBackup  [USB_MAX_EP];
    extern uint32 USB_DmaEpBufferAddrBackup[USB_MAX_EP];
    
    extern const uint8 USB_DmaReqOut     [USB_MAX_EP];    
    extern const uint8 USB_DmaBurstEndOut[USB_MAX_EP];
#else
    #if (USB_EP_DMA_AUTO_OPT == 0u)
        extern uint8 USB_DmaNextTd[USB_MAX_EP];
        extern volatile uint16 USB_inLength [USB_MAX_EP];
        extern volatile uint8  USB_inBufFull[USB_MAX_EP];
        extern const uint8 USB_epX_TD_TERMOUT_EN[USB_MAX_EP];
        extern const uint8 *USB_inDataPointer[USB_MAX_EP];
    #endif /* (USB_EP_DMA_AUTO_OPT == 0u) */
#endif /* CY_PSOC4 */
#endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */

extern volatile uint8 USB_ep0Toggle;
extern volatile uint8 USB_lastPacketSize;
extern volatile uint8 USB_ep0Mode;
extern volatile uint8 USB_ep0Count;
extern volatile uint16 USB_transferByteCount;


/***************************************
*     Private Function Prototypes
***************************************/
void  USB_ReInitComponent(void)            ;
void  USB_HandleSetup(void)                ;
void  USB_HandleIN(void)                   ;
void  USB_HandleOUT(void)                  ;
void  USB_LoadEP0(void)                    ;
uint8 USB_InitControlRead(void)            ;
uint8 USB_InitControlWrite(void)           ;
void  USB_ControlReadDataStage(void)       ;
void  USB_ControlReadStatusStage(void)     ;
void  USB_ControlReadPrematureStatus(void) ;
uint8 USB_InitControlWrite(void)           ;
uint8 USB_InitZeroLengthControlTransfer(void) ;
void  USB_ControlWriteDataStage(void)      ;
void  USB_ControlWriteStatusStage(void)    ;
void  USB_ControlWritePrematureStatus(void);
uint8 USB_InitNoDataControlTransfer(void)  ;
void  USB_NoDataControlStatusStage(void)   ;
void  USB_InitializeStatusBlock(void)      ;
void  USB_UpdateStatusBlock(uint8 completionCode) ;
uint8 USB_DispatchClassRqst(void)          ;

void USB_Config(uint8 clearAltSetting) ;
void USB_ConfigAltChanged(void)        ;
void USB_ConfigReg(void)               ;
void USB_EpStateInit(void)             ;


const T_USB_LUT CYCODE *USB_GetConfigTablePtr(uint8 confIndex);
const T_USB_LUT CYCODE *USB_GetDeviceTablePtr(void)           ;
#if (USB_BOS_ENABLE)
    const T_USB_LUT CYCODE *USB_GetBOSPtr(void)               ;
#endif /* (USB_BOS_ENABLE) */
const uint8 CYCODE *USB_GetInterfaceClassTablePtr(void)                    ;
uint8 USB_ClearEndpointHalt(void)                                          ;
uint8 USB_SetEndpointHalt(void)                                            ;
uint8 USB_ValidateAlternateSetting(void)                                   ;

void USB_SaveConfig(void)      ;
void USB_RestoreConfig(void)   ;

#if (CY_PSOC3 || CY_PSOC5LP)
    #if (USB_EP_MANAGEMENT_DMA_AUTO && (USB_EP_DMA_AUTO_OPT == 0u))
        void USB_LoadNextInEP(uint8 epNumber, uint8 mode)  ;
    #endif /* (USB_EP_MANAGEMENT_DMA_AUTO && (USB_EP_DMA_AUTO_OPT == 0u)) */
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

#if defined(USB_ENABLE_IDSN_STRING)
    void USB_ReadDieID(uint8 descr[])  ;
#endif /* USB_ENABLE_IDSN_STRING */

#if defined(USB_ENABLE_HID_CLASS)
    uint8 USB_DispatchHIDClassRqst(void) ;
#endif /* (USB_ENABLE_HID_CLASS) */

#if defined(USB_ENABLE_AUDIO_CLASS)
    uint8 USB_DispatchAUDIOClassRqst(void) ;
#endif /* (USB_ENABLE_AUDIO_CLASS) */

#if defined(USB_ENABLE_CDC_CLASS)
    uint8 USB_DispatchCDCClassRqst(void) ;
#endif /* (USB_ENABLE_CDC_CLASS) */

#if (USB_ENABLE_MSC_CLASS)
    #if (USB_HANDLE_MSC_REQUESTS)
        uint8 USB_DispatchMSCClassRqst(void) ;
    #endif /* (USB_HANDLE_MSC_REQUESTS) */
#endif /* (USB_ENABLE_MSC_CLASS */

CY_ISR_PROTO(USB_EP_0_ISR);
CY_ISR_PROTO(USB_BUS_RESET_ISR);

#if (USB_SOF_ISR_ACTIVE)
    CY_ISR_PROTO(USB_SOF_ISR);
#endif /* (USB_SOF_ISR_ACTIVE) */

#if (USB_EP1_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_1_ISR);
#endif /* (USB_EP1_ISR_ACTIVE) */

#if (USB_EP2_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_2_ISR);
#endif /* (USB_EP2_ISR_ACTIVE) */

#if (USB_EP3_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_3_ISR);
#endif /* (USB_EP3_ISR_ACTIVE) */

#if (USB_EP4_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_4_ISR);
#endif /* (USB_EP4_ISR_ACTIVE) */

#if (USB_EP5_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_5_ISR);
#endif /* (USB_EP5_ISR_ACTIVE) */

#if (USB_EP6_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_6_ISR);
#endif /* (USB_EP6_ISR_ACTIVE) */

#if (USB_EP7_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_7_ISR);
#endif /* (USB_EP7_ISR_ACTIVE) */

#if (USB_EP8_ISR_ACTIVE)
    CY_ISR_PROTO(USB_EP_8_ISR);
#endif /* (USB_EP8_ISR_ACTIVE) */

#if (USB_EP_MANAGEMENT_DMA)
    CY_ISR_PROTO(USB_ARB_ISR);
#endif /* (USB_EP_MANAGEMENT_DMA) */

#if (USB_DP_ISR_ACTIVE)
    CY_ISR_PROTO(USB_DP_ISR);
#endif /* (USB_DP_ISR_ACTIVE) */

#if (CY_PSOC4)
    CY_ISR_PROTO(USB_INTR_HI_ISR);
    CY_ISR_PROTO(USB_INTR_MED_ISR);
    CY_ISR_PROTO(USB_INTR_LO_ISR);
    #if (USB_LPM_ACTIVE)
        CY_ISR_PROTO(USB_LPM_ISR);
    #endif /* (USB_LPM_ACTIVE) */
#endif /* (CY_PSOC4) */

#if (USB_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    #if (USB_DMA1_ACTIVE)
        void USB_EP1_DMA_DONE_ISR(void);
    #endif /* (USB_DMA1_ACTIVE) */

    #if (USB_DMA2_ACTIVE)
        void USB_EP2_DMA_DONE_ISR(void);
    #endif /* (USB_DMA2_ACTIVE) */

    #if (USB_DMA3_ACTIVE)
        void USB_EP3_DMA_DONE_ISR(void);
    #endif /* (USB_DMA3_ACTIVE) */

    #if (USB_DMA4_ACTIVE)
        void USB_EP4_DMA_DONE_ISR(void);
    #endif /* (USB_DMA4_ACTIVE) */

    #if (USB_DMA5_ACTIVE)
        void USB_EP5_DMA_DONE_ISR(void);
    #endif /* (USB_DMA5_ACTIVE) */

    #if (USB_DMA6_ACTIVE)
        void USB_EP6_DMA_DONE_ISR(void);
    #endif /* (USB_DMA6_ACTIVE) */

    #if (USB_DMA7_ACTIVE)
        void USB_EP7_DMA_DONE_ISR(void);
    #endif /* (USB_DMA7_ACTIVE) */

    #if (USB_DMA8_ACTIVE)
        void USB_EP8_DMA_DONE_ISR(void);
    #endif /* (USB_DMA8_ACTIVE) */

#else
    #if (USB_EP_DMA_AUTO_OPT == 0u)
        CY_ISR_PROTO(USB_EP_DMA_DONE_ISR);
    #endif /* (USB_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */


/***************************************
*         Request Handlers
***************************************/

uint8 USB_HandleStandardRqst(void) ;
uint8 USB_DispatchClassRqst(void)  ;
uint8 USB_HandleVendorRqst(void)   ;


/***************************************
*    HID Internal references
***************************************/

#if defined(USB_ENABLE_HID_CLASS)
    void USB_FindReport(void)            ;
    void USB_FindReportDescriptor(void)  ;
    void USB_FindHidClassDecriptor(void) ;
#endif /* USB_ENABLE_HID_CLASS */


/***************************************
*    MIDI Internal references
***************************************/

#if defined(USB_ENABLE_MIDI_STREAMING)
    void USB_MIDI_IN_EP_Service(void)  ;
#endif /* (USB_ENABLE_MIDI_STREAMING) */


/***************************************
*    CDC Internal references
***************************************/

#if defined(USB_ENABLE_CDC_CLASS)

    typedef struct
    {
        uint8  bRequestType;
        uint8  bNotification;
        uint8  wValue;
        uint8  wValueMSB;
        uint8  wIndex;
        uint8  wIndexMSB;
        uint8  wLength;
        uint8  wLengthMSB;
        uint8  wSerialState;
        uint8  wSerialStateMSB;
    } t_USB_cdc_notification;

    uint8 USB_GetInterfaceComPort(uint8 interface) ;
    uint8 USB_Cdc_EpInit( const T_USB_EP_SETTINGS_BLOCK CYCODE *pEP, uint8 epNum, uint8 cdcComNums) ;

    extern volatile uint8  USB_cdc_dataInEpList[USB_MAX_MULTI_COM_NUM];
    extern volatile uint8  USB_cdc_dataOutEpList[USB_MAX_MULTI_COM_NUM];
    extern volatile uint8  USB_cdc_commInEpList[USB_MAX_MULTI_COM_NUM];
#endif /* (USB_ENABLE_CDC_CLASS) */


#endif /* CY_USBFS_USB_pvt_H */


/* [] END OF FILE */
