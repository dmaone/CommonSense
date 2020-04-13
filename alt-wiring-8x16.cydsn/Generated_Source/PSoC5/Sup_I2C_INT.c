/*******************************************************************************
* File Name: Sup_I2C_INT.c
* Version 3.50
*
* Description:
*  This file provides the source code of Interrupt Service Routine (ISR)
*  for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Sup_I2C_PVT.h"
#include "cyapicallbacks.h"


/*******************************************************************************
*  Place your includes, defines and code here.
********************************************************************************/
/* `#START Sup_I2C_ISR_intc` */

/* `#END` */


/*******************************************************************************
* Function Name: Sup_I2C_ISR
********************************************************************************
*
* Summary:
*  The handler for the I2C interrupt. The slave and master operations are
*  handled here.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
CY_ISR(Sup_I2C_ISR)
{
#if (Sup_I2C_MODE_SLAVE_ENABLED)
   uint8  tmp8;
#endif  /* (Sup_I2C_MODE_SLAVE_ENABLED) */

    uint8  tmpCsr;
    
#ifdef Sup_I2C_ISR_ENTRY_CALLBACK
    Sup_I2C_ISR_EntryCallback();
#endif /* Sup_I2C_ISR_ENTRY_CALLBACK */
    

#if(Sup_I2C_TIMEOUT_FF_ENABLED)
    if(0u != Sup_I2C_TimeoutGetStatus())
    {
        Sup_I2C_TimeoutReset();
        Sup_I2C_state = Sup_I2C_SM_EXIT_IDLE;
        /* Sup_I2C_CSR_REG should be cleared after reset */
    }
#endif /* (Sup_I2C_TIMEOUT_FF_ENABLED) */


    tmpCsr = Sup_I2C_CSR_REG;      /* Make copy as interrupts clear */

#if(Sup_I2C_MODE_MULTI_MASTER_SLAVE_ENABLED)
    if(Sup_I2C_CHECK_START_GEN(Sup_I2C_MCSR_REG))
    {
        Sup_I2C_CLEAR_START_GEN;

        /* Set transfer complete and error flags */
        Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_ERR_XFER |
                                        Sup_I2C_GET_MSTAT_CMPLT);

        /* Slave was addressed */
        Sup_I2C_state = Sup_I2C_SM_SLAVE;
    }
#endif /* (Sup_I2C_MODE_MULTI_MASTER_SLAVE_ENABLED) */


#if(Sup_I2C_MODE_MULTI_MASTER_ENABLED)
    if(Sup_I2C_CHECK_LOST_ARB(tmpCsr))
    {
        /* Set errors */
        Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_ERR_XFER     |
                                        Sup_I2C_MSTAT_ERR_ARB_LOST |
                                        Sup_I2C_GET_MSTAT_CMPLT);

        Sup_I2C_DISABLE_INT_ON_STOP; /* Interrupt on Stop is enabled by write */

        #if(Sup_I2C_MODE_MULTI_MASTER_SLAVE_ENABLED)
            if(Sup_I2C_CHECK_ADDRESS_STS(tmpCsr))
            {
                /* Slave was addressed */
                Sup_I2C_state = Sup_I2C_SM_SLAVE;
            }
            else
            {
                Sup_I2C_BUS_RELEASE;

                Sup_I2C_state = Sup_I2C_SM_EXIT_IDLE;
            }
        #else
            Sup_I2C_BUS_RELEASE;

            Sup_I2C_state = Sup_I2C_SM_EXIT_IDLE;

        #endif /* (Sup_I2C_MODE_MULTI_MASTER_SLAVE_ENABLED) */
    }
