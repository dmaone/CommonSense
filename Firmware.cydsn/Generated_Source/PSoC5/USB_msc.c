/***************************************************************************//**
* \file USB_cdc.c
* \version 3.0
*
* \brief
*  This file contains the USB MSC Class request handler and global API for MSC 
*  class.
*
* Related Document:
*  Universal Serial Bus Class Definitions for Communication Devices Version 1.1
*
********************************************************************************
* \copyright
* Copyright 2012-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USB_msc.h"
#include "USB_pvt.h"
#include "cyapicallbacks.h"

#if (USB_HANDLE_MSC_REQUESTS)

/***************************************
*          Internal variables
***************************************/

static uint8 USB_lunCount = USB_MSC_LUN_NUMBER;


/*******************************************************************************
* Function Name: USB_DispatchMSCClassRqst
****************************************************************************//**
*   
*  \internal 
*  This routine dispatches MSC class requests.
*
* \return
*  Status of request processing: handled or not handled.
*
* \globalvars
*  USB_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_DispatchMSCClassRqst(void) 
{
    uint8 requestHandled = USB_FALSE;
    
    /* Get request data. */
    uint16 value  = USB_GET_UINT16(USB_wValueHiReg,  USB_wValueLoReg);
    uint16 dataLength = USB_GET_UINT16(USB_wLengthHiReg, USB_wLengthLoReg);
       
    /* Check request direction: D2H or H2D. */
    if (0u != (USB_bmRequestTypeReg & USB_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        if (USB_MSC_GET_MAX_LUN == USB_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == USB_MSC_GET_MAX_LUN_WVALUE) &&
                (dataLength == USB_MSC_GET_MAX_LUN_WLENGTH))
            {
                /* Reply to Get Max LUN request: setup control read. */
                USB_currentTD.pData = &USB_lunCount;
                USB_currentTD.count =  USB_MSC_GET_MAX_LUN_WLENGTH;
                
                requestHandled  = USB_InitControlRead();
            }
        }
    }
    else
    {
        /* Handle direction from host to device. */
        
        if (USB_MSC_RESET == USB_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == USB_MSC_RESET_WVALUE) &&
                (dataLength == USB_MSC_RESET_WLENGTH))
            {
                /* Handle to Bulk-Only Reset request: no data control transfer. */
                USB_currentTD.count = USB_MSC_RESET_WLENGTH;
                
            #ifdef USB_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK
                USB_DispatchMSCClass_MSC_RESET_RQST_Callback();
            #endif /* (USB_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK) */
                
                requestHandled = USB_InitNoDataControlTransfer();
            }
        }
    }
    
    return (requestHandled);
}


/*******************************************************************************
* Function Name: USB_MSC_SetLunCount
****************************************************************************//**
*
*  This function sets the number of logical units supported in the application. 
*  The default number of logical units is set in the component customizer.
*
*  \param lunCount: Count of the logical units. Valid range is between 1 and 16.
*
*
* \globalvars
*  USB_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
void USB_MSC_SetLunCount(uint8 lunCount) 
{
    USB_lunCount = (lunCount - 1u);
}


/*******************************************************************************
* Function Name: USB_MSC_GetLunCount
****************************************************************************//**
*
*  This function returns the number of logical units.
*
* \return
*   Number of the logical units.
*
* \globalvars
*  USB_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_MSC_GetLunCount(void) 
{
    return (USB_lunCount + 1u);
}   

#endif /* (USB_HANDLE_MSC_REQUESTS) */


/* [] END OF FILE */
