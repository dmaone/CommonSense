/*******************************************************************************
* File Name: N_UART.h
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


#if !defined(CY_UART_N_UART_H)
#define CY_UART_N_UART_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
* Conditional Compilation Parameters
***************************************/

#define N_UART_RX_ENABLED                     (1u)
#define N_UART_TX_ENABLED                     (1u)
#define N_UART_HD_ENABLED                     (0u)
#define N_UART_RX_INTERRUPT_ENABLED           (0u)
#define N_UART_TX_INTERRUPT_ENABLED           (0u)
#define N_UART_INTERNAL_CLOCK_USED            (1u)
#define N_UART_RXHW_ADDRESS_ENABLED           (0u)
#define N_UART_OVER_SAMPLE_COUNT              (8u)
#define N_UART_PARITY_TYPE                    (0u)
#define N_UART_PARITY_TYPE_SW                 (0u)
#define N_UART_BREAK_DETECT                   (0u)
#define N_UART_BREAK_BITS_TX                  (13u)
#define N_UART_BREAK_BITS_RX                  (13u)
#define N_UART_TXCLKGEN_DP                    (1u)
#define N_UART_USE23POLLING                   (1u)
#define N_UART_FLOW_CONTROL                   (0u)
#define N_UART_CLK_FREQ                       (0u)
#define N_UART_TX_BUFFER_SIZE                 (4u)
#define N_UART_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define N_UART_CONTROL_REG_REMOVED            (0u)
#else
    #define N_UART_CONTROL_REG_REMOVED            (1u)
#endif /* End N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct N_UART_backupStruct_
{
    uint8 enableState;

    #if(N_UART_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End N_UART_CONTROL_REG_REMOVED */

} N_UART_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void N_UART_Start(void) ;
void N_UART_Stop(void) ;
uint8 N_UART_ReadControlRegister(void) ;
void N_UART_WriteControlRegister(uint8 control) ;

void N_UART_Init(void) ;
void N_UART_Enable(void) ;
void N_UART_SaveConfig(void) ;
void N_UART_RestoreConfig(void) ;
void N_UART_Sleep(void) ;
void N_UART_Wakeup(void) ;

/* Only if RX is enabled */
#if( (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) )

    #if (N_UART_RX_INTERRUPT_ENABLED)
        #define N_UART_EnableRxInt()  CyIntEnable (N_UART_RX_VECT_NUM)
        #define N_UART_DisableRxInt() CyIntDisable(N_UART_RX_VECT_NUM)
        CY_ISR_PROTO(N_UART_RXISR);
    #endif /* N_UART_RX_INTERRUPT_ENABLED */

    void N_UART_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void N_UART_SetRxAddress1(uint8 address) ;
    void N_UART_SetRxAddress2(uint8 address) ;

    void  N_UART_SetRxInterruptMode(uint8 intSrc) ;
    uint8 N_UART_ReadRxData(void) ;
    uint8 N_UART_ReadRxStatus(void) ;
    uint8 N_UART_GetChar(void) ;
    uint16 N_UART_GetByte(void) ;
    uint8 N_UART_GetRxBufferSize(void)
                                                            ;
    void N_UART_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define N_UART_GetRxInterruptSource   N_UART_ReadRxStatus

#endif /* End (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) */

/* Only if TX is enabled */
#if(N_UART_TX_ENABLED || N_UART_HD_ENABLED)

    #if(N_UART_TX_INTERRUPT_ENABLED)
        #define N_UART_EnableTxInt()  CyIntEnable (N_UART_TX_VECT_NUM)
        #define N_UART_DisableTxInt() CyIntDisable(N_UART_TX_VECT_NUM)
        #define N_UART_SetPendingTxInt() CyIntSetPending(N_UART_TX_VECT_NUM)
        #define N_UART_ClearPendingTxInt() CyIntClearPending(N_UART_TX_VECT_NUM)
        CY_ISR_PROTO(N_UART_TXISR);
    #endif /* N_UART_TX_INTERRUPT_ENABLED */

    void N_UART_SetTxInterruptMode(uint8 intSrc) ;
    void N_UART_WriteTxData(uint8 txDataByte) ;
    uint8 N_UART_ReadTxStatus(void) ;
    void N_UART_PutChar(uint8 txDataByte) ;
    void N_UART_PutString(const char8 string[]) ;
    void N_UART_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void N_UART_PutCRLF(uint8 txDataByte) ;
    void N_UART_ClearTxBuffer(void) ;
    void N_UART_SetTxAddressMode(uint8 addressMode) ;
    void N_UART_SendBreak(uint8 retMode) ;
    uint8 N_UART_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define N_UART_PutStringConst         N_UART_PutString
    #define N_UART_PutArrayConst          N_UART_PutArray
    #define N_UART_GetTxInterruptSource   N_UART_ReadTxStatus