#endif /* (Sup_I2C_MODE_MULTI_MASTER_ENABLED) */

    /* Check for master operation mode */
    if(Sup_I2C_CHECK_SM_MASTER)
    {
    #if(Sup_I2C_MODE_MASTER_ENABLED)
        if(Sup_I2C_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            switch (Sup_I2C_state)
            {
            case Sup_I2C_SM_MSTR_WR_ADDR:  /* After address is sent, write data */
            case Sup_I2C_SM_MSTR_RD_ADDR:  /* After address is sent, read data */

                tmpCsr &= ((uint8) ~Sup_I2C_CSR_STOP_STATUS); /* Clear Stop bit history on address phase */

                if(Sup_I2C_CHECK_ADDR_ACK(tmpCsr))
                {
                    /* Setup for transmit or receive of data */
                    if(Sup_I2C_state == Sup_I2C_SM_MSTR_WR_ADDR)   /* TRANSMIT data */
                    {
                        /* Check if at least one byte to transfer */
                        if(Sup_I2C_mstrWrBufSize > 0u)
                        {
                            /* Load the 1st data byte */
                            Sup_I2C_DATA_REG = Sup_I2C_mstrWrBufPtr[0u];
                            Sup_I2C_TRANSMIT_DATA;
                            Sup_I2C_mstrWrBufIndex = 1u;   /* Set index to 2nd element */

                            /* Set transmit state until done */
                            Sup_I2C_state = Sup_I2C_SM_MSTR_WR_DATA;
                        }
                        /* End of buffer: complete writing */
                        else if(Sup_I2C_CHECK_NO_STOP(Sup_I2C_mstrControl))
                        {
                            /* Set write complete and master halted */
                            Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_XFER_HALT |
                                                            Sup_I2C_MSTAT_WR_CMPLT);

                            Sup_I2C_state = Sup_I2C_SM_MSTR_HALT; /* Expect ReStart */
                            Sup_I2C_DisableInt();
                        }
                        else
                        {
                            Sup_I2C_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                            Sup_I2C_GENERATE_STOP;
                        }
                    }
                    else  /* Master receive data */
                    {
                        Sup_I2C_READY_TO_READ; /* Release bus to read data */

                        Sup_I2C_state  = Sup_I2C_SM_MSTR_RD_DATA;
                    }
                }
                /* Address is NACKed */
                else if(Sup_I2C_CHECK_ADDR_NAK(tmpCsr))
                {
                    /* Set Address NAK error */
                    Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_ERR_XFER |
                                                    Sup_I2C_MSTAT_ERR_ADDR_NAK);

                    if(Sup_I2C_CHECK_NO_STOP(Sup_I2C_mstrControl))
                    {
                        Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_XFER_HALT |
                                                        Sup_I2C_GET_MSTAT_CMPLT);

                        Sup_I2C_state = Sup_I2C_SM_MSTR_HALT; /* Expect RESTART */
                        Sup_I2C_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        Sup_I2C_ENABLE_INT_ON_STOP; /* Enable interrupt on Stop, to catch it */
                        Sup_I2C_GENERATE_STOP;
                    }
                }
                else
                {
                    /* Address phase is not set for some reason: error */
                    #if(Sup_I2C_TIMEOUT_ENABLED)
                        /* Exit interrupt to take chance for timeout timer to handle this case */
                        Sup_I2C_DisableInt();
                        Sup_I2C_ClearPendingInt();
                    #else
                        /* Block execution flow: unexpected condition */
                        CYASSERT(0u != 0u);
                    #endif /* (Sup_I2C_TIMEOUT_ENABLED) */
                }
                break;

            case Sup_I2C_SM_MSTR_WR_DATA:

                if(Sup_I2C_CHECK_DATA_ACK(tmpCsr))
                {
                    /* Check if end of buffer */
                    if(Sup_I2C_mstrWrBufIndex  < Sup_I2C_mstrWrBufSize)
                    {
                        Sup_I2C_DATA_REG =
                                                 Sup_I2C_mstrWrBufPtr[Sup_I2C_mstrWrBufIndex];
                        Sup_I2C_TRANSMIT_DATA;
                        Sup_I2C_mstrWrBufIndex++;
                    }
                    /* End of buffer: complete writing */
                    else if(Sup_I2C_CHECK_NO_STOP(Sup_I2C_mstrControl))
                    {
                        /* Set write complete and master halted */
                        Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_XFER_HALT |
                                                        Sup_I2C_MSTAT_WR_CMPLT);

                        Sup_I2C_state = Sup_I2C_SM_MSTR_HALT;    /* Expect restart */
                        Sup_I2C_DisableInt();
                    }
                    else  /* Do normal Stop */
                    {
                        Sup_I2C_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                        Sup_I2C_GENERATE_STOP;
                    }
                }
                /* Last byte NAKed: end writing */
                else if(Sup_I2C_CHECK_NO_STOP(Sup_I2C_mstrControl))
                {
                    /* Set write complete, short transfer and master halted */
                    Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_ERR_XFER       |
                                                    Sup_I2C_MSTAT_ERR_SHORT_XFER |
                                                    Sup_I2C_MSTAT_XFER_HALT      |
                                                    Sup_I2C_MSTAT_WR_CMPLT);

                    Sup_I2C_state = Sup_I2C_SM_MSTR_HALT;    /* Expect ReStart */
                    Sup_I2C_DisableInt();
                }
                else  /* Do normal Stop */
                {
                    Sup_I2C_ENABLE_INT_ON_STOP;    /* Enable interrupt on Stop, to catch it */
                    Sup_I2C_GENERATE_STOP;

                    /* Set short transfer and error flag */
                    Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_ERR_SHORT_XFER |
                                                    Sup_I2C_MSTAT_ERR_XFER);
                }

                break;

            case Sup_I2C_SM_MSTR_RD_DATA:

                Sup_I2C_mstrRdBufPtr[Sup_I2C_mstrRdBufIndex] = Sup_I2C_DATA_REG;
                Sup_I2C_mstrRdBufIndex++;

                /* Check if end of buffer */
                if(Sup_I2C_mstrRdBufIndex < Sup_I2C_mstrRdBufSize)
                {
                    Sup_I2C_ACK_AND_RECEIVE;       /* ACK and receive byte */
                }
                /* End of buffer: complete reading */
                else if(Sup_I2C_CHECK_NO_STOP(Sup_I2C_mstrControl))
                {
                    /* Set read complete and master halted */
                    Sup_I2C_mstrStatus |= (Sup_I2C_MSTAT_XFER_HALT |
                                                    Sup_I2C_MSTAT_RD_CMPLT);

                    Sup_I2C_state = Sup_I2C_SM_MSTR_HALT;    /* Expect ReStart */
                    Sup_I2C_DisableInt();
                }
                else
                {
                    Sup_I2C_ENABLE_INT_ON_STOP;
                    Sup_I2C_NAK_AND_RECEIVE;       /* NACK and TRY to generate Stop */
                }
                break;

            default: /* This is an invalid state and should not occur */

            #if(Sup_I2C_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handles this case */
                Sup_I2C_DisableInt();
                Sup_I2C_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (Sup_I2C_TIMEOUT_ENABLED) */

                break;
            }
        }

        /* Catches Stop: end of transaction */
        if(Sup_I2C_CHECK_STOP_STS(tmpCsr))
        {
            Sup_I2C_mstrStatus |= Sup_I2C_GET_MSTAT_CMPLT;

            Sup_I2C_DISABLE_INT_ON_STOP;
            Sup_I2C_state = Sup_I2C_SM_IDLE;
        }
    #endif /* (Sup_I2C_MODE_MASTER_ENABLED) */
    }
    else if(Sup_I2C_CHECK_SM_SLAVE)
    {
    #if(Sup_I2C_MODE_SLAVE_ENABLED)

        if((Sup_I2C_CHECK_STOP_STS(tmpCsr)) || /* Stop || Restart */
           (Sup_I2C_CHECK_BYTE_COMPLETE(tmpCsr) && Sup_I2C_CHECK_ADDRESS_STS(tmpCsr)))
        {
            /* Catch end of master write transaction: use interrupt on Stop */
            /* The Stop bit history on address phase does not have correct state */
            if(Sup_I2C_SM_SL_WR_DATA == Sup_I2C_state)
            {
                Sup_I2C_DISABLE_INT_ON_STOP;

                Sup_I2C_slStatus &= ((uint8) ~Sup_I2C_SSTAT_WR_BUSY);
                Sup_I2C_slStatus |= ((uint8)  Sup_I2C_SSTAT_WR_CMPLT);

                Sup_I2C_state = Sup_I2C_SM_IDLE;
            }
        }

        if(Sup_I2C_CHECK_BYTE_COMPLETE(tmpCsr))
        {
            /* The address only issued after Start or ReStart: so check the address
               to catch these events:
                FF : sets an address phase with a byte_complete interrupt trigger.
                UDB: sets an address phase immediately after Start or ReStart. */
            if(Sup_I2C_CHECK_ADDRESS_STS(tmpCsr))
            {
            /* Check for software address detection */
            #if(Sup_I2C_SW_ADRR_DECODE)
                tmp8 = Sup_I2C_GET_SLAVE_ADDR(Sup_I2C_DATA_REG);

                if(tmp8 == Sup_I2C_slAddress)   /* Check for address match */
                {
                    if(0u != (Sup_I2C_DATA_REG & Sup_I2C_READ_FLAG))
                    {
                        /* Place code to prepare read buffer here                  */
                        /* `#START Sup_I2C_SW_PREPARE_READ_BUF_interrupt` */

                        /* `#END` */

                    #ifdef Sup_I2C_SW_PREPARE_READ_BUF_CALLBACK
                        Sup_I2C_SwPrepareReadBuf_Callback();
                    #endif /* Sup_I2C_SW_PREPARE_READ_BUF_CALLBACK */
                        
                        /* Prepare next operation to read, get data and place in data register */
                        if(Sup_I2C_slRdBufIndex < Sup_I2C_slRdBufSize)
                        {
                            /* Load first data byte from array */
                            Sup_I2C_DATA_REG = Sup_I2C_slRdBufPtr[Sup_I2C_slRdBufIndex];
                            Sup_I2C_ACK_AND_TRANSMIT;
                            Sup_I2C_slRdBufIndex++;

                            Sup_I2C_slStatus |= Sup_I2C_SSTAT_RD_BUSY;
                        }
                        else    /* Overflow: provide 0xFF on bus */
                        {
                            Sup_I2C_DATA_REG = Sup_I2C_OVERFLOW_RETURN;
                            Sup_I2C_ACK_AND_TRANSMIT;

                            Sup_I2C_slStatus  |= (Sup_I2C_SSTAT_RD_BUSY |
                                                           Sup_I2C_SSTAT_RD_ERR_OVFL);
                        }

                        Sup_I2C_state = Sup_I2C_SM_SL_RD_DATA;
                    }
                    else  /* Write transaction: receive 1st byte */
                    {
                        Sup_I2C_ACK_AND_RECEIVE;
                        Sup_I2C_state = Sup_I2C_SM_SL_WR_DATA;

                        Sup_I2C_slStatus |= Sup_I2C_SSTAT_WR_BUSY;
                        Sup_I2C_ENABLE_INT_ON_STOP;
                    }
                }
                else
                {
                    /*     Place code to compare for additional address here    */
                    /* `#START Sup_I2C_SW_ADDR_COMPARE_interruptStart` */

                    /* `#END` */

                #ifdef Sup_I2C_SW_ADDR_COMPARE_ENTRY_CALLBACK
                    Sup_I2C_SwAddrCompare_EntryCallback();
                #endif /* Sup_I2C_SW_ADDR_COMPARE_ENTRY_CALLBACK */
                    
                    Sup_I2C_NAK_AND_RECEIVE;   /* NACK address */

                    /* Place code to end of condition for NACK generation here */
                    /* `#START Sup_I2C_SW_ADDR_COMPARE_interruptEnd`  */

                    /* `#END` */

                #ifdef Sup_I2C_SW_ADDR_COMPARE_EXIT_CALLBACK
                    Sup_I2C_SwAddrCompare_ExitCallback();
                #endif /* Sup_I2C_SW_ADDR_COMPARE_EXIT_CALLBACK */
                }

            #else /* (Sup_I2C_HW_ADRR_DECODE) */

                if(0u != (Sup_I2C_DATA_REG & Sup_I2C_READ_FLAG))
                {
                    /* Place code to prepare read buffer here                  */
                    /* `#START Sup_I2C_HW_PREPARE_READ_BUF_interrupt` */

                    /* `#END` */
                    
                #ifdef Sup_I2C_HW_PREPARE_READ_BUF_CALLBACK
                    Sup_I2C_HwPrepareReadBuf_Callback();
                #endif /* Sup_I2C_HW_PREPARE_READ_BUF_CALLBACK */

                    /* Prepare next operation to read, get data and place in data register */
                    if(Sup_I2C_slRdBufIndex < Sup_I2C_slRdBufSize)
                    {
                        /* Load first data byte from array */
                        Sup_I2C_DATA_REG = Sup_I2C_slRdBufPtr[Sup_I2C_slRdBufIndex];
                        Sup_I2C_ACK_AND_TRANSMIT;
                        Sup_I2C_slRdBufIndex++;

                        Sup_I2C_slStatus |= Sup_I2C_SSTAT_RD_BUSY;
                    }
                    else    /* Overflow: provide 0xFF on bus */
                    {
                        Sup_I2C_DATA_REG = Sup_I2C_OVERFLOW_RETURN;
                        Sup_I2C_ACK_AND_TRANSMIT;

                        Sup_I2C_slStatus  |= (Sup_I2C_SSTAT_RD_BUSY |
                                                       Sup_I2C_SSTAT_RD_ERR_OVFL);
                    }

                    Sup_I2C_state = Sup_I2C_SM_SL_RD_DATA;
                }
                else  /* Write transaction: receive 1st byte */
                {
                    Sup_I2C_ACK_AND_RECEIVE;
                    Sup_I2C_state = Sup_I2C_SM_SL_WR_DATA;

                    Sup_I2C_slStatus |= Sup_I2C_SSTAT_WR_BUSY;
                    Sup_I2C_ENABLE_INT_ON_STOP;
                }

            #endif /* (Sup_I2C_SW_ADRR_DECODE) */
            }
            /* Data states */
            /* Data master writes into slave */
            else if(Sup_I2C_state == Sup_I2C_SM_SL_WR_DATA)
            {
                if(Sup_I2C_slWrBufIndex < Sup_I2C_slWrBufSize)
                {
                    tmp8 = Sup_I2C_DATA_REG;
                    Sup_I2C_ACK_AND_RECEIVE;
                    Sup_I2C_slWrBufPtr[Sup_I2C_slWrBufIndex] = tmp8;
                    Sup_I2C_slWrBufIndex++;
                }
                else  /* of array: complete write, send NACK */
                {
                    Sup_I2C_NAK_AND_RECEIVE;

                    Sup_I2C_slStatus |= Sup_I2C_SSTAT_WR_ERR_OVFL;
                }
            }
            /* Data master reads from slave */
            else if(Sup_I2C_state == Sup_I2C_SM_SL_RD_DATA)
            {
                if(Sup_I2C_CHECK_DATA_ACK(tmpCsr))
                {
                    if(Sup_I2C_slRdBufIndex < Sup_I2C_slRdBufSize)
                    {
                         /* Get data from array */
                        Sup_I2C_DATA_REG = Sup_I2C_slRdBufPtr[Sup_I2C_slRdBufIndex];
                        Sup_I2C_TRANSMIT_DATA;
                        Sup_I2C_slRdBufIndex++;
                    }
                    else   /* Overflow: provide 0xFF on bus */
                    {
                        Sup_I2C_DATA_REG = Sup_I2C_OVERFLOW_RETURN;
                        Sup_I2C_TRANSMIT_DATA;

                        Sup_I2C_slStatus |= Sup_I2C_SSTAT_RD_ERR_OVFL;
                    }
                }
                else  /* Last byte was NACKed: read complete */
                {
                    /* Only NACK appears on bus */
                    Sup_I2C_DATA_REG = Sup_I2C_OVERFLOW_RETURN;
                    Sup_I2C_NAK_AND_TRANSMIT;

                    Sup_I2C_slStatus &= ((uint8) ~Sup_I2C_SSTAT_RD_BUSY);
                    Sup_I2C_slStatus |= ((uint8)  Sup_I2C_SSTAT_RD_CMPLT);

                    Sup_I2C_state = Sup_I2C_SM_IDLE;
                }
            }
            else
            {
            #if(Sup_I2C_TIMEOUT_ENABLED)
                /* Exit interrupt to take chance for timeout timer to handle this case */
                Sup_I2C_DisableInt();
                Sup_I2C_ClearPendingInt();
            #else
                /* Block execution flow: unexpected condition */
                CYASSERT(0u != 0u);
            #endif /* (Sup_I2C_TIMEOUT_ENABLED) */
            }
        }
    #endif /* (Sup_I2C_MODE_SLAVE_ENABLED) */
    }
    else
    {
        /* The FSM skips master and slave processing: return to IDLE */
        Sup_I2C_state = Sup_I2C_SM_IDLE;
    }

