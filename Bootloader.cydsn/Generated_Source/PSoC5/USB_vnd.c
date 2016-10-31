/***************************************************************************//**
* \file USB_vnd.c
* \version 3.10
*
* \brief
*  This file contains the  USB vendor request handler.
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

#if(USB_EXTERN_VND == USB_FALSE)

/***************************************
* Vendor Specific Declarations
***************************************/

/* `#START VENDOR_SPECIFIC_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: USB_HandleVendorRqst
****************************************************************************//**
*
*  This routine provide users with a method to implement vendor specific
*  requests.
*
*  To implement vendor specific requests, add your code in this function to
*  decode and disposition the request.  If the request is handled, your code
*  must set the variable "requestHandled" to TRUE, indicating that the
*  request has been handled.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_HandleVendorRqst(void) 
{
    uint8 requestHandled = USB_FALSE;

    /* Check request direction: D2H or H2D. */
    if (0u != (USB_bmRequestTypeReg & USB_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        switch (USB_bRequestReg)
        {
            case USB_GET_EXTENDED_CONFIG_DESCRIPTOR:
            #if defined(USB_ENABLE_MSOS_STRING)
                USB_currentTD.pData = (volatile uint8 *) &USB_MSOS_CONFIGURATION_DESCR[0u];
                USB_currentTD.count = USB_MSOS_CONFIGURATION_DESCR[0u];
                requestHandled  = USB_InitControlRead();
            #endif /* (USB_ENABLE_MSOS_STRING) */
                break;
            
            default:
                break;
        }
    }

    /* `#START VENDOR_SPECIFIC_CODE` Place your vendor specific request here */

    /* `#END` */

#ifdef USB_HANDLE_VENDOR_RQST_CALLBACK
    if (USB_FALSE == requestHandled)
    {
        requestHandled = USB_HandleVendorRqst_Callback();
    }
#endif /* (USB_HANDLE_VENDOR_RQST_CALLBACK) */

    return (requestHandled);
}


/*******************************************************************************
* Additional user functions supporting Vendor Specific Requests
********************************************************************************/

/* `#START VENDOR_SPECIFIC_FUNCTIONS` Place any additional functions here */

/* `#END` */


#endif /* USB_EXTERN_VND */


/* [] END OF FILE */
