/*******************************************************************************
* File Name: SupervisoryUART.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_SupervisoryUART_H)
#define CY_UART_SupervisoryUART_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define SupervisoryUART_RX_ENABLED                     (1u)
#define SupervisoryUART_TX_ENABLED                     (1u)
#define SupervisoryUART_HD_ENABLED                     (0u)
#define SupervisoryUART_RX_INTERRUPT_ENABLED           (0u)
#define SupervisoryUART_TX_INTERRUPT_ENABLED           (0u)
#define SupervisoryUART_INTERNAL_CLOCK_USED            (1u)
#define SupervisoryUART_RXHW_ADDRESS_ENABLED           (0u)
#define SupervisoryUART_OVER_SAMPLE_COUNT              (8u)
#define SupervisoryUART_PARITY_TYPE                    (0u)
#define SupervisoryUART_PARITY_TYPE_SW                 (0u)
#define SupervisoryUART_BREAK_DETECT                   (0u)
#define SupervisoryUART_BREAK_BITS_TX                  (13u)
#define SupervisoryUART_BREAK_BITS_RX                  (13u)
#define SupervisoryUART_TXCLKGEN_DP                    (1u)
#define SupervisoryUART_USE23POLLING                   (1u)
#define SupervisoryUART_FLOW_CONTROL                   (0u)
#define SupervisoryUART_CLK_FREQ                       (0u)
#define SupervisoryUART_TX_BUFFER_SIZE                 (4u)
#define SupervisoryUART_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define SupervisoryUART_CONTROL_REG_REMOVED            (0u)
#else
    #define SupervisoryUART_CONTROL_REG_REMOVED            (1u)
#endif /* End SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct SupervisoryUART_backupStruct_
{
    uint8 enableState;

    #if(SupervisoryUART_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End SupervisoryUART_CONTROL_REG_REMOVED */

} SupervisoryUART_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void SupervisoryUART_Start(void) ;
void SupervisoryUART_Stop(void) ;
uint8 SupervisoryUART_ReadControlRegister(void) ;
void SupervisoryUART_WriteControlRegister(uint8 control) ;

void SupervisoryUART_Init(void) ;
void SupervisoryUART_Enable(void) ;
void SupervisoryUART_SaveConfig(void) ;
void SupervisoryUART_RestoreConfig(void) ;
void SupervisoryUART_Sleep(void) ;
void SupervisoryUART_Wakeup(void) ;

/* Only if RX is enabled */
#if( (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) )

    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
        #define SupervisoryUART_EnableRxInt()  CyIntEnable (SupervisoryUART_RX_VECT_NUM)
        #define SupervisoryUART_DisableRxInt() CyIntDisable(SupervisoryUART_RX_VECT_NUM)
        CY_ISR_PROTO(SupervisoryUART_RXISR);
    #endif /* SupervisoryUART_RX_INTERRUPT_ENABLED */

    void SupervisoryUART_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void SupervisoryUART_SetRxAddress1(uint8 address) ;
    void SupervisoryUART_SetRxAddress2(uint8 address) ;

    void  SupervisoryUART_SetRxInterruptMode(uint8 intSrc) ;
    uint8 SupervisoryUART_ReadRxData(void) ;
    uint8 SupervisoryUART_ReadRxStatus(void) ;
    uint8 SupervisoryUART_GetChar(void) ;
    uint16 SupervisoryUART_GetByte(void) ;
    uint8 SupervisoryUART_GetRxBufferSize(void)
                                                            ;
    void SupervisoryUART_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define SupervisoryUART_GetRxInterruptSource   SupervisoryUART_ReadRxStatus

#endif /* End (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) */

