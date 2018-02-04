/*******************************************************************************
* File Name: SupervisoryUART.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SupervisoryUART.h"
#if (SupervisoryUART_INTERNAL_CLOCK_USED)
    #include "SupervisoryUART_IntClock.h"
#endif /* End SupervisoryUART_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 SupervisoryUART_initVar = 0u;

#if (SupervisoryUART_TX_INTERRUPT_ENABLED && SupervisoryUART_TX_ENABLED)
    volatile uint8 SupervisoryUART_txBuffer[SupervisoryUART_TX_BUFFER_SIZE];
    volatile uint8 SupervisoryUART_txBufferRead = 0u;
    uint8 SupervisoryUART_txBufferWrite = 0u;
#endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED && SupervisoryUART_TX_ENABLED) */

#if (SupervisoryUART_RX_INTERRUPT_ENABLED && (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED))
    uint8 SupervisoryUART_errorStatus = 0u;
    volatile uint8 SupervisoryUART_rxBuffer[SupervisoryUART_RX_BUFFER_SIZE];
    volatile uint8 SupervisoryUART_rxBufferRead  = 0u;
    volatile uint8 SupervisoryUART_rxBufferWrite = 0u;
    volatile uint8 SupervisoryUART_rxBufferLoopDetect = 0u;
    volatile uint8 SupervisoryUART_rxBufferOverflow   = 0u;
    #if (SupervisoryUART_RXHW_ADDRESS_ENABLED)
        volatile uint8 SupervisoryUART_rxAddressMode = SupervisoryUART_RX_ADDRESS_MODE;
        volatile uint8 SupervisoryUART_rxAddressDetected = 0u;
    #endif /* (SupervisoryUART_RXHW_ADDRESS_ENABLED) */
#endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED && (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)) */


