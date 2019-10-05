/***************************************************************************//**
* \file USB_drv.c
* \version 3.20
*
* \brief
*  This file contains the Endpoint 0 Driver for the USBFS Component.  
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USB_pvt.h"
#include "cyapicallbacks.h"


/***************************************
* Global data allocation
***************************************/

volatile T_USB_EP_CTL_BLOCK USB_EP[USB_MAX_EP];

/** Contains the current configuration number, which is set by the host using a 
 * SET_CONFIGURATION request. This variable is initialized to zero in 
 * USBFS_InitComponent() API and can be read by the USBFS_GetConfiguration() 
 * API.*/
volatile uint8 USB_configuration;

/** Contains the current interface number.*/
volatile uint8 USB_interfaceNumber;

/** This variable is set to one after SET_CONFIGURATION and SET_INTERFACE 
 *requests. It can be read by the USBFS_IsConfigurationChanged() API */
volatile uint8 USB_configurationChanged;

/** Contains the current device address.*/
volatile uint8 USB_deviceAddress;

/** This is a two-bit variable that contains power status in the bit 0 
 * (DEVICE_STATUS_BUS_POWERED or DEVICE_STATUS_SELF_POWERED) and remote wakeup 
 * status (DEVICE_STATUS_REMOTE_WAKEUP) in the bit 1. This variable is 
 * initialized to zero in USBFS_InitComponent() API, configured by the 
 * USBFS_SetPowerStatus() API. The remote wakeup status cannot be set using the 
 * API SetPowerStatus(). */
volatile uint8 USB_deviceStatus;

volatile uint8 USB_interfaceSetting[USB_MAX_INTERFACES_NUMBER];
volatile uint8 USB_interfaceSetting_last[USB_MAX_INTERFACES_NUMBER];
volatile uint8 USB_interfaceStatus[USB_MAX_INTERFACES_NUMBER];

/** Contains the started device number. This variable is set by the 
 * USBFS_Start() or USBFS_InitComponent() APIs.*/
volatile uint8 USB_device;

/** Initialized class array for each interface. It is used for handling Class 
 * specific requests depend on interface class. Different classes in multiple 
 * alternate settings are not supported.*/
const uint8 CYCODE *USB_interfaceClass;


/***************************************
* Local data allocation
***************************************/

volatile uint8  USB_ep0Toggle;
volatile uint8  USB_lastPacketSize;

/** This variable is used by the communication functions to handle the current 
* transfer state.
* Initialized to TRANS_STATE_IDLE in the USBFS_InitComponent() API and after a 
* complete transfer in the status stage.
* Changed to the TRANS_STATE_CONTROL_READ or TRANS_STATE_CONTROL_WRITE in setup 
* transaction depending on the request type.
*/
volatile uint8  USB_transferState;
volatile T_USB_TD USB_currentTD;
volatile uint8  USB_ep0Mode;
volatile uint8  USB_ep0Count;
volatile uint16 USB_transferByteCount;