#ifdef Sup_I2C_ISR_EXIT_CALLBACK
    Sup_I2C_ISR_ExitCallback();
#endif /* Sup_I2C_ISR_EXIT_CALLBACK */    
}


#if ((Sup_I2C_FF_IMPLEMENTED) && (Sup_I2C_WAKEUP_ENABLED))
    /*******************************************************************************
    * Function Name: Sup_I2C_WAKEUP_ISR
    ********************************************************************************
    *
    * Summary:
    *  The interrupt handler to trigger after a wakeup.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    CY_ISR(Sup_I2C_WAKEUP_ISR)
    {
    #ifdef Sup_I2C_WAKEUP_ISR_ENTRY_CALLBACK
        Sup_I2C_WAKEUP_ISR_EntryCallback();
    #endif /* Sup_I2C_WAKEUP_ISR_ENTRY_CALLBACK */
         
        /* Set flag to notify that matched address is received */
        Sup_I2C_wakeupSource = 1u;

        /* SCL is stretched until the I2C_Wake() is called */

    #ifdef Sup_I2C_WAKEUP_ISR_EXIT_CALLBACK
        Sup_I2C_WAKEUP_ISR_ExitCallback();
    #endif /* Sup_I2C_WAKEUP_ISR_EXIT_CALLBACK */
    }
#endif /* ((Sup_I2C_FF_IMPLEMENTED) && (Sup_I2C_WAKEUP_ENABLED)) */


/* [] END OF FILE */