/* Only if TX is enabled */
#if(SupervisoryUART_TX_ENABLED || SupervisoryUART_HD_ENABLED)

    #if(SupervisoryUART_TX_INTERRUPT_ENABLED)
        #define SupervisoryUART_EnableTxInt()  CyIntEnable (SupervisoryUART_TX_VECT_NUM)
        #define SupervisoryUART_DisableTxInt() CyIntDisable(SupervisoryUART_TX_VECT_NUM)
        #define SupervisoryUART_SetPendingTxInt() CyIntSetPending(SupervisoryUART_TX_VECT_NUM)
        #define SupervisoryUART_ClearPendingTxInt() CyIntClearPending(SupervisoryUART_TX_VECT_NUM)
        CY_ISR_PROTO(SupervisoryUART_TXISR);
    #endif /* SupervisoryUART_TX_INTERRUPT_ENABLED */

    void SupervisoryUART_SetTxInterruptMode(uint8 intSrc) ;
    void SupervisoryUART_WriteTxData(uint8 txDataByte) ;
    uint8 SupervisoryUART_ReadTxStatus(void) ;
    void SupervisoryUART_PutChar(uint8 txDataByte) ;
    void SupervisoryUART_PutString(const char8 string[]) ;
    void SupervisoryUART_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void SupervisoryUART_PutCRLF(uint8 txDataByte) ;
    void SupervisoryUART_ClearTxBuffer(void) ;
    void SupervisoryUART_SetTxAddressMode(uint8 addressMode) ;
    void SupervisoryUART_SendBreak(uint8 retMode) ;
    uint8 SupervisoryUART_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define SupervisoryUART_PutStringConst         SupervisoryUART_PutString
    #define SupervisoryUART_PutArrayConst          SupervisoryUART_PutArray
    #define SupervisoryUART_GetTxInterruptSource   SupervisoryUART_ReadTxStatus

#endif /* End SupervisoryUART_TX_ENABLED || SupervisoryUART_HD_ENABLED */

#if(SupervisoryUART_HD_ENABLED)
    void SupervisoryUART_LoadRxConfig(void) ;
    void SupervisoryUART_LoadTxConfig(void) ;
#endif /* End SupervisoryUART_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SupervisoryUART) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    SupervisoryUART_CyBtldrCommStart(void) CYSMALL ;
    void    SupervisoryUART_CyBtldrCommStop(void) CYSMALL ;
    void    SupervisoryUART_CyBtldrCommReset(void) CYSMALL ;
    cystatus SupervisoryUART_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus SupervisoryUART_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SupervisoryUART)
        #define CyBtldrCommStart    SupervisoryUART_CyBtldrCommStart
        #define CyBtldrCommStop     SupervisoryUART_CyBtldrCommStop
        #define CyBtldrCommReset    SupervisoryUART_CyBtldrCommReset
        #define CyBtldrCommWrite    SupervisoryUART_CyBtldrCommWrite
        #define CyBtldrCommRead     SupervisoryUART_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SupervisoryUART) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define SupervisoryUART_BYTE2BYTE_TIME_OUT (25u)
    #define SupervisoryUART_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define SupervisoryUART_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define SupervisoryUART_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define SupervisoryUART_SET_SPACE      (0x00u)
#define SupervisoryUART_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (SupervisoryUART_TX_ENABLED) || (SupervisoryUART_HD_ENABLED) )
    #if(SupervisoryUART_TX_INTERRUPT_ENABLED)
        #define SupervisoryUART_TX_VECT_NUM            (uint8)SupervisoryUART_TXInternalInterrupt__INTC_NUMBER
        #define SupervisoryUART_TX_PRIOR_NUM           (uint8)SupervisoryUART_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* SupervisoryUART_TX_INTERRUPT_ENABLED */

    #define SupervisoryUART_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define SupervisoryUART_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define SupervisoryUART_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(SupervisoryUART_TX_ENABLED)
        #define SupervisoryUART_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (SupervisoryUART_HD_ENABLED) */
        #define SupervisoryUART_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (SupervisoryUART_TX_ENABLED) */

    #define SupervisoryUART_TX_STS_COMPLETE            (uint8)(0x01u << SupervisoryUART_TX_STS_COMPLETE_SHIFT)
    #define SupervisoryUART_TX_STS_FIFO_EMPTY          (uint8)(0x01u << SupervisoryUART_TX_STS_FIFO_EMPTY_SHIFT)
    #define SupervisoryUART_TX_STS_FIFO_FULL           (uint8)(0x01u << SupervisoryUART_TX_STS_FIFO_FULL_SHIFT)
    #define SupervisoryUART_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << SupervisoryUART_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (SupervisoryUART_TX_ENABLED) || (SupervisoryUART_HD_ENABLED)*/