/*******************************************************************************
* Function Name: SupervisoryUART_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  SupervisoryUART_Start() sets the initVar variable, calls the
*  SupervisoryUART_Init() function, and then calls the
*  SupervisoryUART_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The SupervisoryUART_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time SupervisoryUART_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the SupervisoryUART_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SupervisoryUART_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(SupervisoryUART_initVar == 0u)
    {
        SupervisoryUART_Init();
        SupervisoryUART_initVar = 1u;
    }

    SupervisoryUART_Enable();
}


/*******************************************************************************
* Function Name: SupervisoryUART_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call SupervisoryUART_Init() because
*  the SupervisoryUART_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SupervisoryUART_Init(void) 
{
    #if(SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)

        #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(SupervisoryUART_RX_VECT_NUM, &SupervisoryUART_RXISR);
            CyIntSetPriority(SupervisoryUART_RX_VECT_NUM, SupervisoryUART_RX_PRIOR_NUM);
            SupervisoryUART_errorStatus = 0u;
        #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */

        #if (SupervisoryUART_RXHW_ADDRESS_ENABLED)
            SupervisoryUART_SetRxAddressMode(SupervisoryUART_RX_ADDRESS_MODE);
            SupervisoryUART_SetRxAddress1(SupervisoryUART_RX_HW_ADDRESS1);
            SupervisoryUART_SetRxAddress2(SupervisoryUART_RX_HW_ADDRESS2);
        #endif /* End SupervisoryUART_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        SupervisoryUART_RXBITCTR_PERIOD_REG = SupervisoryUART_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        SupervisoryUART_RXSTATUS_MASK_REG  = SupervisoryUART_INIT_RX_INTERRUPTS_MASK;
    #endif /* End SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED*/

    #if(SupervisoryUART_TX_ENABLED)
        #if (SupervisoryUART_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(SupervisoryUART_TX_VECT_NUM, &SupervisoryUART_TXISR);
            CyIntSetPriority(SupervisoryUART_TX_VECT_NUM, SupervisoryUART_TX_PRIOR_NUM);
        #endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (SupervisoryUART_TXCLKGEN_DP)
            SupervisoryUART_TXBITCLKGEN_CTR_REG = SupervisoryUART_BIT_CENTER;
            SupervisoryUART_TXBITCLKTX_COMPLETE_REG = ((SupervisoryUART_NUMBER_OF_DATA_BITS +
                        SupervisoryUART_NUMBER_OF_START_BIT) * SupervisoryUART_OVER_SAMPLE_COUNT) - 1u;
        #else
            SupervisoryUART_TXBITCTR_PERIOD_REG = ((SupervisoryUART_NUMBER_OF_DATA_BITS +
                        SupervisoryUART_NUMBER_OF_START_BIT) * SupervisoryUART_OVER_SAMPLE_8) - 1u;
        #endif /* End SupervisoryUART_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (SupervisoryUART_TX_INTERRUPT_ENABLED)
            SupervisoryUART_TXSTATUS_MASK_REG = SupervisoryUART_TX_STS_FIFO_EMPTY;
        #else
            SupervisoryUART_TXSTATUS_MASK_REG = SupervisoryUART_INIT_TX_INTERRUPTS_MASK;
        #endif /*End SupervisoryUART_TX_INTERRUPT_ENABLED*/

    #endif /* End SupervisoryUART_TX_ENABLED */

    #if(SupervisoryUART_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        SupervisoryUART_WriteControlRegister( \
            (SupervisoryUART_ReadControlRegister() & (uint8)~SupervisoryUART_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(SupervisoryUART_PARITY_TYPE << SupervisoryUART_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End SupervisoryUART_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: SupervisoryUART_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call SupervisoryUART_Enable() because the SupervisoryUART_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SupervisoryUART_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void SupervisoryUART_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        SupervisoryUART_RXBITCTR_CONTROL_REG |= SupervisoryUART_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        SupervisoryUART_RXSTATUS_ACTL_REG  |= SupervisoryUART_INT_ENABLE;

        #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
            SupervisoryUART_EnableRxInt();

            #if (SupervisoryUART_RXHW_ADDRESS_ENABLED)
                SupervisoryUART_rxAddressDetected = 0u;
            #endif /* (SupervisoryUART_RXHW_ADDRESS_ENABLED) */
        #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */
    #endif /* (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED) */

    #if(SupervisoryUART_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!SupervisoryUART_TXCLKGEN_DP)
            SupervisoryUART_TXBITCTR_CONTROL_REG |= SupervisoryUART_CNTR_ENABLE;
        #endif /* End SupervisoryUART_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        SupervisoryUART_TXSTATUS_ACTL_REG |= SupervisoryUART_INT_ENABLE;
        #if (SupervisoryUART_TX_INTERRUPT_ENABLED)
            SupervisoryUART_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            SupervisoryUART_EnableTxInt();
        #endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED) */
     #endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED) */

    #if (SupervisoryUART_INTERNAL_CLOCK_USED)
        SupervisoryUART_IntClock_Start();  /* Enable the clock */
    #endif /* (SupervisoryUART_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: SupervisoryUART_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SupervisoryUART_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)
        SupervisoryUART_RXBITCTR_CONTROL_REG &= (uint8) ~SupervisoryUART_CNTR_ENABLE;
    #endif /* (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED) */

    #if (SupervisoryUART_TX_ENABLED)
        #if(!SupervisoryUART_TXCLKGEN_DP)
            SupervisoryUART_TXBITCTR_CONTROL_REG &= (uint8) ~SupervisoryUART_CNTR_ENABLE;
        #endif /* (!SupervisoryUART_TXCLKGEN_DP) */
    #endif /* (SupervisoryUART_TX_ENABLED) */

    #if (SupervisoryUART_INTERNAL_CLOCK_USED)
        SupervisoryUART_IntClock_Stop();   /* Disable the clock */
    #endif /* (SupervisoryUART_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)
        SupervisoryUART_RXSTATUS_ACTL_REG  &= (uint8) ~SupervisoryUART_INT_ENABLE;

        #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
            SupervisoryUART_DisableRxInt();
        #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */
    #endif /* (SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED) */

    #if (SupervisoryUART_TX_ENABLED)
        SupervisoryUART_TXSTATUS_ACTL_REG &= (uint8) ~SupervisoryUART_INT_ENABLE;

        #if (SupervisoryUART_TX_INTERRUPT_ENABLED)
            SupervisoryUART_DisableTxInt();
        #endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED) */
    #endif /* (SupervisoryUART_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: SupervisoryUART_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 SupervisoryUART_ReadControlRegister(void) 
{
    #if (SupervisoryUART_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(SupervisoryUART_CONTROL_REG);
    #endif /* (SupervisoryUART_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: SupervisoryUART_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  SupervisoryUART_WriteControlRegister(uint8 control) 
{
    #if (SupervisoryUART_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       SupervisoryUART_CONTROL_REG = control;
    #endif /* (SupervisoryUART_CONTROL_REG_REMOVED) */
}


#if(SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)
    /*******************************************************************************
    * Function Name: SupervisoryUART_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      SupervisoryUART_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      SupervisoryUART_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      SupervisoryUART_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      SupervisoryUART_RX_STS_BREAK            Interrupt on break.
    *      SupervisoryUART_RX_STS_OVERRUN          Interrupt on overrun error.
    *      SupervisoryUART_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      SupervisoryUART_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void SupervisoryUART_SetRxInterruptMode(uint8 intSrc) 
    {
        SupervisoryUART_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  SupervisoryUART_rxBuffer - RAM buffer pointer for save received data.
    *  SupervisoryUART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  SupervisoryUART_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  SupervisoryUART_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 SupervisoryUART_ReadRxData(void) 
    {
        uint8 rxData;

    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        SupervisoryUART_DisableRxInt();

        locRxBufferRead  = SupervisoryUART_rxBufferRead;
        locRxBufferWrite = SupervisoryUART_rxBufferWrite;

        if( (SupervisoryUART_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = SupervisoryUART_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= SupervisoryUART_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            SupervisoryUART_rxBufferRead = locRxBufferRead;

            if(SupervisoryUART_rxBufferLoopDetect != 0u)
            {
                SupervisoryUART_rxBufferLoopDetect = 0u;
                #if ((SupervisoryUART_RX_INTERRUPT_ENABLED) && (SupervisoryUART_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( SupervisoryUART_HD_ENABLED )
                        if((SupervisoryUART_CONTROL_REG & SupervisoryUART_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            SupervisoryUART_RXSTATUS_MASK_REG  |= SupervisoryUART_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        SupervisoryUART_RXSTATUS_MASK_REG  |= SupervisoryUART_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end SupervisoryUART_HD_ENABLED */
                #endif /* ((SupervisoryUART_RX_INTERRUPT_ENABLED) && (SupervisoryUART_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = SupervisoryUART_RXDATA_REG;
        }

        SupervisoryUART_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = SupervisoryUART_RXDATA_REG;

    #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  SupervisoryUART_RX_STS_FIFO_NOTEMPTY.
    *  SupervisoryUART_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  SupervisoryUART_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   SupervisoryUART_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   SupervisoryUART_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 SupervisoryUART_ReadRxStatus(void) 
    {
        uint8 status;

        status = SupervisoryUART_RXSTATUS_REG & SupervisoryUART_RX_HW_MASK;

    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
        if(SupervisoryUART_rxBufferOverflow != 0u)
        {
            status |= SupervisoryUART_RX_STS_SOFT_BUFF_OVER;
            SupervisoryUART_rxBufferOverflow = 0u;
        }
    #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. SupervisoryUART_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  SupervisoryUART_rxBuffer - RAM buffer pointer for save received data.
    *  SupervisoryUART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  SupervisoryUART_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  SupervisoryUART_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 SupervisoryUART_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        SupervisoryUART_DisableRxInt();

        locRxBufferRead  = SupervisoryUART_rxBufferRead;
        locRxBufferWrite = SupervisoryUART_rxBufferWrite;

        if( (SupervisoryUART_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = SupervisoryUART_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= SupervisoryUART_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            SupervisoryUART_rxBufferRead = locRxBufferRead;

            if(SupervisoryUART_rxBufferLoopDetect != 0u)
            {
                SupervisoryUART_rxBufferLoopDetect = 0u;
                #if( (SupervisoryUART_RX_INTERRUPT_ENABLED) && (SupervisoryUART_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( SupervisoryUART_HD_ENABLED )
                        if((SupervisoryUART_CONTROL_REG & SupervisoryUART_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            SupervisoryUART_RXSTATUS_MASK_REG |= SupervisoryUART_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        SupervisoryUART_RXSTATUS_MASK_REG |= SupervisoryUART_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end SupervisoryUART_HD_ENABLED */
                #endif /* SupervisoryUART_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = SupervisoryUART_RXSTATUS_REG;
            if((rxStatus & SupervisoryUART_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = SupervisoryUART_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (SupervisoryUART_RX_STS_BREAK | SupervisoryUART_RX_STS_PAR_ERROR |
                                SupervisoryUART_RX_STS_STOP_ERROR | SupervisoryUART_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        SupervisoryUART_EnableRxInt();

    #else

        rxStatus =SupervisoryUART_RXSTATUS_REG;
        if((rxStatus & SupervisoryUART_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = SupervisoryUART_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (SupervisoryUART_RX_STS_BREAK | SupervisoryUART_RX_STS_PAR_ERROR |
                            SupervisoryUART_RX_STS_STOP_ERROR | SupervisoryUART_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 SupervisoryUART_GetByte(void) 
    {
        
    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        SupervisoryUART_DisableRxInt();
        locErrorStatus = (uint16)SupervisoryUART_errorStatus;
        SupervisoryUART_errorStatus = 0u;
        SupervisoryUART_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | SupervisoryUART_ReadRxData() );
    #else
        return ( ((uint16)SupervisoryUART_ReadRxStatus() << 8u) | SupervisoryUART_ReadRxData() );
    #endif /* SupervisoryUART_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  SupervisoryUART_rxBufferWrite - used to calculate left bytes.
    *  SupervisoryUART_rxBufferRead - used to calculate left bytes.
    *  SupervisoryUART_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 SupervisoryUART_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        SupervisoryUART_DisableRxInt();

        if(SupervisoryUART_rxBufferRead == SupervisoryUART_rxBufferWrite)
        {
            if(SupervisoryUART_rxBufferLoopDetect != 0u)
            {
                size = SupervisoryUART_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(SupervisoryUART_rxBufferRead < SupervisoryUART_rxBufferWrite)
        {
            size = (SupervisoryUART_rxBufferWrite - SupervisoryUART_rxBufferRead);
        }
        else
        {
            size = (SupervisoryUART_RX_BUFFER_SIZE - SupervisoryUART_rxBufferRead) + SupervisoryUART_rxBufferWrite;
        }

        SupervisoryUART_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((SupervisoryUART_RXSTATUS_REG & SupervisoryUART_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_rxBufferWrite - cleared to zero.
    *  SupervisoryUART_rxBufferRead - cleared to zero.
    *  SupervisoryUART_rxBufferLoopDetect - cleared to zero.
    *  SupervisoryUART_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void SupervisoryUART_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        SupervisoryUART_RXDATA_AUX_CTL_REG |= (uint8)  SupervisoryUART_RX_FIFO_CLR;
        SupervisoryUART_RXDATA_AUX_CTL_REG &= (uint8) ~SupervisoryUART_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        SupervisoryUART_DisableRxInt();

        SupervisoryUART_rxBufferRead = 0u;
        SupervisoryUART_rxBufferWrite = 0u;
        SupervisoryUART_rxBufferLoopDetect = 0u;
        SupervisoryUART_rxBufferOverflow = 0u;

        SupervisoryUART_EnableRxInt();

    #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  SupervisoryUART__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  SupervisoryUART__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  SupervisoryUART__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  SupervisoryUART__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  SupervisoryUART__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  SupervisoryUART_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void SupervisoryUART_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(SupervisoryUART_RXHW_ADDRESS_ENABLED)
            #if(SupervisoryUART_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* SupervisoryUART_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = SupervisoryUART_CONTROL_REG & (uint8)~SupervisoryUART_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << SupervisoryUART_CTRL_RXADDR_MODE0_SHIFT);
                SupervisoryUART_CONTROL_REG = tmpCtrl;

                #if(SupervisoryUART_RX_INTERRUPT_ENABLED && \
                   (SupervisoryUART_RXBUFFERSIZE > SupervisoryUART_FIFO_LENGTH) )
                    SupervisoryUART_rxAddressMode = addressMode;
                    SupervisoryUART_rxAddressDetected = 0u;
                #endif /* End SupervisoryUART_RXBUFFERSIZE > SupervisoryUART_FIFO_LENGTH*/
            #endif /* End SupervisoryUART_CONTROL_REG_REMOVED */
        #else /* SupervisoryUART_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End SupervisoryUART_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SupervisoryUART_SetRxAddress1(uint8 address) 
    {
        SupervisoryUART_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SupervisoryUART_SetRxAddress2(uint8 address) 
    {
        SupervisoryUART_RXADDRESS2_REG = address;
    }

#endif  /* SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED*/


#if( (SupervisoryUART_TX_ENABLED) || (SupervisoryUART_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: SupervisoryUART_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   SupervisoryUART_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   SupervisoryUART_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   SupervisoryUART_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   SupervisoryUART_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void SupervisoryUART_SetTxInterruptMode(uint8 intSrc) 
    {
        SupervisoryUART_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  SupervisoryUART_txBuffer - RAM buffer pointer for save data for transmission
    *  SupervisoryUART_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  SupervisoryUART_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  SupervisoryUART_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void SupervisoryUART_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(SupervisoryUART_initVar != 0u)
        {
        #if (SupervisoryUART_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            SupervisoryUART_DisableTxInt();

            if( (SupervisoryUART_txBufferRead == SupervisoryUART_txBufferWrite) &&
                ((SupervisoryUART_TXSTATUS_REG & SupervisoryUART_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                SupervisoryUART_TXDATA_REG = txDataByte;
            }
            else
            {
                if(SupervisoryUART_txBufferWrite >= SupervisoryUART_TX_BUFFER_SIZE)
                {
                    SupervisoryUART_txBufferWrite = 0u;
                }

                SupervisoryUART_txBuffer[SupervisoryUART_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                SupervisoryUART_txBufferWrite++;
            }

            SupervisoryUART_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            SupervisoryUART_TXDATA_REG = txDataByte;

        #endif /*(SupervisoryUART_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 SupervisoryUART_ReadTxStatus(void) 
    {
        return(SupervisoryUART_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_txBuffer - RAM buffer pointer for save data for transmission
    *  SupervisoryUART_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  SupervisoryUART_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  SupervisoryUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void SupervisoryUART_PutChar(uint8 txDataByte) 
    {
    #if (SupervisoryUART_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            SupervisoryUART_DisableTxInt();
        #endif /* (SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = SupervisoryUART_txBufferWrite;
            locTxBufferRead  = SupervisoryUART_txBufferRead;

        #if ((SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            SupervisoryUART_EnableTxInt();
        #endif /* (SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(SupervisoryUART_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((SupervisoryUART_TXSTATUS_REG & SupervisoryUART_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            SupervisoryUART_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= SupervisoryUART_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            SupervisoryUART_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            SupervisoryUART_DisableTxInt();
        #endif /* (SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            SupervisoryUART_txBufferWrite = locTxBufferWrite;

        #if ((SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3))
            SupervisoryUART_EnableTxInt();
        #endif /* (SupervisoryUART_TX_BUFFER_SIZE > SupervisoryUART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (SupervisoryUART_TXSTATUS_REG & SupervisoryUART_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                SupervisoryUART_SetPendingTxInt();
            }
        }

    #else

        while((SupervisoryUART_TXSTATUS_REG & SupervisoryUART_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        SupervisoryUART_TXDATA_REG = txDataByte;

    #endif /* SupervisoryUART_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void SupervisoryUART_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(SupervisoryUART_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                SupervisoryUART_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void SupervisoryUART_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(SupervisoryUART_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                SupervisoryUART_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void SupervisoryUART_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(SupervisoryUART_initVar != 0u)
        {
            SupervisoryUART_PutChar(txDataByte);
            SupervisoryUART_PutChar(0x0Du);
            SupervisoryUART_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  SupervisoryUART_txBufferWrite - used to calculate left space.
    *  SupervisoryUART_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 SupervisoryUART_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (SupervisoryUART_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        SupervisoryUART_DisableTxInt();

        if(SupervisoryUART_txBufferRead == SupervisoryUART_txBufferWrite)
        {
            size = 0u;
        }
        else if(SupervisoryUART_txBufferRead < SupervisoryUART_txBufferWrite)
        {
            size = (SupervisoryUART_txBufferWrite - SupervisoryUART_txBufferRead);
        }
        else
        {
            size = (SupervisoryUART_TX_BUFFER_SIZE - SupervisoryUART_txBufferRead) +
                    SupervisoryUART_txBufferWrite;
        }

        SupervisoryUART_EnableTxInt();

    #else

        size = SupervisoryUART_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & SupervisoryUART_TX_STS_FIFO_FULL) != 0u)
        {
            size = SupervisoryUART_FIFO_LENGTH;
        }
        else if((size & SupervisoryUART_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_txBufferWrite - cleared to zero.
    *  SupervisoryUART_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void SupervisoryUART_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        SupervisoryUART_TXDATA_AUX_CTL_REG |= (uint8)  SupervisoryUART_TX_FIFO_CLR;
        SupervisoryUART_TXDATA_AUX_CTL_REG &= (uint8) ~SupervisoryUART_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (SupervisoryUART_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        SupervisoryUART_DisableTxInt();

        SupervisoryUART_txBufferRead = 0u;
        SupervisoryUART_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        SupervisoryUART_EnableTxInt();

    #endif /* (SupervisoryUART_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   SupervisoryUART_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   SupervisoryUART_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   SupervisoryUART_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   SupervisoryUART_SEND_WAIT_REINIT - Performs both options: 
    *      SupervisoryUART_SEND_BREAK and SupervisoryUART_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  SupervisoryUART_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with SupervisoryUART_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The SupervisoryUART_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void SupervisoryUART_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(SupervisoryUART_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(SupervisoryUART_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == SupervisoryUART_SEND_BREAK) ||
                (retMode == SupervisoryUART_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() |
                                                      SupervisoryUART_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                SupervisoryUART_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = SupervisoryUART_TXSTATUS_REG;
                }
                while((tmpStat & SupervisoryUART_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == SupervisoryUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == SupervisoryUART_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = SupervisoryUART_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & SupervisoryUART_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == SupervisoryUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == SupervisoryUART_REINIT) ||
                (retMode == SupervisoryUART_SEND_WAIT_REINIT) )
            {
                SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() &
                                              (uint8)~SupervisoryUART_CTRL_HD_SEND_BREAK);
            }

        #else /* SupervisoryUART_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == SupervisoryUART_SEND_BREAK) ||
                (retMode == SupervisoryUART_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (SupervisoryUART_PARITY_TYPE != SupervisoryUART__B_UART__NONE_REVB) || \
                                    (SupervisoryUART_PARITY_TYPE_SW != 0u) )
                    SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() |
                                                          SupervisoryUART_CTRL_HD_SEND_BREAK);
                #endif /* End SupervisoryUART_PARITY_TYPE != SupervisoryUART__B_UART__NONE_REVB  */

                #if(SupervisoryUART_TXCLKGEN_DP)
                    txPeriod = SupervisoryUART_TXBITCLKTX_COMPLETE_REG;
                    SupervisoryUART_TXBITCLKTX_COMPLETE_REG = SupervisoryUART_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = SupervisoryUART_TXBITCTR_PERIOD_REG;
                    SupervisoryUART_TXBITCTR_PERIOD_REG = SupervisoryUART_TXBITCTR_BREAKBITS8X;
                #endif /* End SupervisoryUART_TXCLKGEN_DP */

                /* Send zeros */
                SupervisoryUART_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = SupervisoryUART_TXSTATUS_REG;
                }
                while((tmpStat & SupervisoryUART_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == SupervisoryUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == SupervisoryUART_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = SupervisoryUART_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & SupervisoryUART_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == SupervisoryUART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == SupervisoryUART_REINIT) ||
                (retMode == SupervisoryUART_SEND_WAIT_REINIT) )
            {

            #if(SupervisoryUART_TXCLKGEN_DP)
                SupervisoryUART_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                SupervisoryUART_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End SupervisoryUART_TXCLKGEN_DP */

            #if( (SupervisoryUART_PARITY_TYPE != SupervisoryUART__B_UART__NONE_REVB) || \
                 (SupervisoryUART_PARITY_TYPE_SW != 0u) )
                SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() &
                                                      (uint8) ~SupervisoryUART_CTRL_HD_SEND_BREAK);
            #endif /* End SupervisoryUART_PARITY_TYPE != NONE */
            }
        #endif    /* End SupervisoryUART_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       SupervisoryUART_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       SupervisoryUART_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears SupervisoryUART_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void SupervisoryUART_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( SupervisoryUART_CONTROL_REG_REMOVED == 0u )
            SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() |
                                                  SupervisoryUART_CTRL_MARK);
        #endif /* End SupervisoryUART_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( SupervisoryUART_CONTROL_REG_REMOVED == 0u )
            SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() &
                                                  (uint8) ~SupervisoryUART_CTRL_MARK);
        #endif /* End SupervisoryUART_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndSupervisoryUART_TX_ENABLED */

#if(SupervisoryUART_HD_ENABLED)


    /*******************************************************************************
    * Function Name: SupervisoryUART_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void SupervisoryUART_LoadRxConfig(void) 
    {
        SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() &
                                                (uint8)~SupervisoryUART_CTRL_HD_SEND);
        SupervisoryUART_RXBITCTR_PERIOD_REG = SupervisoryUART_HD_RXBITCTR_INIT;

    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        SupervisoryUART_SetRxInterruptMode(SupervisoryUART_INIT_RX_INTERRUPTS_MASK);
    #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: SupervisoryUART_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void SupervisoryUART_LoadTxConfig(void) 
    {
    #if (SupervisoryUART_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        SupervisoryUART_SetRxInterruptMode(0u);
    #endif /* (SupervisoryUART_RX_INTERRUPT_ENABLED) */

        SupervisoryUART_WriteControlRegister(SupervisoryUART_ReadControlRegister() | SupervisoryUART_CTRL_HD_SEND);
        SupervisoryUART_RXBITCTR_PERIOD_REG = SupervisoryUART_HD_TXBITCTR_INIT;
    }

#endif  /* SupervisoryUART_HD_ENABLED */


/* [] END OF FILE */