#endif /* End N_UART_TX_ENABLED || N_UART_HD_ENABLED */

#if(N_UART_HD_ENABLED)
    void N_UART_LoadRxConfig(void) ;
    void N_UART_LoadTxConfig(void) ;
#endif /* End N_UART_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_N_UART) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    N_UART_CyBtldrCommStart(void) CYSMALL ;
    void    N_UART_CyBtldrCommStop(void) CYSMALL ;
    void    N_UART_CyBtldrCommReset(void) CYSMALL ;
    cystatus N_UART_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus N_UART_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_N_UART)
        #define CyBtldrCommStart    N_UART_CyBtldrCommStart
        #define CyBtldrCommStop     N_UART_CyBtldrCommStop
        #define CyBtldrCommReset    N_UART_CyBtldrCommReset
        #define CyBtldrCommWrite    N_UART_CyBtldrCommWrite
        #define CyBtldrCommRead     N_UART_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_N_UART) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define N_UART_BYTE2BYTE_TIME_OUT (25u)
    #define N_UART_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define N_UART_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define N_UART_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define N_UART_SET_SPACE      (0x00u)
#define N_UART_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (N_UART_TX_ENABLED) || (N_UART_HD_ENABLED) )
    #if(N_UART_TX_INTERRUPT_ENABLED)
        #define N_UART_TX_VECT_NUM            (uint8)N_UART_TXInternalInterrupt__INTC_NUMBER
        #define N_UART_TX_PRIOR_NUM           (uint8)N_UART_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* N_UART_TX_INTERRUPT_ENABLED */

    #define N_UART_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define N_UART_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define N_UART_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(N_UART_TX_ENABLED)
        #define N_UART_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (N_UART_HD_ENABLED) */
        #define N_UART_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (N_UART_TX_ENABLED) */

    #define N_UART_TX_STS_COMPLETE            (uint8)(0x01u << N_UART_TX_STS_COMPLETE_SHIFT)
    #define N_UART_TX_STS_FIFO_EMPTY          (uint8)(0x01u << N_UART_TX_STS_FIFO_EMPTY_SHIFT)
    #define N_UART_TX_STS_FIFO_FULL           (uint8)(0x01u << N_UART_TX_STS_FIFO_FULL_SHIFT)
    #define N_UART_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << N_UART_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (N_UART_TX_ENABLED) || (N_UART_HD_ENABLED)*/

#if( (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) )
    #if(N_UART_RX_INTERRUPT_ENABLED)
        #define N_UART_RX_VECT_NUM            (uint8)N_UART_RXInternalInterrupt__INTC_NUMBER
        #define N_UART_RX_PRIOR_NUM           (uint8)N_UART_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* N_UART_RX_INTERRUPT_ENABLED */
    #define N_UART_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define N_UART_RX_STS_BREAK_SHIFT             (0x01u)
    #define N_UART_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define N_UART_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define N_UART_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define N_UART_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define N_UART_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define N_UART_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define N_UART_RX_STS_MRKSPC           (uint8)(0x01u << N_UART_RX_STS_MRKSPC_SHIFT)
    #define N_UART_RX_STS_BREAK            (uint8)(0x01u << N_UART_RX_STS_BREAK_SHIFT)
    #define N_UART_RX_STS_PAR_ERROR        (uint8)(0x01u << N_UART_RX_STS_PAR_ERROR_SHIFT)
    #define N_UART_RX_STS_STOP_ERROR       (uint8)(0x01u << N_UART_RX_STS_STOP_ERROR_SHIFT)
    #define N_UART_RX_STS_OVERRUN          (uint8)(0x01u << N_UART_RX_STS_OVERRUN_SHIFT)
    #define N_UART_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << N_UART_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define N_UART_RX_STS_ADDR_MATCH       (uint8)(0x01u << N_UART_RX_STS_ADDR_MATCH_SHIFT)
    #define N_UART_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << N_UART_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define N_UART_RX_HW_MASK                     (0x7Fu)
