/***************************************************************************//**
* \file USB_cls.c
* \version 3.0
*
* \brief
*  This file contains the USB Class request handler.
*
********************************************************************************
* \copyright
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USB_pvt.h"
#include "cyapicallbacks.h"

#if(USB_EXTERN_CLS == USB_FALSE)

/***************************************
* User Implemented Class Driver Declarations.
***************************************/
/* `#START USER_DEFINED_CLASS_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: USB_DispatchClassRqst
****************************************************************************//**
*  This routine dispatches class specific requests depend on interface class.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USB_DispatchClassRqst(void) 
{
    uint8 requestHandled;
    uint8 interfaceNumber;

    /* Get interface to which request is intended. */
    switch (USB_bmRequestTypeReg & USB_RQST_RCPT_MASK)
    {
        case USB_RQST_RCPT_IFC:
            /* Class-specific request directed to interface: wIndexLoReg 
            * contains interface number.
            */
            interfaceNumber = (uint8) USB_wIndexLoReg;
            break;
        
        case USB_RQST_RCPT_EP:
            /* Class-specific request directed to endpoint: wIndexLoReg contains 
            * endpoint number. Find interface related to endpoint, 
            */
            interfaceNumber = USB_EP[USB_wIndexLoReg & USB_DIR_UNUSED].interface;
            break;
            
        default:
            /* Default interface is zero. */
            interfaceNumber = 0u;
            break;
    }

#if (defined(USB_ENABLE_HID_CLASS) ||\
            defined(USB_ENABLE_AUDIO_CLASS) ||\
            defined(USB_ENABLE_CDC_CLASS) ||\
            USB_ENABLE_MSC_CLASS)

    /* Handle class request depends on interface type. */
    switch (USB_interfaceClass[interfaceNumber])
    {
    #if defined(USB_ENABLE_HID_CLASS)
        case USB_CLASS_HID:
            requestHandled = USB_DispatchHIDClassRqst();
            break;
    #endif /* (USB_ENABLE_HID_CLASS) */
            
    #if defined(USB_ENABLE_AUDIO_CLASS)
        case USB_CLASS_AUDIO:
            requestHandled = USB_DispatchAUDIOClassRqst();
            break;
    #endif /* (USB_CLASS_AUDIO) */
            
    #if defined(USB_ENABLE_CDC_CLASS)
        case USB_CLASS_CDC:
            requestHandled = USB_DispatchCDCClassRqst();
            break;
    #endif /* (USB_ENABLE_CDC_CLASS) */
        
    #if (USB_ENABLE_MSC_CLASS)
        case USB_CLASS_MSD:
        #if (USB_HANDLE_MSC_REQUESTS)
            /* MSC requests are handled by the component. */
            requestHandled = USB_DispatchMSCClassRqst();
        #elif defined(USB_DISPATCH_MSC_CLASS_RQST_CALLBACK)
            /* MSC requests are handled by user defined callbcak. */
            requestHandled = USB_DispatchMSCClassRqst_Callback();
        #else
            /* MSC requests are not handled. */
            requestHandled = USB_FALSE;
        #endif /* (USB_HANDLE_MSC_REQUESTS) */
            break;
    #endif /* (USB_ENABLE_MSC_CLASS) */
        
        default:
            /* Request is not handled: unknown class request type. */
            requestHandled = USB_FALSE;
            break;
    }
#else /*No class is defined*/
    if (0u != interfaceNumber)
    {
        /* Suppress warning message */
    }
    requestHandled = USB_FALSE;
#endif /*HID or AUDIO or MSC or CDC class enabled*/

    /* `#START USER_DEFINED_CLASS_CODE` Place your Class request here */

    /* `#END` */

#ifdef USB_DISPATCH_CLASS_RQST_CALLBACK
    if (USB_FALSE == requestHandled)
    {
        requestHandled = USB_DispatchClassRqst_Callback(interfaceNumber);
    }
#endif /* (USB_DISPATCH_CLASS_RQST_CALLBACK) */

    return (requestHandled);
}


/*******************************************************************************
* Additional user functions supporting Class Specific Requests
********************************************************************************/

/* `#START CLASS_SPECIFIC_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif /* USB_EXTERN_CLS */


/* [] END OF FILE */