#if( (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) )
    #if(SupervisoryUART_RX_INTERRUPT_ENABLED)
        #define SupervisoryUART_RX_VECT_NUM            (uint8)SupervisoryUART_RXInternalInterrupt__INTC_NUMBER
        #define SupervisoryUART_RX_PRIOR_NUM           (uint8)SupervisoryUART_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* SupervisoryUART_RX_INTERRUPT_ENABLED */
    #define SupervisoryUART_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define SupervisoryUART_RX_STS_BREAK_SHIFT             (0x01u)
    #define SupervisoryUART_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define SupervisoryUART_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define SupervisoryUART_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define SupervisoryUART_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define SupervisoryUART_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define SupervisoryUART_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define SupervisoryUART_RX_STS_MRKSPC           (uint8)(0x01u << SupervisoryUART_RX_STS_MRKSPC_SHIFT)
    #define SupervisoryUART_RX_STS_BREAK            (uint8)(0x01u << SupervisoryUART_RX_STS_BREAK_SHIFT)
    #define SupervisoryUART_RX_STS_PAR_ERROR        (uint8)(0x01u << SupervisoryUART_RX_STS_PAR_ERROR_SHIFT)
    #define SupervisoryUART_RX_STS_STOP_ERROR       (uint8)(0x01u << SupervisoryUART_RX_STS_STOP_ERROR_SHIFT)
    #define SupervisoryUART_RX_STS_OVERRUN          (uint8)(0x01u << SupervisoryUART_RX_STS_OVERRUN_SHIFT)
    #define SupervisoryUART_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << SupervisoryUART_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define SupervisoryUART_RX_STS_ADDR_MATCH       (uint8)(0x01u << SupervisoryUART_RX_STS_ADDR_MATCH_SHIFT)
    #define SupervisoryUART_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << SupervisoryUART_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define SupervisoryUART_RX_HW_MASK                     (0x7Fu)
#endif /* End (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) */

/* Control Register definitions */
#define SupervisoryUART_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define SupervisoryUART_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define SupervisoryUART_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define SupervisoryUART_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define SupervisoryUART_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define SupervisoryUART_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define SupervisoryUART_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define SupervisoryUART_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define SupervisoryUART_CTRL_HD_SEND               (uint8)(0x01u << SupervisoryUART_CTRL_HD_SEND_SHIFT)
#define SupervisoryUART_CTRL_HD_SEND_BREAK         (uint8)(0x01u << SupervisoryUART_CTRL_HD_SEND_BREAK_SHIFT)
#define SupervisoryUART_CTRL_MARK                  (uint8)(0x01u << SupervisoryUART_CTRL_MARK_SHIFT)
#define SupervisoryUART_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << SupervisoryUART_CTRL_PARITY_TYPE0_SHIFT)
#define SupervisoryUART_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << SupervisoryUART_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define SupervisoryUART_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define SupervisoryUART_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define SupervisoryUART_SEND_BREAK                         (0x00u)
#define SupervisoryUART_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define SupervisoryUART_REINIT                             (0x02u)
#define SupervisoryUART_SEND_WAIT_REINIT                   (0x03u)

#define SupervisoryUART_OVER_SAMPLE_8                      (8u)
#define SupervisoryUART_OVER_SAMPLE_16                     (16u)

#define SupervisoryUART_BIT_CENTER                         (SupervisoryUART_OVER_SAMPLE_COUNT - 2u)

#define SupervisoryUART_FIFO_LENGTH                        (4u)
#define SupervisoryUART_NUMBER_OF_START_BIT                (1u)
#define SupervisoryUART_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define SupervisoryUART_TXBITCTR_BREAKBITS8X   ((SupervisoryUART_BREAK_BITS_TX * SupervisoryUART_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define SupervisoryUART_TXBITCTR_BREAKBITS ((SupervisoryUART_BREAK_BITS_TX * SupervisoryUART_OVER_SAMPLE_COUNT) - 1u)

#define SupervisoryUART_HALF_BIT_COUNT   \
                            (((SupervisoryUART_OVER_SAMPLE_COUNT / 2u) + (SupervisoryUART_USE23POLLING * 1u)) - 2u)