#endif /* End (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) */

/* Control Register definitions */
#define N_UART_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define N_UART_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define N_UART_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define N_UART_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define N_UART_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define N_UART_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define N_UART_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define N_UART_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define N_UART_CTRL_HD_SEND               (uint8)(0x01u << N_UART_CTRL_HD_SEND_SHIFT)
#define N_UART_CTRL_HD_SEND_BREAK         (uint8)(0x01u << N_UART_CTRL_HD_SEND_BREAK_SHIFT)
#define N_UART_CTRL_MARK                  (uint8)(0x01u << N_UART_CTRL_MARK_SHIFT)
#define N_UART_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << N_UART_CTRL_PARITY_TYPE0_SHIFT)
#define N_UART_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << N_UART_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define N_UART_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define N_UART_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define N_UART_SEND_BREAK                         (0x00u)
#define N_UART_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define N_UART_REINIT                             (0x02u)
#define N_UART_SEND_WAIT_REINIT                   (0x03u)

#define N_UART_OVER_SAMPLE_8                      (8u)
#define N_UART_OVER_SAMPLE_16                     (16u)

#define N_UART_BIT_CENTER                         (N_UART_OVER_SAMPLE_COUNT - 2u)

#define N_UART_FIFO_LENGTH                        (4u)
#define N_UART_NUMBER_OF_START_BIT                (1u)
#define N_UART_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define N_UART_TXBITCTR_BREAKBITS8X   ((N_UART_BREAK_BITS_TX * N_UART_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define N_UART_TXBITCTR_BREAKBITS ((N_UART_BREAK_BITS_TX * N_UART_OVER_SAMPLE_COUNT) - 1u)

#define N_UART_HALF_BIT_COUNT   \
                            (((N_UART_OVER_SAMPLE_COUNT / 2u) + (N_UART_USE23POLLING * 1u)) - 2u)
#if (N_UART_OVER_SAMPLE_COUNT == N_UART_OVER_SAMPLE_8)
    #define N_UART_HD_TXBITCTR_INIT   (((N_UART_BREAK_BITS_TX + \
                            N_UART_NUMBER_OF_START_BIT) * N_UART_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define N_UART_RXBITCTR_INIT  ((((N_UART_BREAK_BITS_RX + N_UART_NUMBER_OF_START_BIT) \
                            * N_UART_OVER_SAMPLE_COUNT) + N_UART_HALF_BIT_COUNT) - 1u)

#else /* N_UART_OVER_SAMPLE_COUNT == N_UART_OVER_SAMPLE_16 */
    #define N_UART_HD_TXBITCTR_INIT   ((8u * N_UART_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define N_UART_RXBITCTR_INIT      (((7u * N_UART_OVER_SAMPLE_COUNT) - 1u) + \
                                                      N_UART_HALF_BIT_COUNT)
#endif /* End N_UART_OVER_SAMPLE_COUNT */

#define N_UART_HD_RXBITCTR_INIT                   N_UART_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 N_UART_initVar;
#if (N_UART_TX_INTERRUPT_ENABLED && N_UART_TX_ENABLED)
    extern volatile uint8 N_UART_txBuffer[N_UART_TX_BUFFER_SIZE];
    extern volatile uint8 N_UART_txBufferRead;
    extern uint8 N_UART_txBufferWrite;
#endif /* (N_UART_TX_INTERRUPT_ENABLED && N_UART_TX_ENABLED) */
#if (N_UART_RX_INTERRUPT_ENABLED && (N_UART_RX_ENABLED || N_UART_HD_ENABLED))
    extern uint8 N_UART_errorStatus;
    extern volatile uint8 N_UART_rxBuffer[N_UART_RX_BUFFER_SIZE];
    extern volatile uint8 N_UART_rxBufferRead;
    extern volatile uint8 N_UART_rxBufferWrite;
    extern volatile uint8 N_UART_rxBufferLoopDetect;
    extern volatile uint8 N_UART_rxBufferOverflow;
    #if (N_UART_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 N_UART_rxAddressMode;
        extern volatile uint8 N_UART_rxAddressDetected;
    #endif /* (N_UART_RXHW_ADDRESS_ENABLED) */