/*******************************************************************************
* Function Name: USB_ep_0_Interrupt
****************************************************************************//**
*
*  This Interrupt Service Routine handles Endpoint 0 (Control Pipe) traffic.
*  It dispatches setup requests and handles the data and status stages.
*
*
*******************************************************************************/
CY_ISR(USB_EP_0_ISR)
{
    uint8 tempReg;
    uint8 modifyReg;

#ifdef USB_EP_0_ISR_ENTRY_CALLBACK
    USB_EP_0_ISR_EntryCallback();
#endif /* (USB_EP_0_ISR_ENTRY_CALLBACK) */
    
    tempReg = USB_EP0_CR_REG;
    if ((tempReg & USB_MODE_ACKD) != 0u)
    {
        modifyReg = 1u;
        if ((tempReg & USB_MODE_SETUP_RCVD) != 0u)
        {
            if ((tempReg & USB_MODE_MASK) != USB_MODE_NAK_IN_OUT)
            {
                /* Mode not equal to NAK_IN_OUT: invalid setup */
                modifyReg = 0u;
            }
            else
            {
                USB_HandleSetup();
                
                if ((USB_ep0Mode & USB_MODE_SETUP_RCVD) != 0u)
                {
                    /* SETUP bit set: exit without mode modificaiton */
                    modifyReg = 0u;
                }
            }
        }
        else if ((tempReg & USB_MODE_IN_RCVD) != 0u)
        {
            USB_HandleIN();
        }
        else if ((tempReg & USB_MODE_OUT_RCVD) != 0u)
        {
            USB_HandleOUT();
        }
        else
        {
            modifyReg = 0u;
        }
        
        /* Modify the EP0_CR register */
        if (modifyReg != 0u)
        {
            
            tempReg = USB_EP0_CR_REG;
            
            /* Make sure that SETUP bit is cleared before modification */
            if ((tempReg & USB_MODE_SETUP_RCVD) == 0u)
            {
                /* Update count register */
                tempReg = (uint8) USB_ep0Toggle | USB_ep0Count;
                USB_EP0_CNT_REG = tempReg;
               
                /* Make sure that previous write operaiton was successful */
                if (tempReg == USB_EP0_CNT_REG)
                {
                    /* Repeat until next successful write operation */
                    do
                    {
                        /* Init temporary variable */
                        modifyReg = USB_ep0Mode;
                        
                        /* Unlock register */
                        tempReg = (uint8) (USB_EP0_CR_REG & USB_MODE_SETUP_RCVD);
                        
                        /* Check if SETUP bit is not set */
                        if (0u == tempReg)
                        {
                            /* Set the Mode Register  */
                            USB_EP0_CR_REG = USB_ep0Mode;
                            
                            /* Writing check */
                            modifyReg = USB_EP0_CR_REG & USB_MODE_MASK;
                        }
                    }
                    while (modifyReg != USB_ep0Mode);
                }
            }
        }
    }

    USB_ClearSieInterruptSource(USB_INTR_SIE_EP0_INTR);
	
#ifdef USB_EP_0_ISR_EXIT_CALLBACK
    USB_EP_0_ISR_ExitCallback();
#endif /* (USB_EP_0_ISR_EXIT_CALLBACK) */
}


/*******************************************************************************
* Function Name: USB_HandleSetup
****************************************************************************//**
*
*  This Routine dispatches requests for the four USB request types
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_HandleSetup(void) 
{
    uint8 requestHandled;
    
    /* Clear register lock by SIE (read register) and clear setup bit 
    * (write any value in register).
    */
    requestHandled = (uint8) USB_EP0_CR_REG;
    USB_EP0_CR_REG = (uint8) requestHandled;
    requestHandled = (uint8) USB_EP0_CR_REG;

    if ((requestHandled & USB_MODE_SETUP_RCVD) != 0u)
    {
        /* SETUP bit is set: exit without mode modification. */
        USB_ep0Mode = requestHandled;
    }
    else
    {
        /* In case the previous transfer did not complete, close it out */
        USB_UpdateStatusBlock(USB_XFER_PREMATURE);

        /* Check request type. */
        switch (USB_bmRequestTypeReg & USB_RQST_TYPE_MASK)
        {
            case USB_RQST_TYPE_STD:
                requestHandled = USB_HandleStandardRqst();
                break;
                
            case USB_RQST_TYPE_CLS:
                requestHandled = USB_DispatchClassRqst();
                break;
                
            case USB_RQST_TYPE_VND:
                requestHandled = USB_HandleVendorRqst();
                break;
                
            default:
                requestHandled = USB_FALSE;
                break;
        }
        
        /* If request is not recognized. Stall endpoint 0 IN and OUT. */
        if (requestHandled == USB_FALSE)
        {
            USB_ep0Mode = USB_MODE_STALL_IN_OUT;
        }
    }
}