#if (SupervisoryUART_OVER_SAMPLE_COUNT == SupervisoryUART_OVER_SAMPLE_8)
    #define SupervisoryUART_HD_TXBITCTR_INIT   (((SupervisoryUART_BREAK_BITS_TX + \
                            SupervisoryUART_NUMBER_OF_START_BIT) * SupervisoryUART_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define SupervisoryUART_RXBITCTR_INIT  ((((SupervisoryUART_BREAK_BITS_RX + SupervisoryUART_NUMBER_OF_START_BIT) \
                            * SupervisoryUART_OVER_SAMPLE_COUNT) + SupervisoryUART_HALF_BIT_COUNT) - 1u)

#else /* SupervisoryUART_OVER_SAMPLE_COUNT == SupervisoryUART_OVER_SAMPLE_16 */
    #define SupervisoryUART_HD_TXBITCTR_INIT   ((8u * SupervisoryUART_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define SupervisoryUART_RXBITCTR_INIT      (((7u * SupervisoryUART_OVER_SAMPLE_COUNT) - 1u) + \
                                                      SupervisoryUART_HALF_BIT_COUNT)
#endif /* End SupervisoryUART_OVER_SAMPLE_COUNT */

#define SupervisoryUART_HD_RXBITCTR_INIT                   SupervisoryUART_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 SupervisoryUART_initVar;
#if (SupervisoryUART_TX_INTERRUPT_ENABLED && SupervisoryUART_TX_ENABLED)
    extern volatile uint8 SupervisoryUART_txBuffer[SupervisoryUART_TX_BUFFER_SIZE];
    extern volatile uint8 SupervisoryUART_txBufferRead;
    extern uint8 SupervisoryUART_txBufferWrite;
#endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED && SupervisoryUART_TX_ENABLED) */
#if (SupervisoryUART_RX_INTERRUPT_ENABLED && (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED))
    extern uint8 SupervisoryUART_errorStatus;
    extern volatile uint8 SupervisoryUART_rxBuffer[SupervisoryUART_RX_BUFFER_SIZE];
    extern volatile uint8 SupervisoryUART_rxBufferRead;
    extern volatile uint8 SupervisoryUART_rxBufferWrite;
    extern volatile uint8 SupervisoryUART_rxBufferLoopDetect;
    extern volatile uint8 SupervisoryUART_rxBufferOverflow;
    #if (SupervisoryUART_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 SupervisoryUART_rxAddressMode;
        extern volatile uint8 SupervisoryUART_rxAddressDetected;
    #endif /* (SupervisoryUART_RXHW_ADDRESS_ENABLED) */
#endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED && (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define SupervisoryUART__B_UART__AM_SW_BYTE_BYTE 1
#define SupervisoryUART__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define SupervisoryUART__B_UART__AM_HW_BYTE_BY_BYTE 3
#define SupervisoryUART__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define SupervisoryUART__B_UART__AM_NONE 0

#define SupervisoryUART__B_UART__NONE_REVB 0
#define SupervisoryUART__B_UART__EVEN_REVB 1
#define SupervisoryUART__B_UART__ODD_REVB 2
#define SupervisoryUART__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define SupervisoryUART_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define SupervisoryUART_NUMBER_OF_STOP_BITS    (1u)

#if (SupervisoryUART_RXHW_ADDRESS_ENABLED)
    #define SupervisoryUART_RX_ADDRESS_MODE    (0u)
    #define SupervisoryUART_RX_HW_ADDRESS1     (0u)
    #define SupervisoryUART_RX_HW_ADDRESS2     (0u)
#endif /* (SupervisoryUART_RXHW_ADDRESS_ENABLED) */

#define SupervisoryUART_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << SupervisoryUART_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << SupervisoryUART_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << SupervisoryUART_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << SupervisoryUART_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << SupervisoryUART_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << SupervisoryUART_RX_STS_BREAK_SHIFT) \
                                        | (0 << SupervisoryUART_RX_STS_OVERRUN_SHIFT))