#endif /* (N_UART_RX_INTERRUPT_ENABLED && (N_UART_RX_ENABLED || N_UART_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define N_UART__B_UART__AM_SW_BYTE_BYTE 1
#define N_UART__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define N_UART__B_UART__AM_HW_BYTE_BY_BYTE 3
#define N_UART__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define N_UART__B_UART__AM_NONE 0

#define N_UART__B_UART__NONE_REVB 0
#define N_UART__B_UART__EVEN_REVB 1
#define N_UART__B_UART__ODD_REVB 2
#define N_UART__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define N_UART_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define N_UART_NUMBER_OF_STOP_BITS    (1u)

#if (N_UART_RXHW_ADDRESS_ENABLED)
    #define N_UART_RX_ADDRESS_MODE    (0u)
    #define N_UART_RX_HW_ADDRESS1     (0u)
    #define N_UART_RX_HW_ADDRESS2     (0u)
#endif /* (N_UART_RXHW_ADDRESS_ENABLED) */

#define N_UART_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((0 << N_UART_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << N_UART_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << N_UART_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << N_UART_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << N_UART_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << N_UART_RX_STS_BREAK_SHIFT) \
                                        | (0 << N_UART_RX_STS_OVERRUN_SHIFT))

#define N_UART_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << N_UART_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << N_UART_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << N_UART_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << N_UART_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define N_UART_CONTROL_REG \
                            (* (reg8 *) N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define N_UART_CONTROL_PTR \
                            (  (reg8 *) N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(N_UART_TX_ENABLED)
    #define N_UART_TXDATA_REG          (* (reg8 *) N_UART_BUART_sTX_TxShifter_u0__F0_REG)
    #define N_UART_TXDATA_PTR          (  (reg8 *) N_UART_BUART_sTX_TxShifter_u0__F0_REG)
    #define N_UART_TXDATA_AUX_CTL_REG  (* (reg8 *) N_UART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define N_UART_TXDATA_AUX_CTL_PTR  (  (reg8 *) N_UART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define N_UART_TXSTATUS_REG        (* (reg8 *) N_UART_BUART_sTX_TxSts__STATUS_REG)
    #define N_UART_TXSTATUS_PTR        (  (reg8 *) N_UART_BUART_sTX_TxSts__STATUS_REG)
    #define N_UART_TXSTATUS_MASK_REG   (* (reg8 *) N_UART_BUART_sTX_TxSts__MASK_REG)
    #define N_UART_TXSTATUS_MASK_PTR   (  (reg8 *) N_UART_BUART_sTX_TxSts__MASK_REG)
    #define N_UART_TXSTATUS_ACTL_REG   (* (reg8 *) N_UART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define N_UART_TXSTATUS_ACTL_PTR   (  (reg8 *) N_UART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(N_UART_TXCLKGEN_DP)
        #define N_UART_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define N_UART_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define N_UART_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define N_UART_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define N_UART_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define N_UART_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define N_UART_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define N_UART_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define N_UART_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define N_UART_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) N_UART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* N_UART_TXCLKGEN_DP */

#endif /* End N_UART_TX_ENABLED */

#if(N_UART_HD_ENABLED)

    #define N_UART_TXDATA_REG             (* (reg8 *) N_UART_BUART_sRX_RxShifter_u0__F1_REG )
    #define N_UART_TXDATA_PTR             (  (reg8 *) N_UART_BUART_sRX_RxShifter_u0__F1_REG )
    #define N_UART_TXDATA_AUX_CTL_REG     (* (reg8 *) N_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define N_UART_TXDATA_AUX_CTL_PTR     (  (reg8 *) N_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define N_UART_TXSTATUS_REG           (* (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_REG )
    #define N_UART_TXSTATUS_PTR           (  (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_REG )
    #define N_UART_TXSTATUS_MASK_REG      (* (reg8 *) N_UART_BUART_sRX_RxSts__MASK_REG )
    #define N_UART_TXSTATUS_MASK_PTR      (  (reg8 *) N_UART_BUART_sRX_RxSts__MASK_REG )
    #define N_UART_TXSTATUS_ACTL_REG      (* (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define N_UART_TXSTATUS_ACTL_PTR      (  (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End N_UART_HD_ENABLED */

#if( (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) )
    #define N_UART_RXDATA_REG             (* (reg8 *) N_UART_BUART_sRX_RxShifter_u0__F0_REG )
    #define N_UART_RXDATA_PTR             (  (reg8 *) N_UART_BUART_sRX_RxShifter_u0__F0_REG )
    #define N_UART_RXADDRESS1_REG         (* (reg8 *) N_UART_BUART_sRX_RxShifter_u0__D0_REG )
    #define N_UART_RXADDRESS1_PTR         (  (reg8 *) N_UART_BUART_sRX_RxShifter_u0__D0_REG )
    #define N_UART_RXADDRESS2_REG         (* (reg8 *) N_UART_BUART_sRX_RxShifter_u0__D1_REG )
    #define N_UART_RXADDRESS2_PTR         (  (reg8 *) N_UART_BUART_sRX_RxShifter_u0__D1_REG )
    #define N_UART_RXDATA_AUX_CTL_REG     (* (reg8 *) N_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define N_UART_RXBITCTR_PERIOD_REG    (* (reg8 *) N_UART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define N_UART_RXBITCTR_PERIOD_PTR    (  (reg8 *) N_UART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define N_UART_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) N_UART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define N_UART_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) N_UART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define N_UART_RXBITCTR_COUNTER_REG   (* (reg8 *) N_UART_BUART_sRX_RxBitCounter__COUNT_REG )
    #define N_UART_RXBITCTR_COUNTER_PTR   (  (reg8 *) N_UART_BUART_sRX_RxBitCounter__COUNT_REG )

    #define N_UART_RXSTATUS_REG           (* (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_REG )
    #define N_UART_RXSTATUS_PTR           (  (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_REG )
    #define N_UART_RXSTATUS_MASK_REG      (* (reg8 *) N_UART_BUART_sRX_RxSts__MASK_REG )
    #define N_UART_RXSTATUS_MASK_PTR      (  (reg8 *) N_UART_BUART_sRX_RxSts__MASK_REG )
    #define N_UART_RXSTATUS_ACTL_REG      (* (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define N_UART_RXSTATUS_ACTL_PTR      (  (reg8 *) N_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) */

#if(N_UART_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define N_UART_INTCLOCK_CLKEN_REG     (* (reg8 *) N_UART_IntClock__PM_ACT_CFG)
    #define N_UART_INTCLOCK_CLKEN_PTR     (  (reg8 *) N_UART_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define N_UART_INTCLOCK_CLKEN_MASK    N_UART_IntClock__PM_ACT_MSK
#endif /* End N_UART_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(N_UART_TX_ENABLED)
    #define N_UART_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End N_UART_TX_ENABLED */

#if(N_UART_HD_ENABLED)
    #define N_UART_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End N_UART_HD_ENABLED */

#if( (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) )
    #define N_UART_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define N_UART_WAIT_1_MS      N_UART_BL_CHK_DELAY_MS   

#define N_UART_TXBUFFERSIZE   N_UART_TX_BUFFER_SIZE
#define N_UART_RXBUFFERSIZE   N_UART_RX_BUFFER_SIZE

#if (N_UART_RXHW_ADDRESS_ENABLED)
    #define N_UART_RXADDRESSMODE  N_UART_RX_ADDRESS_MODE
    #define N_UART_RXHWADDRESS1   N_UART_RX_HW_ADDRESS1
    #define N_UART_RXHWADDRESS2   N_UART_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define N_UART_RXAddressMode  N_UART_RXADDRESSMODE
#endif /* (N_UART_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define N_UART_initvar                    N_UART_initVar

#define N_UART_RX_Enabled                 N_UART_RX_ENABLED
#define N_UART_TX_Enabled                 N_UART_TX_ENABLED
#define N_UART_HD_Enabled                 N_UART_HD_ENABLED
#define N_UART_RX_IntInterruptEnabled     N_UART_RX_INTERRUPT_ENABLED
#define N_UART_TX_IntInterruptEnabled     N_UART_TX_INTERRUPT_ENABLED
#define N_UART_InternalClockUsed          N_UART_INTERNAL_CLOCK_USED
#define N_UART_RXHW_Address_Enabled       N_UART_RXHW_ADDRESS_ENABLED
#define N_UART_OverSampleCount            N_UART_OVER_SAMPLE_COUNT
#define N_UART_ParityType                 N_UART_PARITY_TYPE

#if( N_UART_TX_ENABLED && (N_UART_TXBUFFERSIZE > N_UART_FIFO_LENGTH))
    #define N_UART_TXBUFFER               N_UART_txBuffer
    #define N_UART_TXBUFFERREAD           N_UART_txBufferRead
    #define N_UART_TXBUFFERWRITE          N_UART_txBufferWrite
#endif /* End N_UART_TX_ENABLED */
#if( ( N_UART_RX_ENABLED || N_UART_HD_ENABLED ) && \
     (N_UART_RXBUFFERSIZE > N_UART_FIFO_LENGTH) )
    #define N_UART_RXBUFFER               N_UART_rxBuffer
    #define N_UART_RXBUFFERREAD           N_UART_rxBufferRead
    #define N_UART_RXBUFFERWRITE          N_UART_rxBufferWrite
    #define N_UART_RXBUFFERLOOPDETECT     N_UART_rxBufferLoopDetect
    #define N_UART_RXBUFFER_OVERFLOW      N_UART_rxBufferOverflow
#endif /* End N_UART_RX_ENABLED */

#ifdef N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define N_UART_CONTROL                N_UART_CONTROL_REG
#endif /* End N_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(N_UART_TX_ENABLED)
    #define N_UART_TXDATA                 N_UART_TXDATA_REG
    #define N_UART_TXSTATUS               N_UART_TXSTATUS_REG
    #define N_UART_TXSTATUS_MASK          N_UART_TXSTATUS_MASK_REG
    #define N_UART_TXSTATUS_ACTL          N_UART_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(N_UART_TXCLKGEN_DP)
        #define N_UART_TXBITCLKGEN_CTR        N_UART_TXBITCLKGEN_CTR_REG
        #define N_UART_TXBITCLKTX_COMPLETE    N_UART_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define N_UART_TXBITCTR_PERIOD        N_UART_TXBITCTR_PERIOD_REG
        #define N_UART_TXBITCTR_CONTROL       N_UART_TXBITCTR_CONTROL_REG
        #define N_UART_TXBITCTR_COUNTER       N_UART_TXBITCTR_COUNTER_REG
    #endif /* N_UART_TXCLKGEN_DP */
#endif /* End N_UART_TX_ENABLED */

#if(N_UART_HD_ENABLED)
    #define N_UART_TXDATA                 N_UART_TXDATA_REG
    #define N_UART_TXSTATUS               N_UART_TXSTATUS_REG
    #define N_UART_TXSTATUS_MASK          N_UART_TXSTATUS_MASK_REG
    #define N_UART_TXSTATUS_ACTL          N_UART_TXSTATUS_ACTL_REG
#endif /* End N_UART_HD_ENABLED */

#if( (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) )
    #define N_UART_RXDATA                 N_UART_RXDATA_REG
    #define N_UART_RXADDRESS1             N_UART_RXADDRESS1_REG
    #define N_UART_RXADDRESS2             N_UART_RXADDRESS2_REG
    #define N_UART_RXBITCTR_PERIOD        N_UART_RXBITCTR_PERIOD_REG
    #define N_UART_RXBITCTR_CONTROL       N_UART_RXBITCTR_CONTROL_REG
    #define N_UART_RXBITCTR_COUNTER       N_UART_RXBITCTR_COUNTER_REG
    #define N_UART_RXSTATUS               N_UART_RXSTATUS_REG
    #define N_UART_RXSTATUS_MASK          N_UART_RXSTATUS_MASK_REG
    #define N_UART_RXSTATUS_ACTL          N_UART_RXSTATUS_ACTL_REG
#endif /* End  (N_UART_RX_ENABLED) || (N_UART_HD_ENABLED) */

#if(N_UART_INTERNAL_CLOCK_USED)
    #define N_UART_INTCLOCK_CLKEN         N_UART_INTCLOCK_CLKEN_REG
#endif /* End N_UART_INTERNAL_CLOCK_USED */

#define N_UART_WAIT_FOR_COMLETE_REINIT    N_UART_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_N_UART_H */


/* [] END OF FILE */