/*******************************************************************************
* Function Name: USB_HandleIN
****************************************************************************//**
*
*  This routine handles EP0 IN transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_HandleIN(void) 
{
    switch (USB_transferState)
    {
        case USB_TRANS_STATE_IDLE:
            break;
        
        case USB_TRANS_STATE_CONTROL_READ:
            USB_ControlReadDataStage();
            break;
            
        case USB_TRANS_STATE_CONTROL_WRITE:
            USB_ControlWriteStatusStage();
            break;
            
        case USB_TRANS_STATE_NO_DATA_CONTROL:
            USB_NoDataControlStatusStage();
            break;
            
        default:    /* there are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: USB_HandleOUT
****************************************************************************//**
*
*  This routine handles EP0 OUT transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_HandleOUT(void) 
{
    switch (USB_transferState)
    {
        case USB_TRANS_STATE_IDLE:
            break;
        
        case USB_TRANS_STATE_CONTROL_READ:
            USB_ControlReadStatusStage();
            break;
            
        case USB_TRANS_STATE_CONTROL_WRITE:
            USB_ControlWriteDataStage();
            break;
            
        case USB_TRANS_STATE_NO_DATA_CONTROL:
            /* Update the completion block */
            USB_UpdateStatusBlock(USB_XFER_ERROR);
            
            /* We expect no more data, so stall INs and OUTs */
            USB_ep0Mode = USB_MODE_STALL_IN_OUT;
            break;
            
        default:    
            /* There are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: USB_LoadEP0
****************************************************************************//**
*
*  This routine loads the EP0 data registers for OUT transfers. It uses the
*  currentTD (previously initialized by the _InitControlWrite function and
*  updated for each OUT transfer, and the bLastPacketSize) to determine how
*  many uint8s to transfer on the current OUT.
*
*  If the number of uint8s remaining is zero and the last transfer was full,
*  we need to send a zero length packet.  Otherwise we send the minimum
*  of the control endpoint size (8) or remaining number of uint8s for the
*  transaction.
*
*
* \globalvars
*  USB_transferByteCount - Update the transfer byte count from the
*     last transaction.
*  USB_ep0Count - counts the data loaded to the SIE memory in
*     current packet.
*  USB_lastPacketSize - remembers the USBFS_ep0Count value for the
*     next packet.
*  USB_transferByteCount - sum of the previous bytes transferred
*     on previous packets(sum of USBFS_lastPacketSize)
*  USB_ep0Toggle - inverted
*  USB_ep0Mode  - prepare for mode register content.
*  USB_transferState - set to TRANS_STATE_CONTROL_READ
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_LoadEP0(void) 
{
    uint8 ep0Count = 0u;

    /* Update the transfer byte count from the last transaction */
    USB_transferByteCount += USB_lastPacketSize;

    /* Now load the next transaction */
    while ((USB_currentTD.count > 0u) && (ep0Count < 8u))
    {
        USB_EP0_DR_BASE.epData[ep0Count] = (uint8) *USB_currentTD.pData;
        USB_currentTD.pData = &USB_currentTD.pData[1u];
        ep0Count++;
        USB_currentTD.count--;
    }

    /* Support zero-length packet */
    if ((USB_lastPacketSize == 8u) || (ep0Count > 0u))
    {
        /* Update the data toggle */
        USB_ep0Toggle ^= USB_EP0_CNT_DATA_TOGGLE;
        /* Set the Mode Register  */
        USB_ep0Mode = USB_MODE_ACK_IN_STATUS_OUT;
        /* Update the state (or stay the same) */
        USB_transferState = USB_TRANS_STATE_CONTROL_READ;
    }
    else
    {
        /* Expect Status Stage Out */
        USB_ep0Mode = USB_MODE_STATUS_OUT_ONLY;
        /* Update the state (or stay the same) */
        USB_transferState = USB_TRANS_STATE_CONTROL_READ;
    }

    /* Save the packet size for next time */
    USB_ep0Count =       (uint8) ep0Count;
    USB_lastPacketSize = (uint8) ep0Count;
}


/*******************************************************************************
* Function Name: USB_InitControlRead
****************************************************************************//**
*
*  Initialize a control read transaction. It is used to send data to the host.
*  The following global variables should be initialized before this function
*  called. To send zero length packet use InitZeroLengthControlTransfer
*  function.
*
*
* \return
*  requestHandled state.
*
* \globalvars
*  USB_currentTD.count - counts of data to be sent.
*  USB_currentTD.pData - data pointer.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_InitControlRead(void) 
{
    uint16 xferCount;

    if (USB_currentTD.count == 0u)
    {
        (void) USB_InitZeroLengthControlTransfer();
    }
    else
    {
        /* Set up the state machine */
        USB_transferState = USB_TRANS_STATE_CONTROL_READ;
        
        /* Set the toggle, it gets updated in LoadEP */
        USB_ep0Toggle = 0u;
        
        /* Initialize the Status Block */
        USB_InitializeStatusBlock();
        
        xferCount = ((uint16)((uint16) USB_lengthHiReg << 8u) | ((uint16) USB_lengthLoReg));

        if (USB_currentTD.count > xferCount)
        {
            USB_currentTD.count = xferCount;
        }
        
        USB_LoadEP0();
    }

    return (USB_TRUE);
}


