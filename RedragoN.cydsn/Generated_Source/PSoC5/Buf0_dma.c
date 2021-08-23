/***************************************************************************
* File Name: Buf0_dma.c  
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
#include <Buf0_dma.H>



/****************************************************************************
* 
* The following defines are available in Cyfitter.h
* 
* 
* 
* Buf0__DRQ_CTL_REG
* 
* 
* Buf0__DRQ_NUMBER
* 
* Number of TD's used by this channel.
* Buf0__NUMBEROF_TDS
* 
* Priority of this channel.
* Buf0__PRIORITY
* 
* True if Buf0_TERMIN_SEL is used.
* Buf0__TERMIN_EN
* 
* TERMIN interrupt line to signal terminate.
* Buf0__TERMIN_SEL
* 
* 
* True if Buf0_TERMOUT0_SEL is used.
* Buf0__TERMOUT0_EN
* 
* 
* TERMOUT0 interrupt line to signal completion.
* Buf0__TERMOUT0_SEL
* 
* 
* True if Buf0_TERMOUT1_SEL is used.
* Buf0__TERMOUT1_EN
* 
* 
* TERMOUT1 interrupt line to signal completion.
* Buf0__TERMOUT1_SEL
* 
****************************************************************************/


/* Zero based index of Buf0 dma channel */
uint8 Buf0_DmaHandle = DMA_INVALID_CHANNEL;

/*********************************************************************
* Function Name: uint8 Buf0_DmaInitalize
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
uint8 Buf0_DmaInitialize(uint8 BurstCount, uint8 ReqestPerBurst, uint16 UpperSrcAddress, uint16 UpperDestAddress) 
{

    /* Allocate a DMA channel. */
    Buf0_DmaHandle = (uint8)Buf0__DRQ_NUMBER;

    /* Configure the channel. */
    (void)CyDmaChSetConfiguration(Buf0_DmaHandle,
                                  BurstCount,
                                  ReqestPerBurst,
                                  (uint8)Buf0__TERMOUT0_SEL,
                                  (uint8)Buf0__TERMOUT1_SEL,
                                  (uint8)Buf0__TERMIN_SEL);

    /* Set the extended address for the transfers */
    (void)CyDmaChSetExtendedAddress(Buf0_DmaHandle, UpperSrcAddress, UpperDestAddress);

    /* Set the priority for this channel */
    (void)CyDmaChPriority(Buf0_DmaHandle, (uint8)Buf0__PRIORITY);
    
    return Buf0_DmaHandle;
}

/*********************************************************************
* Function Name: void Buf0_DmaRelease
**********************************************************************
* Summary:
*   Frees the channel associated with Buf0.
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
void Buf0_DmaRelease(void) 
{
    /* Disable the channel */
    (void)CyDmaChDisable(Buf0_DmaHandle);
}

