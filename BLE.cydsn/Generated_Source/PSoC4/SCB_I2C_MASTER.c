/***************************************************************************//**
* \file SCB_I2C_MASTER.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  I2C Master mode.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SCB_PVT.h"
#include "SCB_I2C_PVT.h"

#if(SCB_I2C_MASTER_CONST)

/***************************************
*      I2C Master Private Vars
***************************************/

/* Master variables */
volatile uint16 SCB_mstrStatus;      /* Master Status byte  */
volatile uint8  SCB_mstrControl;     /* Master Control byte */

/* Receive buffer variables */
volatile uint8 * SCB_mstrRdBufPtr;   /* Pointer to Master Read buffer */
volatile uint32  SCB_mstrRdBufSize;  /* Master Read buffer size       */
volatile uint32  SCB_mstrRdBufIndex; /* Master Read buffer Index      */

/* Transmit buffer variables */
volatile uint8 * SCB_mstrWrBufPtr;   /* Pointer to Master Write buffer */
volatile uint32  SCB_mstrWrBufSize;  /* Master Write buffer size       */
volatile uint32  SCB_mstrWrBufIndex; /* Master Write buffer Index      */
volatile uint32  SCB_mstrWrBufIndexTmp; /* Master Write buffer Index Tmp */

#if (!SCB_CY_SCBIP_V0 && \
    SCB_I2C_MULTI_MASTER_SLAVE_CONST && SCB_I2C_WAKE_ENABLE_CONST)
    static void SCB_I2CMasterDisableEcAm(void);
#else
    #define SCB_I2CMasterDisableEcAm()     do{ /* Empty */ }while(0)
#endif /* (!SCB_CY_SCBIP_V0) */

static uint32 SCB_I2CMasterHandleStatus(uint32 status);
static uint32 SCB_I2CMasterWaitOneUnit(uint32 *timeout);


