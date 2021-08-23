/***************************************************************************
* File Name: FinalBuf_dma.c  
* Version 1.70
*
*  Description:
*   Provides an API for the DMAC component. The API includes functions
*   for the DMA controller, DMA channels and Transfer Descriptors.
*
*
*   Note:
*     This module requires the developer to finish or fill in the auto
*     generated funcions and setup the dma channel and TD's.
*
********************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/
#include <CYLIB.H>
#include <CYDMAC.H>
#include <FinalBuf_dma.H>



/****************************************************************************
* 
* The following defines are available in Cyfitter.h
* 
* 
* 
* FinalBuf__DRQ_CTL_REG
* 
* 
* FinalBuf__DRQ_NUMBER
* 
* Number of TD's used by this channel.
* FinalBuf__NUMBEROF_TDS
* 
* Priority of this channel.
* FinalBuf__PRIORITY
* 
* True if FinalBuf_TERMIN_SEL is used.
* FinalBuf__TERMIN_EN
* 
* TERMIN interrupt line to signal terminate.
* FinalBuf__TERMIN_SEL
* 
* 
* True if FinalBuf_TERMOUT0_SEL is used.
* FinalBuf__TERMOUT0_EN
* 
* 
* TERMOUT0 interrupt line to signal completion.
* FinalBuf__TERMOUT0_SEL
* 
* 
* True if FinalBuf_TERMOUT1_SEL is used.
* FinalBuf__TERMOUT1_EN
* 
* 
* TERMOUT1 interrupt line to signal completion.
* FinalBuf__TERMOUT1_SEL
* 
****************************************************************************/


/* Zero based index of FinalBuf dma channel */
uint8 FinalBuf_DmaHandle = DMA_INVALID_CHANNEL;

/*********************************************************************
* Function Name: uint8 FinalBuf_DmaInitalize
**********************************************************************
* Summary:
*   Allocates and initialises a channel of the DMAC to be used by the
*   caller.
*
* Parameters:
*   BurstCount.
*       
*       
*   ReqestPerBurst.
*       
*       
*   UpperSrcAddress.
*       
*       
*   UpperDestAddress.
*       
*
* Return:
*   The channel that can be used by the caller for DMA activity.
*   DMA_INVALID_CHANNEL (0xFF) if there are no channels left. 
*
*
*******************************************************************/
uint8 FinalBuf_DmaInitialize(uint8 BurstCount, uint8 ReqestPerBurst, uint16 UpperSrcAddress, uint16 UpperDestAddress) 
{

    /* Allocate a DMA channel. */
    FinalBuf_DmaHandle = (uint8)FinalBuf__DRQ_NUMBER;

    /* Configure the channel. */
    (void)CyDmaChSetConfiguration(FinalBuf_DmaHandle,
                                  BurstCount,
                                  ReqestPerBurst,
                                  (uint8)FinalBuf__TERMOUT0_SEL,
                                  (uint8)FinalBuf__TERMOUT1_SEL,
                                  (uint8)FinalBuf__TERMIN_SEL);

    /* Set the extended address for the transfers */
    (void)CyDmaChSetExtendedAddress(FinalBuf_DmaHandle, UpperSrcAddress, UpperDestAddress);

    /* Set the priority for this channel */
    (void)CyDmaChPriority(FinalBuf_DmaHandle, (uint8)FinalBuf__PRIORITY);
    
    return FinalBuf_DmaHandle;
}

/*********************************************************************
* Function Name: void FinalBuf_DmaRelease
**********************************************************************
* Summary:
*   Frees the channel associated with FinalBuf.
*
*
* Parameters:
*   void.
*
*
*
* Return:
*   void.
*
*******************************************************************/
void FinalBuf_DmaRelease(void) 
{
    /* Disable the channel */
    (void)CyDmaChDisable(FinalBuf_DmaHandle);
}