#define SupervisoryUART_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << SupervisoryUART_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << SupervisoryUART_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << SupervisoryUART_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << SupervisoryUART_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define SupervisoryUART_CONTROL_REG \
                            (* (reg8 *) SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define SupervisoryUART_CONTROL_PTR \
                            (  (reg8 *) SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(SupervisoryUART_TX_ENABLED)
    #define SupervisoryUART_TXDATA_REG          (* (reg8 *) SupervisoryUART_BUART_sTX_TxShifter_u0__F0_REG)
    #define SupervisoryUART_TXDATA_PTR          (  (reg8 *) SupervisoryUART_BUART_sTX_TxShifter_u0__F0_REG)
    #define SupervisoryUART_TXDATA_AUX_CTL_REG  (* (reg8 *) SupervisoryUART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define SupervisoryUART_TXDATA_AUX_CTL_PTR  (  (reg8 *) SupervisoryUART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define SupervisoryUART_TXSTATUS_REG        (* (reg8 *) SupervisoryUART_BUART_sTX_TxSts__STATUS_REG)
    #define SupervisoryUART_TXSTATUS_PTR        (  (reg8 *) SupervisoryUART_BUART_sTX_TxSts__STATUS_REG)
    #define SupervisoryUART_TXSTATUS_MASK_REG   (* (reg8 *) SupervisoryUART_BUART_sTX_TxSts__MASK_REG)
    #define SupervisoryUART_TXSTATUS_MASK_PTR   (  (reg8 *) SupervisoryUART_BUART_sTX_TxSts__MASK_REG)
    #define SupervisoryUART_TXSTATUS_ACTL_REG   (* (reg8 *) SupervisoryUART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define SupervisoryUART_TXSTATUS_ACTL_PTR   (  (reg8 *) SupervisoryUART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(SupervisoryUART_TXCLKGEN_DP)
        #define SupervisoryUART_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define SupervisoryUART_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define SupervisoryUART_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define SupervisoryUART_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define SupervisoryUART_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define SupervisoryUART_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define SupervisoryUART_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define SupervisoryUART_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define SupervisoryUART_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define SupervisoryUART_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) SupervisoryUART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* SupervisoryUART_TXCLKGEN_DP */

#endif /* End SupervisoryUART_TX_ENABLED */

#if(SupervisoryUART_HD_ENABLED)

    #define SupervisoryUART_TXDATA_REG             (* (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__F1_REG )
    #define SupervisoryUART_TXDATA_PTR             (  (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__F1_REG )
    #define SupervisoryUART_TXDATA_AUX_CTL_REG     (* (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define SupervisoryUART_TXDATA_AUX_CTL_PTR     (  (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define SupervisoryUART_TXSTATUS_REG           (* (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_REG )
    #define SupervisoryUART_TXSTATUS_PTR           (  (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_REG )
    #define SupervisoryUART_TXSTATUS_MASK_REG      (* (reg8 *) SupervisoryUART_BUART_sRX_RxSts__MASK_REG )
    #define SupervisoryUART_TXSTATUS_MASK_PTR      (  (reg8 *) SupervisoryUART_BUART_sRX_RxSts__MASK_REG )
    #define SupervisoryUART_TXSTATUS_ACTL_REG      (* (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define SupervisoryUART_TXSTATUS_ACTL_PTR      (  (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End SupervisoryUART_HD_ENABLED */

#if( (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) )
    #define SupervisoryUART_RXDATA_REG             (* (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__F0_REG )
    #define SupervisoryUART_RXDATA_PTR             (  (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__F0_REG )
    #define SupervisoryUART_RXADDRESS1_REG         (* (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__D0_REG )
    #define SupervisoryUART_RXADDRESS1_PTR         (  (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__D0_REG )
    #define SupervisoryUART_RXADDRESS2_REG         (* (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__D1_REG )
    #define SupervisoryUART_RXADDRESS2_PTR         (  (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__D1_REG )
    #define SupervisoryUART_RXDATA_AUX_CTL_REG     (* (reg8 *) SupervisoryUART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define SupervisoryUART_RXBITCTR_PERIOD_REG    (* (reg8 *) SupervisoryUART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define SupervisoryUART_RXBITCTR_PERIOD_PTR    (  (reg8 *) SupervisoryUART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define SupervisoryUART_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) SupervisoryUART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define SupervisoryUART_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) SupervisoryUART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define SupervisoryUART_RXBITCTR_COUNTER_REG   (* (reg8 *) SupervisoryUART_BUART_sRX_RxBitCounter__COUNT_REG )
    #define SupervisoryUART_RXBITCTR_COUNTER_PTR   (  (reg8 *) SupervisoryUART_BUART_sRX_RxBitCounter__COUNT_REG )

    #define SupervisoryUART_RXSTATUS_REG           (* (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_REG )
    #define SupervisoryUART_RXSTATUS_PTR           (  (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_REG )
    #define SupervisoryUART_RXSTATUS_MASK_REG      (* (reg8 *) SupervisoryUART_BUART_sRX_RxSts__MASK_REG )
    #define SupervisoryUART_RXSTATUS_MASK_PTR      (  (reg8 *) SupervisoryUART_BUART_sRX_RxSts__MASK_REG )
    #define SupervisoryUART_RXSTATUS_ACTL_REG      (* (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define SupervisoryUART_RXSTATUS_ACTL_PTR      (  (reg8 *) SupervisoryUART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) */

#if(SupervisoryUART_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define SupervisoryUART_INTCLOCK_CLKEN_REG     (* (reg8 *) SupervisoryUART_IntClock__PM_ACT_CFG)
    #define SupervisoryUART_INTCLOCK_CLKEN_PTR     (  (reg8 *) SupervisoryUART_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define SupervisoryUART_INTCLOCK_CLKEN_MASK    SupervisoryUART_IntClock__PM_ACT_MSK
#endif /* End SupervisoryUART_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(SupervisoryUART_TX_ENABLED)
    #define SupervisoryUART_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End SupervisoryUART_TX_ENABLED */

#if(SupervisoryUART_HD_ENABLED)
    #define SupervisoryUART_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End SupervisoryUART_HD_ENABLED */

#if( (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) )
    #define SupervisoryUART_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define SupervisoryUART_WAIT_1_MS      SupervisoryUART_BL_CHK_DELAY_MS   

#define SupervisoryUART_TXBUFFERSIZE   SupervisoryUART_TX_BUFFER_SIZE
#define SupervisoryUART_RXBUFFERSIZE   SupervisoryUART_RX_BUFFER_SIZE

#if (SupervisoryUART_RXHW_ADDRESS_ENABLED)
    #define SupervisoryUART_RXADDRESSMODE  SupervisoryUART_RX_ADDRESS_MODE
    #define SupervisoryUART_RXHWADDRESS1   SupervisoryUART_RX_HW_ADDRESS1
    #define SupervisoryUART_RXHWADDRESS2   SupervisoryUART_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define SupervisoryUART_RXAddressMode  SupervisoryUART_RXADDRESSMODE
#endif /* (SupervisoryUART_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define SupervisoryUART_initvar                    SupervisoryUART_initVar

#define SupervisoryUART_RX_Enabled                 SupervisoryUART_RX_ENABLED
#define SupervisoryUART_TX_Enabled                 SupervisoryUART_TX_ENABLED
#define SupervisoryUART_HD_Enabled                 SupervisoryUART_HD_ENABLED
#define SupervisoryUART_RX_IntInterruptEnabled     SupervisoryUART_RX_INTERRUPT_ENABLED
#define SupervisoryUART_TX_IntInterruptEnabled     SupervisoryUART_TX_INTERRUPT_ENABLED
#define SupervisoryUART_InternalClockUsed          SupervisoryUART_INTERNAL_CLOCK_USED
#define SupervisoryUART_RXHW_Address_Enabled       SupervisoryUART_RXHW_ADDRESS_ENABLED
#define SupervisoryUART_OverSampleCount            SupervisoryUART_OVER_SAMPLE_COUNT
#define SupervisoryUART_ParityType                 SupervisoryUART_PARITY_TYPE

#if( SupervisoryUART_TX_ENABLED && (SupervisoryUART_TXBUFFERSIZE > SupervisoryUART_FIFO_LENGTH))
    #define SupervisoryUART_TXBUFFER               SupervisoryUART_txBuffer
    #define SupervisoryUART_TXBUFFERREAD           SupervisoryUART_txBufferRead
    #define SupervisoryUART_TXBUFFERWRITE          SupervisoryUART_txBufferWrite
#endif /* End SupervisoryUART_TX_ENABLED */
#if( ( SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED ) && \
     (SupervisoryUART_RXBUFFERSIZE > SupervisoryUART_FIFO_LENGTH) )
    #define SupervisoryUART_RXBUFFER               SupervisoryUART_rxBuffer
    #define SupervisoryUART_RXBUFFERREAD           SupervisoryUART_rxBufferRead
    #define SupervisoryUART_RXBUFFERWRITE          SupervisoryUART_rxBufferWrite
    #define SupervisoryUART_RXBUFFERLOOPDETECT     SupervisoryUART_rxBufferLoopDetect
    #define SupervisoryUART_RXBUFFER_OVERFLOW      SupervisoryUART_rxBufferOverflow
#endif /* End SupervisoryUART_RX_ENABLED */

#ifdef SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define SupervisoryUART_CONTROL                SupervisoryUART_CONTROL_REG
#endif /* End SupervisoryUART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(SupervisoryUART_TX_ENABLED)
    #define SupervisoryUART_TXDATA                 SupervisoryUART_TXDATA_REG
    #define SupervisoryUART_TXSTATUS               SupervisoryUART_TXSTATUS_REG
    #define SupervisoryUART_TXSTATUS_MASK          SupervisoryUART_TXSTATUS_MASK_REG
    #define SupervisoryUART_TXSTATUS_ACTL          SupervisoryUART_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(SupervisoryUART_TXCLKGEN_DP)
        #define SupervisoryUART_TXBITCLKGEN_CTR        SupervisoryUART_TXBITCLKGEN_CTR_REG
        #define SupervisoryUART_TXBITCLKTX_COMPLETE    SupervisoryUART_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define SupervisoryUART_TXBITCTR_PERIOD        SupervisoryUART_TXBITCTR_PERIOD_REG
        #define SupervisoryUART_TXBITCTR_CONTROL       SupervisoryUART_TXBITCTR_CONTROL_REG
        #define SupervisoryUART_TXBITCTR_COUNTER       SupervisoryUART_TXBITCTR_COUNTER_REG
    #endif /* SupervisoryUART_TXCLKGEN_DP */
#endif /* End SupervisoryUART_TX_ENABLED */

#if(SupervisoryUART_HD_ENABLED)
    #define SupervisoryUART_TXDATA                 SupervisoryUART_TXDATA_REG
    #define SupervisoryUART_TXSTATUS               SupervisoryUART_TXSTATUS_REG
    #define SupervisoryUART_TXSTATUS_MASK          SupervisoryUART_TXSTATUS_MASK_REG
    #define SupervisoryUART_TXSTATUS_ACTL          SupervisoryUART_TXSTATUS_ACTL_REG
#endif /* End SupervisoryUART_HD_ENABLED */

#if( (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) )
    #define SupervisoryUART_RXDATA                 SupervisoryUART_RXDATA_REG
    #define SupervisoryUART_RXADDRESS1             SupervisoryUART_RXADDRESS1_REG
    #define SupervisoryUART_RXADDRESS2             SupervisoryUART_RXADDRESS2_REG
    #define SupervisoryUART_RXBITCTR_PERIOD        SupervisoryUART_RXBITCTR_PERIOD_REG
    #define SupervisoryUART_RXBITCTR_CONTROL       SupervisoryUART_RXBITCTR_CONTROL_REG
    #define SupervisoryUART_RXBITCTR_COUNTER       SupervisoryUART_RXBITCTR_COUNTER_REG
    #define SupervisoryUART_RXSTATUS               SupervisoryUART_RXSTATUS_REG
    #define SupervisoryUART_RXSTATUS_MASK          SupervisoryUART_RXSTATUS_MASK_REG
    #define SupervisoryUART_RXSTATUS_ACTL          SupervisoryUART_RXSTATUS_ACTL_REG
#endif /* End  (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) */

#if(SupervisoryUART_INTERNAL_CLOCK_USED)
    #define SupervisoryUART_INTCLOCK_CLKEN         SupervisoryUART_INTCLOCK_CLKEN_REG
#endif /* End SupervisoryUART_INTERNAL_CLOCK_USED */

#define SupervisoryUART_WAIT_FOR_COMLETE_REINIT    SupervisoryUART_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_SupervisoryUART_H */


/* [] END OF FILE */