/*******************************************************************************
* Function Name: SCB_I2CMasterWriteBuf
****************************************************************************//**
*
*  Automatically writes an entire buffer of data to a slave device.
*  Once the data transfer is initiated by this function, further data transfer
*  is handled by the included ISR.
*  Enables the I2C interrupt and clears SCB_I2C_MSTAT_WR_CMPLT
*  status.
*
*  \param slaveAddr: 7-bit slave address.
*  \param xferData: Pointer to buffer of data to be sent.
*  \param cnt: Size of buffer to send.
*  \param mode: Transfer mode defines:
*  (1) Whether a start or restart condition is generated at the beginning
*  of the transfer, and
*  (2) Whether the transfer is completed or halted before the stop
*  condition is generated on the bus.Transfer mode, mode constants
*  may be ORed together.
*  - SCB_I2C_MODE_COMPLETE_XFER - Perform complete transfer
*    from Start to Stop.
*  - SCB_I2C_MODE_REPEAT_START - Send Repeat Start instead
*    of Start. A Stop is generated after transfer is completed unless
*    NO_STOP is specified.
*  - SCB_I2C_MODE_NO_STOP Execute transfer without a Stop.
*    The following transfer expected to perform ReStart.
*
* \return
*  Error status.
*  - SCB_I2C_MSTR_NO_ERROR - Function complete without error.
*    The master started the transfer.
*  - SCB_I2C_MSTR_BUS_BUSY - Bus is busy. Nothing was sent on
*    the bus. The attempt has to be retried.
*  - SCB_I2C_MSTR_NOT_READY - Master is not ready for to start
*    transfer. A master still has not completed previous transaction or a
*    slave operation is in progress (in multi-master-slave configuration).
*    Nothing was sent on the bus. The attempt has to be retried.
*
* \globalvars
*  SCB_mstrStatus  - used to store current status of I2C Master.
*  SCB_state       - used to store current state of software FSM.
*  SCB_mstrControl - used to control master end of transaction with
*  or without the Stop generation.
*  SCB_mstrWrBufPtr - used to store pointer to master write buffer.
*  SCB_mstrWrBufIndex - used to current index within master write
*  buffer.
*  SCB_mstrWrBufSize - used to store master write buffer size.
*
*******************************************************************************/
uint32 SCB_I2CMasterWriteBuf(uint32 slaveAddress, uint8 * wrData, uint32 cnt, uint32 mode)
{
    uint32 errStatus;

    errStatus = SCB_I2C_MSTR_NOT_READY;

    if(NULL != wrData)  /* Check buffer pointer */
    {
        /* Check FSM state and bus before generating Start/ReStart condition */
        if(SCB_CHECK_I2C_FSM_IDLE)
        {
            SCB_DisableInt();  /* Lock from interruption */

            /* Check bus state */
            errStatus = SCB_CHECK_I2C_STATUS(SCB_I2C_STATUS_BUS_BUSY) ?
                            SCB_I2C_MSTR_BUS_BUSY : SCB_I2C_MSTR_NO_ERROR;
        }
        else if(SCB_CHECK_I2C_FSM_HALT)
        {
            SCB_mstrStatus &= (uint16) ~SCB_I2C_MSTAT_XFER_HALT;
                              errStatus  = SCB_I2C_MSTR_NO_ERROR;
        }
        else
        {
            /* Unexpected FSM state: exit */
        }
    }

    /* Check if master is ready to start  */
    if(SCB_I2C_MSTR_NO_ERROR == errStatus) /* No error proceed */
    {
        /* Non-empty for master-slave mode when wakeup enabled */
        SCB_I2CMasterDisableEcAm();

        /* Set up write transaction */
        SCB_state = SCB_I2C_FSM_MSTR_WR_ADDR;
        SCB_mstrWrBufIndexTmp = 0u;
        SCB_mstrWrBufIndex    = 0u;
        SCB_mstrWrBufSize     = cnt;
        SCB_mstrWrBufPtr      = (volatile uint8 *) wrData;
        SCB_mstrControl       = (uint8) mode;

        slaveAddress = SCB_GET_I2C_8BIT_ADDRESS(slaveAddress);

        SCB_mstrStatus &= (uint16) ~SCB_I2C_MSTAT_WR_CMPLT;

        SCB_ClearTxInterruptSource    (SCB_INTR_TX_UNDERFLOW);
        SCB_ClearMasterInterruptSource(SCB_INTR_MASTER_ALL);
        SCB_SetMasterInterruptMode    (SCB_I2C_INTR_MASTER_MASK);

        /* The TX and RX FIFO have to be EMPTY */

        /* Enable interrupt source to catch when address is sent */
        SCB_SetTxInterruptMode(SCB_INTR_TX_UNDERFLOW);

        /* Generate Start or ReStart */
        if(SCB_CHECK_I2C_MODE_RESTART(mode))
        {
            SCB_I2C_MASTER_GENERATE_RESTART;
            SCB_TX_FIFO_WR_REG = slaveAddress;
        }
        else
        {
            SCB_TX_FIFO_WR_REG = slaveAddress;
            SCB_I2C_MASTER_GENERATE_START;
        }
    }

    SCB_EnableInt();   /* Release lock */

    return(errStatus);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterReadBuf
****************************************************************************//**
*
*  Automatically reads an entire buffer of data from a slave device.
*  Once the data transfer is initiated by this function, further data transfer
*  is handled by the included ISR.
*  Enables the I2C interrupt and clears SCB_I2C_MSTAT_RD_CMPLT
*  status.
*
*  \param slaveAddr: 7-bit slave address.
*  \param xferData: Pointer to buffer of data to be sent.
*  \param cnt: Size of buffer to send.
*  \param mode: Transfer mode defines:
*  (1) Whether a start or restart condition is generated at the beginning
*  of the transfer, and
*  (2) Whether the transfer is completed or halted before the stop
*  condition is generated on the bus.Transfer mode, mode constants may
*  be ORed together. See SCB_I2CMasterWriteBuf()
*  function for constants.
*
* \return
*  Error status.See SCB_I2CMasterWriteBuf()
*  function for constants.
*
* \globalvars
*  SCB_mstrStatus  - used to store current status of I2C Master.
*  SCB_state       - used to store current state of software FSM.
*  SCB_mstrControl - used to control master end of transaction with
*  or without the Stop generation.
*  SCB_mstrRdBufPtr - used to store pointer to master read buffer.
*  SCB_mstrRdBufIndex - used to current index within master read
*  buffer.
*  SCB_mstrRdBufSize - used to store master read buffer size.
*
*******************************************************************************/
uint32 SCB_I2CMasterReadBuf(uint32 slaveAddress, uint8 * rdData, uint32 cnt, uint32 mode)
{
    uint32 errStatus;

    errStatus = SCB_I2C_MSTR_NOT_READY;

    if(NULL != rdData)
    {
        /* Check FSM state and bus before generating Start/ReStart condition */
        if(SCB_CHECK_I2C_FSM_IDLE)
        {
            SCB_DisableInt();  /* Lock from interruption */

            /* Check bus state */
            errStatus = SCB_CHECK_I2C_STATUS(SCB_I2C_STATUS_BUS_BUSY) ?
                            SCB_I2C_MSTR_BUS_BUSY : SCB_I2C_MSTR_NO_ERROR;
        }
        else if(SCB_CHECK_I2C_FSM_HALT)
        {
            SCB_mstrStatus &= (uint16) ~SCB_I2C_MSTAT_XFER_HALT;
                              errStatus  =  SCB_I2C_MSTR_NO_ERROR;
        }
        else
        {
            /* Unexpected FSM state: exit */
        }
    }

    /* Check master ready to proceed */
    if(SCB_I2C_MSTR_NO_ERROR == errStatus) /* No error proceed */
    {
        /* Non-empty for master-slave mode when wakeup enabled */
        SCB_I2CMasterDisableEcAm();

        /* Set up read transaction */
        SCB_state = SCB_I2C_FSM_MSTR_RD_ADDR;
        SCB_mstrRdBufIndex = 0u;
        SCB_mstrRdBufSize  = cnt;
        SCB_mstrRdBufPtr   = (volatile uint8 *) rdData;
        SCB_mstrControl    = (uint8) mode;

        slaveAddress = (SCB_GET_I2C_8BIT_ADDRESS(slaveAddress) | SCB_I2C_READ_FLAG);

        SCB_mstrStatus &= (uint16) ~SCB_I2C_MSTAT_RD_CMPLT;

        SCB_ClearMasterInterruptSource(SCB_INTR_MASTER_ALL);
        SCB_SetMasterInterruptMode    (SCB_I2C_INTR_MASTER_MASK);

        /* TX and RX FIFO have to be EMPTY */

        /* Prepare reading */
        if(SCB_mstrRdBufSize < SCB_I2C_FIFO_SIZE)
        {
            /* Reading byte-by-byte */
            SCB_SetRxInterruptMode(SCB_INTR_RX_NOT_EMPTY);
        }
        else
        {
            /* Receive RX FIFO chunks */
            SCB_ENABLE_MASTER_AUTO_DATA_ACK;
            SCB_SetRxInterruptMode(SCB_INTR_RX_FULL);
        }

        /* Generate Start or ReStart */
        if(SCB_CHECK_I2C_MODE_RESTART(mode))
        {
            SCB_I2C_MASTER_GENERATE_RESTART;
            SCB_TX_FIFO_WR_REG = slaveAddress;
        }
        else
        {
            SCB_TX_FIFO_WR_REG = slaveAddress;
            SCB_I2C_MASTER_GENERATE_START;
        }
    }

    SCB_EnableInt();   /* Release lock */

    return(errStatus);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterSendStart
****************************************************************************//**
*
* Generates Start condition and sends slave address with read/write bit.
* Disables the I2C interrupt.
* This function is blocking and does not return until start condition and
* address byte are sent and ACK/NACK response is received or errors occurred.
*
* \param slaveAddress
* Right justified 7-bit Slave address (valid range 8 to 120).
*
* \param bitRnW
* Direction of the following transfer. It is defined by read/write bit within
* address byte.
*  - SCB_I2C_WRITE_XFER_MODE - Set write direction for the
*    following transfer.
*  - SCB_I2C_READ_XFER_MODE - Set read direction for the
*    following transfer.
*
* \param timeoutMs
* Defines in milliseconds the time that this function can block for.
* If that time expires, the function returns. If a zero is passed,
* the function waits forever for the action to complete. If a timeout occurs,
* the SCB block is reset. Note The maximum value is (maximum uint32)/1000.
*
* \return
* Error status.
*  - SCB_I2C_MSTR_NO_ERROR - Function complete without error.
*  - SCB_I2C_MSTR_BUS_BUSY - Bus is busy.
*    Nothing was sent on the bus. The attempt has to be retried.
*  - SCB_I2C_MSTR_NOT_READY - Master is not ready for to
*    start transfer.
*    A master still has not completed previous transaction or a slave
*    operation is in progress (in multi-master-slave configuration).
*    Nothing was sent on the bus. The attempt has to be retried.
*  - SCB_I2C_MSTR_ERR_LB_NAK - Error condition: Last byte was
*    NAKed.
*  - SCB_I2C_MSTR_ERR_ARB_LOST - Error condition: Master lost
*    arbitration.
*  - SCB_I2C_MSTR_ERR_BUS_ERR - Error condition: Master
*    encountered a bus error. Bus error is misplaced start or stop detection.
*  - SCB_I2C_MSTR_ERR_ABORT_START - Error condition: The start
*    condition generation was aborted due to beginning of Slave operation.
*    This error condition is only applicable for Multi-Master-Slave mode.
*
* \globalvars
* SCB_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 SCB_I2CMasterSendStart(uint32 slaveAddress, uint32 bitRnW, uint32 timeoutMs)
{
    uint32 errStatus = SCB_I2C_MSTR_NOT_READY;

    if(SCB_CHECK_I2C_FSM_IDLE)
    {
        SCB_DisableInt();  /* Lock from interruption */

        if(SCB_CHECK_I2C_STATUS(SCB_I2C_STATUS_BUS_BUSY))
        {
            errStatus = SCB_I2C_MSTR_BUS_BUSY;
        }
        else
        {
            uint32 locStatus;
            uint32 timeout = SCB_I2C_CONVERT_TIMEOUT_TO_US(timeoutMs);

            /* Non-empty for master-slave mode when wakeup enabled */
            SCB_I2CMasterDisableEcAm();

            /* Clean up the hardware before a transaction. */
            SCB_SetMasterInterruptMode    (SCB_NO_INTR_SOURCES);
            SCB_ClearMasterInterruptSource(SCB_INTR_MASTER_ALL);
            SCB_ClearRxInterruptSource    (SCB_INTR_RX_ALL);

            slaveAddress = SCB_GET_I2C_8BIT_ADDRESS(slaveAddress);

            if(0u == bitRnW) /* Write direction */
            {
                SCB_state = SCB_I2C_FSM_MSTR_WR_DATA;
            }
            else             /* Read direction */
            {
                SCB_state = SCB_I2C_FSM_MSTR_RD_DATA;
                         slaveAddress |= SCB_I2C_READ_FLAG;
            }

            /* TX and RX FIFO have to be empty at here */

             /* Generate Start and send address */
            SCB_TX_FIFO_WR_REG = slaveAddress;
            SCB_I2C_MASTER_GENERATE_START;

            /* Wait until address has been transferred. Note that for master reads at least one byte. */
            do
            {
                locStatus  = SCB_GetMasterInterruptSource() & SCB_INTR_MASTER_SEND_BYTE;
                locStatus |= SCB_GetSlaveInterruptSource() & SCB_SLAVE_INTR_I2C_ADDR;
                locStatus |= SCB_I2CMasterWaitOneUnit(&timeout);
            }
            while (0u == locStatus);

            /* Handle completion event and convert to status */
            errStatus = SCB_I2CMasterHandleStatus(locStatus);
        }

        SCB_EnableInt();  /* Release lock */
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterSendRestart
****************************************************************************//**
*
* Generates Restart condition and sends slave address with read/write bit.
* This function is blocking and does not return until start condition and
* address are sent and ACK/NACK response is received or errors occurred.
*
* \param slaveAddress
* Right justified 7-bit Slave address (valid range 8 to 120).
*
* \param bitRnW
* Direction of the following transfer. It is defined by read/write bit within
* address byte.See SCB_I2CMasterSendStart() function for constants.
*
* \param timeoutMs
* Defines in milliseconds the time that this function can block for.
* If that time expires, the function returns. If a zero is passed,
* the function waits forever for the action to complete. If a timeout occurs,
* the SCB block is reset. Note The maximum value is (maximum uint32)/1000.
*
* \return
* Error status.
* See SCB_I2CMasterSendStart() function for constants.
*
* \sideeffect
* A valid Start or ReStart condition must be generated before calling
* this function. This function does nothing if Start or ReStart conditions
* failed before this function was called.
* For read transaction, at least one byte has to be read before ReStart
* generation.
*
* \globalvars
* SCB_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 SCB_I2CMasterSendRestart(uint32 slaveAddress, uint32 bitRnW, uint32 timeoutMs)
{
    uint32 errStatus = SCB_I2C_MSTR_NOT_READY;

    /* Check FSM state before generating ReStart condition */
    if(SCB_CHECK_I2C_MASTER_ACTIVE)
    {
        uint32 locStatus;
        uint32 timeout = SCB_I2C_CONVERT_TIMEOUT_TO_US(timeoutMs);

        slaveAddress = SCB_GET_I2C_8BIT_ADDRESS(slaveAddress);

        if(0u == bitRnW) /* Write direction */
        {
            SCB_state = SCB_I2C_FSM_MSTR_WR_DATA;
        }
        else             /* Read direction */
        {
            SCB_state  = SCB_I2C_FSM_MSTR_RD_DATA;
                      slaveAddress |= SCB_I2C_READ_FLAG;
        }

        /* TX and RX FIFO have to be empty at here */

        /* A proper ReStart sequence is: set command to generate ReStart, then put an address byte in the TX FIFO.
        * Put address into the TX FIFO the 1st makes scb IP think that this data byte for current write transaction.
        */
        SCB_I2C_MASTER_GENERATE_RESTART;
        SCB_TX_FIFO_WR_REG = slaveAddress;

        /* Wait until address has been transferred. Note that for master reads at least one byte. */
        do
        {
            locStatus  = SCB_GetMasterInterruptSource() & SCB_INTR_MASTER_SEND_BYTE;
            locStatus |= SCB_I2CMasterWaitOneUnit(&timeout);
        }
        while (0u == locStatus);


        /* Handle completion event and convert to status */
        errStatus = SCB_I2CMasterHandleStatus(locStatus);
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterSendStop
****************************************************************************//**
*
* Generates Stop condition on the bus.
* The NAK is generated before Stop in case of a read transaction.
* At least one byte has to be read if a Start or ReStart condition with read
* direction was generated before.
* This function is blocking and does not return until a Stop condition is
* generated or error occurred.
*
* \param timeoutMs
* Defines in milliseconds the time that this function can block for.
* If that time expires, the function returns. If a zero is passed,
* the function waits forever for the action to complete. If a timeout occurs,
* the SCB block is reset. Note The maximum value is (maximum uint32)/1000.
*
* \return
* Error status.
* See SCB_I2CMasterSendStart() function for constants.
*
* \sideeffect
* A valid Start or ReStart condition must be generated before calling
* this function. This function does nothing if Start or ReStart conditions
* failed before this function was called.
* For read transaction, at least one byte has to be read before ReStart
* generation.
*
* \globalvars
* SCB_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 SCB_I2CMasterSendStop(uint32 timeoutMs)
{
    uint32 errStatus = SCB_I2C_MSTR_NOT_READY;

    /* Check FSM state before generating Stop condition */
    if(SCB_CHECK_I2C_MASTER_ACTIVE)
    {
        uint32 locStatus;
        uint32 timeout = SCB_I2C_CONVERT_TIMEOUT_TO_US(timeoutMs);

        /* Write direction: generates Stop; Read  direction: generates NACK and Stop; */
        SCB_I2C_MASTER_GENERATE_STOP;

        /* Wait for a completion event from the master */
        do
        {
            locStatus  = SCB_GetMasterInterruptSource() & SCB_INTR_MASTER_SEND_STOP;
            locStatus |= SCB_I2CMasterWaitOneUnit(&timeout);
        }
        while (0u == locStatus);

        /* Handle completion event and convert to status */
        errStatus = SCB_I2CMasterHandleStatus(locStatus);
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterWriteByte
****************************************************************************//**
*
* Sends one byte to a slave.
* This function is blocking and does not return until byte is transmitted
* or error occurred.
*
* \param wrByte
* The data byte to send to the slave.
*
* \param timeoutMs
* Defines in milliseconds the time that this function can block for.
* If that time expires, the function returns. If a zero is passed,
* the function waits forever for the action to complete. If a timeout occurs,
* the SCB block is reset. Note The maximum value is (maximum uint32)/1000.
*
* \return
* Error status.
* See SCB_I2CMasterSendStart() function for constants.
*
* \sideeffect
* A valid Start or ReStart condition must be generated before calling
* this function. This function does nothing if Start or ReStart condition
* failed before this function was called.
*
* \globalvars
* SCB_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 SCB_I2CMasterWriteByte(uint32 wrByte, uint32 timeoutMs)
{
    uint32 errStatus = SCB_I2C_MSTR_NOT_READY;

    /* Check FSM state before write byte */
    if(SCB_CHECK_I2C_MASTER_ACTIVE)
    {
        uint32 locStatus;
        uint32 timeout = SCB_I2C_CONVERT_TIMEOUT_TO_US(timeoutMs);

        SCB_TX_FIFO_WR_REG = wrByte;

        /* Wait for a completion event from the master */
        do
        {
            locStatus  = SCB_GetMasterInterruptSource() & SCB_INTR_MASTER_SEND_BYTE;
            locStatus |= SCB_I2CMasterWaitOneUnit(&timeout);
        }
        while (0u == locStatus);

        /* Handle completion event and convert to status */
        errStatus = SCB_I2CMasterHandleStatus(locStatus);
    }

    return(errStatus);
}



/*******************************************************************************
* Function Name: SCB_I2CMasterReadByte
****************************************************************************//**
*
* Reads one byte from a slave and generates ACK or prepares to generate NAK.
* The NAK will be generated before Stop or ReStart condition by
* SCB_I2CMasterSendStop() or
* SCB_I2CMasterSendRestart() function appropriately.
* This function is blocking. It does not return until a byte is received or
* an error occurs.
*
* \param ackNack
* Response to received byte.
*  - SCB_I2C_ACK_DATA - Generates ACK.
*     The master notifies slave that transfer continues.
*  - SCB_I2C_NAK_DATA - Prepares to generate NAK.
*     The master will notify slave that transfer is completed.
*
* \param rdByte
* The pointer to the location to store the data byte that was read from
* the slave.
* Note that the byte should be ignored if error status is returned.
*
* \param timeoutMs
* Defines in milliseconds the time that this function can block for.
* If that time expires, the function returns. If a zero is passed,
* the function waits forever for the action to complete. If a timeout occurs,
* the SCB block is reset. Note The maximum value is (maximum uint32)/1000.
*
* \return
* Error status.
* See SCB_I2CMasterSendStart() function for constants.
*
* \sideeffect
* A valid Start or ReStart condition must be generated before calling
* this function. This function does nothing if Start or ReStart condition
* failed before this function was called.
*
* \globalvars
* SCB_state - used to store current state of software FSM.
*
*******************************************************************************/
uint32 SCB_I2CMasterReadByte(uint32 ackNack, uint8 *rdByte, uint32 timeoutMs)
{
    uint32 errStatus = SCB_I2C_MSTR_NOT_READY;

    /* Check FSM state before read byte */
    if(SCB_CHECK_I2C_MASTER_ACTIVE)
    {
        uint32 locStatus;
        uint32 rxNotEmpty;
        uint32 timeout = SCB_I2C_CONVERT_TIMEOUT_TO_US(timeoutMs);

        /* Wait for a completion event from the master */
        do
        {
            locStatus  = SCB_GetMasterInterruptSource() & SCB_INTR_MASTER_RECEIVE_BYTE;
            rxNotEmpty = SCB_GetRxInterruptSource() & SCB_INTR_RX_NOT_EMPTY;
            locStatus |= SCB_I2CMasterWaitOneUnit(&timeout);
        }
        while ((0u == locStatus) && (0u == rxNotEmpty));

        /* Get byte from RX FIFO */
        if (0u != rxNotEmpty)
        {
            *rdByte = (uint8)SCB_RX_FIFO_RD_REG;
            SCB_ClearRxInterruptSource(SCB_INTR_RX_ALL);
        }

        /* Handle completion event and convert to status */
        errStatus = SCB_I2CMasterHandleStatus(locStatus);

        if (SCB_I2C_MSTR_NO_ERROR == errStatus)
        {
            if(SCB_I2C_ACK_DATA == ackNack)
            {
                SCB_I2C_MASTER_GENERATE_ACK;
            }
            else
            {
                /* NACK is generated by Stop or ReStart command */
            }
        }
    }

    return(errStatus);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterGetReadBufSize
****************************************************************************//**
*
*  Returns the number of bytes that has been transferred with an
*  SCB_I2CMasterReadBuf() function.
*
* \return
*  Byte count of transfer. If the transfer is not yet complete, it returns
*  the byte count transferred so far.
*
* \sideeffect
*  This function returns not valid value if
*  SCB_I2C_MSTAT_ERR_ARB_LOST or
*  SCB_I2C_MSTAT_ERR_BUS_ERROR occurred while read transfer.
*
* \globalvars
*  SCB_mstrRdBufIndex - used to current index within master read
*  buffer.
*
*******************************************************************************/
uint32 SCB_I2CMasterGetReadBufSize(void)
{
    return(SCB_mstrRdBufIndex);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterGetWriteBufSize
****************************************************************************//**
*
*  Returns the number of bytes that have been transferred with an
*  SCB_I2CMasterWriteBuf() function.
*
* \return
*   Byte count of transfer. If the transfer is not yet complete, it returns
*   zero unit transfer completion.
*
* \sideeffect
*   This function returns not valid value if
*   SCB_I2C_MSTAT_ERR_ARB_LOST or
*   SCB_I2C_MSTAT_ERR_BUS_ERROR occurred while read transfer.
*
* \globalvars
*  SCB_mstrWrBufIndex - used to current index within master write
*  buffer.
*
*******************************************************************************/
uint32 SCB_I2CMasterGetWriteBufSize(void)
{
    return(SCB_mstrWrBufIndex);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterClearReadBuf
****************************************************************************//**
*
*  Resets the read buffer pointer back to the first byte in the buffer.
*
* \globalvars
*  SCB_mstrRdBufIndex - used to current index within master read
*   buffer.
*  SCB_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
void SCB_I2CMasterClearReadBuf(void)
{
    SCB_DisableInt();  /* Lock from interruption */

    SCB_mstrRdBufIndex = 0u;
    SCB_mstrStatus    &= (uint16) ~SCB_I2C_MSTAT_RD_CMPLT;

    SCB_EnableInt();   /* Release lock */
}


/*******************************************************************************
* Function Name: SCB_I2CMasterClearWriteBuf
****************************************************************************//**
*
*  Resets the write buffer pointer back to the first byte in the buffer.
*
* \globalvars
*  SCB_mstrRdBufIndex - used to current index within master read
*   buffer.
*  SCB_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
void SCB_I2CMasterClearWriteBuf(void)
{
    SCB_DisableInt();  /* Lock from interruption */

    SCB_mstrWrBufIndex = 0u;
    SCB_mstrStatus    &= (uint16) ~SCB_I2C_MSTAT_WR_CMPLT;

    SCB_EnableInt();   /* Release lock */
}


/*******************************************************************************
* Function Name: SCB_I2CMasterStatus
****************************************************************************//**
*
*  Returns the master's communication status.
*
* \return
*  Current status of I2C master. This status incorporates status constants.
*  Each constant is a bit field value. The value returned may have multiple
*  bits set to indicate the status of the read or write transfer.
*  - SCB_I2C_MSTAT_RD_CMPLT - Read transfer complete.
*    The error condition status bits must be checked to ensure that
*    read transfer was completed successfully.
*  - SCB_I2C_MSTAT_WR_CMPLT - Write transfer complete.
*    The error condition status bits must be checked to ensure that write
*    transfer was completed successfully.
*  - SCB_I2C_MSTAT_XFER_INP - Transfer in progress.
*  - SCB_I2C_MSTAT_XFER_HALT - Transfer has been halted.
*    The I2C bus is waiting for ReStart or Stop condition generation.
*  - SCB_I2C_MSTAT_ERR_SHORT_XFER - Error condition: Write
*    transfer completed before all bytes were transferred. The slave NAKed
*    the byte which was expected to be ACKed.
*  - SCB_I2C_MSTAT_ERR_ADDR_NAK - Error condition: Slave did
*    not acknowledge address.
*  - SCB_I2C_MSTAT_ERR_ARB_LOST - Error condition: Master lost
*    arbitration during communications with slave.
*  - SCB_I2C_MSTAT_ERR_BUS_ERROR - Error condition: bus error
*    occurred during master transfer due to misplaced Start or Stop
*    condition on the bus.
*  - SCB_I2C_MSTAT_ERR_ABORT_XFER - Error condition: Slave was
*    addressed by another master while master performed the start condition
*    generation. As a result, master has automatically switched to slave
*    mode and is responding. The master transaction has not taken place
*    This error condition only applicable for Multi-Master-Slave mode.
*  - SCB_I2C_MSTAT_ERR_XFER - Error condition: This is the
*    ORed value of all error conditions provided above.
*
* \globalvars
*  SCB_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
uint32 SCB_I2CMasterStatus(void)
{
    uint32 status;

    SCB_DisableInt();  /* Lock from interruption */

    status = (uint32) SCB_mstrStatus;

    if (SCB_CHECK_I2C_MASTER_ACTIVE)
    {
        /* Add status of master pending transaction: MSTAT_XFER_INP */
        status |= (uint32) SCB_I2C_MSTAT_XFER_INP;
    }

    SCB_EnableInt();   /* Release lock */

    return(status);
}


/*******************************************************************************
* Function Name: SCB_I2CMasterClearStatus
****************************************************************************//**
*
*  Clears all status flags and returns the master status.
*
* \return
*  Current status of master. See the SCB_I2CMasterStatus()
*  function for constants.
*
* \globalvars
*  SCB_mstrStatus - used to store current status of I2C Master.
*
*******************************************************************************/
uint32 SCB_I2CMasterClearStatus(void)
{
    uint32 status;

    SCB_DisableInt();  /* Lock from interruption */

    /* Read and clear master status */
    status = (uint32) SCB_mstrStatus;
    SCB_mstrStatus = SCB_I2C_MSTAT_CLEAR;

    SCB_EnableInt();   /* Release lock */

    return(status);
}


/*******************************************************************************
* Function Name: SCB_I2CReStartGeneration
****************************************************************************//**
*
*  Generates a ReStart condition:
*  - SCB IP V1 and later: Generates ReStart using the scb IP functionality
*    Sets the I2C_MASTER_CMD_M_START and I2C_MASTER_CMD_M_NACK (if the previous
*    transaction was read) bits in the SCB.I2C_MASTER_CMD register.
*    This combination forces the master to generate ReStart.
*
*  - SCB IP V0: Generates Restart using the GPIO and scb IP functionality.
*    After the master completes write or read, the SCL is stretched.
*    The master waits until SDA line is released by the slave. Then the GPIO
*    function is enabled and the scb IP disabled as it already does not drive
*    the bus. In case of the previous transfer was read, the NACK is generated
*    by the GPIO. The delay of tLOW is added to manage the hold time.
*    Set I2C_M_CMD.START and enable the scb IP. The ReStart generation
*    is started after the I2C function is enabled for the SCL.
*    Note1: the scb IP due re-enable generates Start but on the I2C bus it
*           appears as ReStart.
*    Note2: the I2C_M_CMD.START is queued if scb IP is disabled.
*    Note3: the I2C_STATUS_M_READ is cleared is address was NACKed before.
*
* \sideeffect
*  SCB IP V0: The NACK generation by the GPIO may cause a greater SCL period
*             than expected for the selected master data rate.
*
*******************************************************************************/
void SCB_I2CReStartGeneration(void)
{
#if(SCB_CY_SCBIP_V0)
    /* Generates Restart use GPIO and scb IP functionality. Ticket ID#143715,
    * ID#145238 and ID#173656.
    * In case of timeout the SCB_I2CMasterSendRestart handles this
    * case using own timeout and execute appropriate recovery.
    */

    const uint16 oneUnit = 1u;
    uint32 timeout = SCB_I2C_PHASE_GEN_TIMEOUT;

    /* Store direction of transfer */
    uint32 readDirection = SCB_I2C_STATUS_REG & SCB_I2C_STATUS_M_READ;

    /* Wait until slave releases SDA line: SCL tHIGH is complete */
    while ((SCB_WAIT_SDA_SET_HIGH) && (timeout > 0u))
    {
        CyDelayUs(oneUnit);
        --timeout;
    }

    if (timeout > 0u)
    {
        /* Refill timeout */
        timeout = SCB_I2C_PHASE_GEN_TIMEOUT;

        /* Drive SCL line use GPIO */
        SCB_SET_I2C_SCL_DR       (SCB_I2C_SCL_LOW);
        SCB_SET_I2C_SCL_HSIOM_SEL(SCB_HSIOM_GPIO_SEL);

        /* Disable SCB block */
        SCB_CTRL_REG &= (uint32) ~SCB_CTRL_ENABLED;

        if (0u != readDirection)
        {
            /* Generate NACK for read transactions use GPIO functionality */

            /* Drive tLOW: SCL low phase */
            SCB_SET_I2C_SCL_DR(SCB_I2C_SCL_LOW);
            CyDelayUs(SCB_I2C_TLOW_TIME);

            /* Drive tHIGH: SCL high phase */
            SCB_SET_I2C_SCL_DR(SCB_I2C_SCL_HIGH);
            CyDelayUs(SCB_I2C_THIGH_TIME);

            /* Wait until slave releases SDA line: SCL tHIGH is complete */
            while ((SCB_WAIT_SCL_SET_HIGH) && (timeout > 0u))
            {
                CyDelayUs(oneUnit);
                --timeout;
            }
        }

        if (timeout > 0u)
        {
            /* Drive tLOW: hold time for write and read */
            SCB_SET_I2C_SCL_DR(SCB_I2C_SCL_LOW);
            CyDelayUs(SCB_I2C_TLOW_TIME);

            /* Set Start command, it will appear as ReStart on the bus */
            SCB_I2C_MASTER_CMD_REG  = SCB_I2C_MASTER_CMD_M_START;
        }

        /* Enable SCB block: ReStart will appear on the bus as soon as SCL
        * configuration is restored and bus busy cleared.
        */
        SCB_CTRL_REG |= SCB_CTRL_ENABLED;

        /* Restore SCL configuration */
        SCB_SET_I2C_SCL_HSIOM_SEL(SCB_HSIOM_I2C_SEL);
        SCB_SET_I2C_SCL_DR       (SCB_I2C_SCL_HIGH);
    }
#else
    uint32 cmd;

    /* Generates ReStart use scb IP functionality */
    cmd  = SCB_I2C_MASTER_CMD_M_START;
    cmd |= SCB_CHECK_I2C_STATUS(SCB_I2C_STATUS_M_READ) ?
                (SCB_I2C_MASTER_CMD_M_NACK) : (0u);

    SCB_I2C_MASTER_CMD_REG = cmd;
#endif /* (SCB_CY_SCBIP_V1) */
}

#endif /* (SCB_I2C_MASTER_CONST) */


#if (!SCB_CY_SCBIP_V0 && \
    SCB_I2C_MULTI_MASTER_SLAVE_CONST && SCB_I2C_WAKE_ENABLE_CONST)
    /*******************************************************************************
    * Function Name: SCB_I2CMasterDisableEcAm
    ****************************************************************************//**
    *
    *  Disables externally clocked address match logic to enable master operation
    *  in active mode.
    *
    *******************************************************************************/
    static void SCB_I2CMasterDisableEcAm(void)
    {
        /* Disables externally clocked address match logic to enable master operation
        * in active mode. This applicable only for Multi-Master-Slave when wakeup is
        * enabled. Ticket ID#192742.
        */
        if (0u != (SCB_CTRL_REG & SCB_CTRL_EC_AM_MODE))
        {
            /* Enable external address match logic */
            SCB_Stop();
            SCB_CTRL_REG &= (uint32) ~SCB_CTRL_EC_AM_MODE;
            SCB_Enable();
        }
    }
#endif /* (!SCB_CY_SCBIP_V0) */


/*******************************************************************************
* Function Name: SCB_I2CMasterHandleStatus
****************************************************************************//**
*
* Converts hardware status to firmware status and executes required processing.
*
* \param status
* Hardware I2C master status to handle. It also includes
* SCB_I2C_MASTER_TIMEOUT and SCB_SLAVE_INTR_I2C_ADDR.
*
* \return
* Error status.
* See SCB_I2CMasterSendStart() function for constants.
*
*******************************************************************************/
static uint32 SCB_I2CMasterHandleStatus(uint32 status)
{
    uint32 retStatus;
    uint32 resetBlock = 0u;

    if (0u != (status & SCB_I2C_MASTER_TIMEOUT))
    {
        retStatus  = SCB_I2C_MASTER_TIMEOUT;
        resetBlock = SCB_I2C_RESET_ERROR;
    }
    else if (0u != (SCB_SLAVE_INTR_I2C_ADDR & status))
    {
        /* Abort the master operation, the slave was addressed first */
        retStatus = SCB_I2C_MSTR_ERR_ABORT_START;

        SCB_I2C_MASTER_CMD_REG = 0u;

        SCB_state = SCB_I2C_FSM_IDLE;
    }
    else if (0u != (status & SCB_INTR_MASTER_I2C_ARB_LOST))
    {
        retStatus  = SCB_I2C_MSTR_ERR_ARB_LOST;

        if (SCB_CHECK_I2C_FSM_ADDR)
        {
            /* If slave is not enabled reset IP, otherwise give it a chance to match address */
            if (0u == (SCB_I2C_CTRL_REG & SCB_I2C_CTRL_SLAVE_MODE))
            {
                resetBlock = SCB_I2C_RESET_ERROR;
            }

            SCB_state = SCB_I2C_FSM_IDLE;
        }
        else
        {
            /* Reset IP block when on address stage */
            resetBlock = SCB_I2C_RESET_ERROR;
        }
    }
    else if (0u != (status & SCB_INTR_MASTER_I2C_BUS_ERROR))
    {
        retStatus  = SCB_I2C_MSTR_ERR_BUS_ERR;
        resetBlock = SCB_I2C_RESET_ERROR;
    }
    else if (0u != (status & SCB_INTR_MASTER_I2C_NACK))
    {
        retStatus = SCB_I2C_MSTR_ERR_LB_NAK;
    }
    else /* Includes: INTR_MASTER_I2C_STOP and INTR_MASTER_I2C_ACK */
    {
        retStatus = SCB_I2C_MSTR_NO_ERROR;

        if (0u != (status & SCB_INTR_MASTER_I2C_STOP))
        {
            /* Stop: end of transaction, go to idle state */
            SCB_state = SCB_I2C_FSM_IDLE;
        }
        else
        {
            /* ACK: continue transaction */
            if (SCB_CHECK_I2C_FSM_ADDR)
            {
                /* Move from address state to data */
                SCB_state = (SCB_CHECK_I2C_FSM_RD) ?
                                          SCB_I2C_FSM_MSTR_RD_DATA : SCB_I2C_FSM_MSTR_WR_DATA;
            }
        }
    }

    if (SCB_I2C_RESET_ERROR == resetBlock)
    {
        SCB_I2CFwBlockReset();
    }
    else
    {
        SCB_ClearMasterInterruptSource(SCB_INTR_MASTER_ALL);
    }

    return (retStatus);
}


/******************************************************************************
* Function Name: I2CMasterWaitOneUnit
****************************************************************************//**
*
* Waits for one unit before unblock code execution.
* Note If a timeout value is 0, this function does nothing and returns 0.
*
* \param timeout
* The pointer to a timeout value.
*
* \return
* Returns 0 if a timeout does not expire or the timeout mask.
*
*******************************************************************************/
static uint32 SCB_I2CMasterWaitOneUnit(uint32 *timeout)
{
    const uint16 oneUnit = 1u;
    uint32 status = 0u;

    /* If the timeout equal to 0. Ignore the timeout */
    if (*timeout > 0u)
    {
        CyDelayUs(oneUnit);
        --(*timeout);

        if (0u == *timeout)
        {
            status = SCB_I2C_MASTER_TIMEOUT;
        }
    }

    return (status);
}




/* [] END OF FILE */