/*******************************************************************************
* Function Name: USB_InitZeroLengthControlTransfer
****************************************************************************//**
*
*  Initialize a zero length data IN transfer.
*
* \return
*  requestHandled state.
*
* \globalvars
*  USB_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USB_ep0Mode  - prepare for mode register content.
*  USB_transferState - set to TRANS_STATE_CONTROL_READ
*  USB_ep0Count - cleared, means the zero-length packet.
*  USB_lastPacketSize - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_InitZeroLengthControlTransfer(void)
                                                
{
    /* Update the state */
    USB_transferState = USB_TRANS_STATE_CONTROL_READ;
    
    /* Set the data toggle */
    USB_ep0Toggle = USB_EP0_CNT_DATA_TOGGLE;
    
    /* Set the Mode Register  */
    USB_ep0Mode = USB_MODE_ACK_IN_STATUS_OUT;
    
    /* Save the packet size for next time */
    USB_lastPacketSize = 0u;
    
    USB_ep0Count = 0u;

    return (USB_TRUE);
}


/*******************************************************************************
* Function Name: USB_ControlReadDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control read transfer.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_ControlReadDataStage(void) 

{
    USB_LoadEP0();
}


/*******************************************************************************
* Function Name: USB_ControlReadStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control read transfer.
*
*
* \globalvars
*  USB_USBFS_transferByteCount - updated with last packet size.
*  USB_transferState - set to TRANS_STATE_IDLE.
*  USB_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_ControlReadStatusStage(void) 
{
    /* Update the transfer byte count */
    USB_transferByteCount += USB_lastPacketSize;
    
    /* Go Idle */
    USB_transferState = USB_TRANS_STATE_IDLE;
    
    /* Update the completion block */
    USB_UpdateStatusBlock(USB_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    USB_ep0Mode = USB_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USB_InitControlWrite
****************************************************************************//**
*
*  Initialize a control write transaction
*
* \return
*  requestHandled state.
*
* \globalvars
*  USB_USBFS_transferState - set to TRANS_STATE_CONTROL_WRITE
*  USB_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USB_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_InitControlWrite(void) 
{
    uint16 xferCount;

    /* Set up the state machine */
    USB_transferState = USB_TRANS_STATE_CONTROL_WRITE;
    
    /* This might not be necessary */
    USB_ep0Toggle = USB_EP0_CNT_DATA_TOGGLE;
    
    /* Initialize the Status Block */
    USB_InitializeStatusBlock();

    xferCount = ((uint16)((uint16) USB_lengthHiReg << 8u) | ((uint16) USB_lengthLoReg));

    if (USB_currentTD.count > xferCount)
    {
        USB_currentTD.count = xferCount;
    }

    /* Expect Data or Status Stage */
    USB_ep0Mode = USB_MODE_ACK_OUT_STATUS_IN;

    return(USB_TRUE);
}


/*******************************************************************************
* Function Name: USB_ControlWriteDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control write transfer
*       1. Get the data (We assume the destination was validated previously)
*       2. Update the count and data toggle
*       3. Update the mode register for the next transaction
*
*
* \globalvars
*  USB_transferByteCount - Update the transfer byte count from the
*    last transaction.
*  USB_ep0Count - counts the data loaded from the SIE memory
*    in current packet.
*  USB_transferByteCount - sum of the previous bytes transferred
*    on previous packets(sum of USBFS_lastPacketSize)
*  USB_ep0Toggle - inverted
*  USB_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_ControlWriteDataStage(void) 
{
    uint8 ep0Count;
    uint8 regIndex = 0u;

    ep0Count = (USB_EP0_CNT_REG & USB_EPX_CNT0_MASK) - USB_EPX_CNTX_CRC_COUNT;

    USB_transferByteCount += (uint8)ep0Count;

    while ((USB_currentTD.count > 0u) && (ep0Count > 0u))
    {
        *USB_currentTD.pData = (uint8) USB_EP0_DR_BASE.epData[regIndex];
        USB_currentTD.pData = &USB_currentTD.pData[1u];
        regIndex++;
        ep0Count--;
        USB_currentTD.count--;
    }
    
    USB_ep0Count = (uint8)ep0Count;
    
    /* Update the data toggle */
    USB_ep0Toggle ^= USB_EP0_CNT_DATA_TOGGLE;
    
    /* Expect Data or Status Stage */
    USB_ep0Mode = USB_MODE_ACK_OUT_STATUS_IN;
}


