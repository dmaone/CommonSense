/*******************************************************************************
* File Name: SupervisoryUARTINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SupervisoryUART.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (SupervisoryUART_RX_INTERRUPT_ENABLED && (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED))
    /*******************************************************************************
    * Function Name: SupervisoryUART_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_rxBuffer - RAM buffer pointer for save received data.
    *  SupervisoryUART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  SupervisoryUART_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  SupervisoryUART_rxBufferOverflow - software overflow flag. Set to one
    *     when SupervisoryUART_rxBufferWrite index overtakes
    *     SupervisoryUART_rxBufferRead index.
    *  SupervisoryUART_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when SupervisoryUART_rxBufferWrite is equal to
    *    SupervisoryUART_rxBufferRead
    *  SupervisoryUART_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  SupervisoryUART_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(SupervisoryUART_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef SupervisoryUART_RXISR_ENTRY_CALLBACK
        SupervisoryUART_RXISR_EntryCallback();
    #endif /* SupervisoryUART_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START SupervisoryUART_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = SupervisoryUART_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in SupervisoryUART_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (SupervisoryUART_RX_STS_BREAK | 
                            SupervisoryUART_RX_STS_PAR_ERROR |
                            SupervisoryUART_RX_STS_STOP_ERROR | 
                            SupervisoryUART_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                SupervisoryUART_errorStatus |= readStatus & ( SupervisoryUART_RX_STS_BREAK | 
                                                            SupervisoryUART_RX_STS_PAR_ERROR | 
                                                            SupervisoryUART_RX_STS_STOP_ERROR | 
                                                            SupervisoryUART_RX_STS_OVERRUN);
                /* `#START SupervisoryUART_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef SupervisoryUART_RXISR_ERROR_CALLBACK
                SupervisoryUART_RXISR_ERROR_Callback();
            #endif /* SupervisoryUART_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & SupervisoryUART_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = SupervisoryUART_RXDATA_REG;
            #if (SupervisoryUART_RXHW_ADDRESS_ENABLED)
                if(SupervisoryUART_rxAddressMode == (uint8)SupervisoryUART__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & SupervisoryUART_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & SupervisoryUART_RX_STS_ADDR_MATCH) != 0u)
                        {
                            SupervisoryUART_rxAddressDetected = 1u;
                        }
                        else
                        {
                            SupervisoryUART_rxAddressDetected = 0u;
                        }
                    }
                    if(SupervisoryUART_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        SupervisoryUART_rxBuffer[SupervisoryUART_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    SupervisoryUART_rxBuffer[SupervisoryUART_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                SupervisoryUART_rxBuffer[SupervisoryUART_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (SupervisoryUART_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(SupervisoryUART_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        SupervisoryUART_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    SupervisoryUART_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(SupervisoryUART_rxBufferWrite >= SupervisoryUART_RX_BUFFER_SIZE)
                    {
                        SupervisoryUART_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(SupervisoryUART_rxBufferWrite == SupervisoryUART_rxBufferRead)
                    {
                        SupervisoryUART_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (SupervisoryUART_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            SupervisoryUART_RXSTATUS_MASK_REG  &= (uint8)~SupervisoryUART_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(SupervisoryUART_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (SupervisoryUART_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & SupervisoryUART_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START SupervisoryUART_RXISR_END` */

        /* `#END` */

    #ifdef SupervisoryUART_RXISR_EXIT_CALLBACK
        SupervisoryUART_RXISR_ExitCallback();
    #endif /* SupervisoryUART_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED && (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)) */


#if (SupervisoryUART_TX_INTERRUPT_ENABLED && SupervisoryUART_TX_ENABLED)
    /*******************************************************************************
    * Function Name: SupervisoryUART_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_txBuffer - RAM buffer pointer for transmit data from.
    *  SupervisoryUART_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  SupervisoryUART_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(SupervisoryUART_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef SupervisoryUART_TXISR_ENTRY_CALLBACK
        SupervisoryUART_TXISR_EntryCallback();
    #endif /* SupervisoryUART_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START SupervisoryUART_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((SupervisoryUART_txBufferRead != SupervisoryUART_txBufferWrite) &&
             ((SupervisoryUART_TXSTATUS_REG & SupervisoryUART_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(SupervisoryUART_txBufferRead >= SupervisoryUART_TX_BUFFER_SIZE)
            {
                SupervisoryUART_txBufferRead = 0u;
            }

            SupervisoryUART_TXDATA_REG = SupervisoryUART_txBuffer[SupervisoryUART_txBufferRead];

            /* Set next pointer */
            SupervisoryUART_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START SupervisoryUART_TXISR_END` */

        /* `#END` */

    #ifdef SupervisoryUART_TXISR_EXIT_CALLBACK
        SupervisoryUART_TXISR_ExitCallback();
    #endif /* SupervisoryUART_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED && SupervisoryUART_TX_ENABLED) */


/* [] END OF FILE */