/*******************************************************************************
* Function Name: USB_ControlWriteStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control write transfer
*
* \globalvars
*  USB_transferState - set to TRANS_STATE_IDLE.
*  USB_USBFS_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_ControlWriteStatusStage(void) 
{
    /* Go Idle */
    USB_transferState = USB_TRANS_STATE_IDLE;
    
    /* Update the completion block */    
    USB_UpdateStatusBlock(USB_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    USB_ep0Mode = USB_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USB_InitNoDataControlTransfer
****************************************************************************//**
*
*  Initialize a no data control transfer
*
* \return
*  requestHandled state.
*
* \globalvars
*  USB_transferState - set to TRANS_STATE_NO_DATA_CONTROL.
*  USB_ep0Mode  - set to MODE_STATUS_IN_ONLY.
*  USB_ep0Count - cleared.
*  USB_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_InitNoDataControlTransfer(void) 
{
    USB_transferState = USB_TRANS_STATE_NO_DATA_CONTROL;
    USB_ep0Mode       = USB_MODE_STATUS_IN_ONLY;
    USB_ep0Toggle     = USB_EP0_CNT_DATA_TOGGLE;
    USB_ep0Count      = 0u;

    return (USB_TRUE);
}


/*******************************************************************************
* Function Name: USB_NoDataControlStatusStage
****************************************************************************//**
*  Handle the Status Stage of a no data control transfer.
*
*  SET_ADDRESS is special, since we need to receive the status stage with
*  the old address.
*
* \globalvars
*  USB_transferState - set to TRANS_STATE_IDLE.
*  USB_ep0Mode  - set to MODE_STALL_IN_OUT.
*  USB_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USB_deviceAddress - used to set new address and cleared
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_NoDataControlStatusStage(void) 
{
    if (0u != USB_deviceAddress)
    {
        /* Update device address if we got new address. */
        USB_CR0_REG = (uint8) USB_deviceAddress | USB_CR0_ENABLE;
        USB_deviceAddress = 0u;
    }

    USB_transferState = USB_TRANS_STATE_IDLE;
    
    /* Update the completion block. */
    USB_UpdateStatusBlock(USB_XFER_STATUS_ACK);
    
    /* Stall IN and OUT, no more data is expected. */
    USB_ep0Mode = USB_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USB_UpdateStatusBlock
****************************************************************************//**
*
*  Update the Completion Status Block for a Request.  The block is updated
*  with the completion code the USB_transferByteCount.  The
*  StatusBlock Pointer is set to NULL.
*
*  completionCode - status.
*
*
* \globalvars
*  USB_currentTD.pStatusBlock->status - updated by the
*    completionCode parameter.
*  USB_currentTD.pStatusBlock->length - updated.
*  USB_currentTD.pStatusBlock - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_UpdateStatusBlock(uint8 completionCode) 
{
    if (USB_currentTD.pStatusBlock != NULL)
    {
        USB_currentTD.pStatusBlock->status = completionCode;
        USB_currentTD.pStatusBlock->length = USB_transferByteCount;
        USB_currentTD.pStatusBlock = NULL;
    }
}


/*******************************************************************************
* Function Name: USB_InitializeStatusBlock
****************************************************************************//**
*
*  Initialize the Completion Status Block for a Request.  The completion
*  code is set to USB_XFER_IDLE.
*
*  Also, initializes USB_transferByteCount.  Save some space,
*  this is the only consumer.
*
* \globalvars
*  USB_currentTD.pStatusBlock->status - set to XFER_IDLE.
*  USB_currentTD.pStatusBlock->length - cleared.
*  USB_transferByteCount - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_InitializeStatusBlock(void) 
{
    USB_transferByteCount = 0u;
    
    if (USB_currentTD.pStatusBlock != NULL)
    {
        USB_currentTD.pStatusBlock->status = USB_XFER_IDLE;
        USB_currentTD.pStatusBlock->length = 0u;
    }
}


/* [] END OF FILE */
