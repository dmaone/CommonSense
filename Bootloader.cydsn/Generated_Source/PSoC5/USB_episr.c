/***************************************************************************//**
* \file USB_episr.c
* \version 3.10
*
* \brief
*  This file contains the Data endpoint Interrupt Service Routines.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USB_pvt.h"
#include "USB_cydmac.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


#if (USB_EP1_ISR_ACTIVE)
    /******************************************************************************
    * Function Name: USB_EP_1_ISR
    ***************************************************************************//**
    *
    *  Endpoint 1 Interrupt Service Routine
    *
    ******************************************************************************/
    CY_ISR(USB_EP_1_ISR)
    {

    #ifdef USB_EP_1_ISR_ENTRY_CALLBACK
        USB_EP_1_ISR_EntryCallback();
    #endif /* (USB_EP_1_ISR_ENTRY_CALLBACK) */

        /* `#START EP1_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    
        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP1_INTR);
            
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP1].addr & USB_DIR_IN))
    #endif /* (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP1].epCr0;
            
            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP1) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP1].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP1].apiEpState = USB_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if (USB_midi_out_ep == USB_EP1)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */
    
        /* `#START EP1_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_1_ISR_EXIT_CALLBACK
        USB_EP_1_ISR_ExitCallback();
    #endif /* (USB_EP_1_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }

#endif /* (USB_EP1_ISR_ACTIVE) */


#if (USB_EP2_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_EP_2_ISR
    ****************************************************************************//**
    *
    *  Endpoint 2 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(USB_EP_2_ISR)
    {
    #ifdef USB_EP_2_ISR_ENTRY_CALLBACK
        USB_EP_2_ISR_EntryCallback();
    #endif /* (USB_EP_2_ISR_ENTRY_CALLBACK) */

        /* `#START EP2_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */

        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP2_INTR);

        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP2].addr & USB_DIR_IN))
    #endif /* (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP2].epCr0;
            
            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP2) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP2].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP2].apiEpState = USB_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if (USB_midi_out_ep == USB_EP2)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */        
    
        /* `#START EP2_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_2_ISR_EXIT_CALLBACK
        USB_EP_2_ISR_ExitCallback();
    #endif /* (USB_EP_2_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USB_EP2_ISR_ACTIVE) */


#if (USB_EP3_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_EP_3_ISR
    ****************************************************************************//**
    *
    *  Endpoint 3 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(USB_EP_3_ISR)
    {
    #ifdef USB_EP_3_ISR_ENTRY_CALLBACK
        USB_EP_3_ISR_EntryCallback();
    #endif /* (USB_EP_3_ISR_ENTRY_CALLBACK) */

        /* `#START EP3_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */

        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP3_INTR);    

        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP3].addr & USB_DIR_IN))
    #endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP3].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP3) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP3].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP3].apiEpState = USB_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if (USB_midi_out_ep == USB_EP3)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */        

        /* `#START EP3_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_3_ISR_EXIT_CALLBACK
        USB_EP_3_ISR_ExitCallback();
    #endif /* (USB_EP_3_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USB_EP3_ISR_ACTIVE) */


#if (USB_EP4_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_EP_4_ISR
    ****************************************************************************//**
    *
    *  Endpoint 4 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(USB_EP_4_ISR)
    {
    #ifdef USB_EP_4_ISR_ENTRY_CALLBACK
        USB_EP_4_ISR_EntryCallback();
    #endif /* (USB_EP_4_ISR_ENTRY_CALLBACK) */

        /* `#START EP4_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */

        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP4_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP4].addr & USB_DIR_IN))
    #endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP4].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP4) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP4].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP4].apiEpState = USB_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if(USB_midi_out_ep == USB_EP4)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */        

        /* `#START EP4_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_4_ISR_EXIT_CALLBACK
        USB_EP_4_ISR_ExitCallback();
    #endif /* (USB_EP_4_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USB_EP4_ISR_ACTIVE) */


#if (USB_EP5_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_EP_5_ISR
    ****************************************************************************//**
    *
    *  Endpoint 5 Interrupt Service Routine
    *
    *
    *******************************************************************************/
    CY_ISR(USB_EP_5_ISR)
    {
    #ifdef USB_EP_5_ISR_ENTRY_CALLBACK
        USB_EP_5_ISR_EntryCallback();
    #endif /* (USB_EP_5_ISR_ENTRY_CALLBACK) */

        /* `#START EP5_USER_CODE` Place your code here */

        /* `#END` */

    #if (defined(USB_ENABLE_MIDI_STREAMING) && !defined(USB_MAIN_SERVICE_MIDI_OUT) && \
                 USB_ISR_SERVICE_MIDI_OUT && CY_PSOC3)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */

        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP5_INTR);
    
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP5].addr & USB_DIR_IN))
    #endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP5].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP5) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP5].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP5].apiEpState = USB_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))        
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if (USB_midi_out_ep == USB_EP5)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP5_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_5_ISR_EXIT_CALLBACK
        USB_EP_5_ISR_ExitCallback();
    #endif /* (USB_EP_5_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USB_EP5_ISR_ACTIVE) */


#if (USB_EP6_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_EP_6_ISR
    ****************************************************************************//**
    *
    *  Endpoint 6 Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(USB_EP_6_ISR)
    {
    #ifdef USB_EP_6_ISR_ENTRY_CALLBACK
        USB_EP_6_ISR_EntryCallback();
    #endif /* (USB_EP_6_ISR_ENTRY_CALLBACK) */

        /* `#START EP6_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */

        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP6_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP6].addr & USB_DIR_IN))
    #endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP6].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP6) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP6].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }
            
            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP6].apiEpState = USB_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if (USB_midi_out_ep == USB_EP6)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP6_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_6_ISR_EXIT_CALLBACK
        USB_EP_6_ISR_ExitCallback();
    #endif /* (USB_EP_6_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USB_EP6_ISR_ACTIVE) */


#if (USB_EP7_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_EP_7_ISR
    ****************************************************************************//**
    *
    *  Endpoint 7 Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(USB_EP_7_ISR)
    {
    #ifdef USB_EP_7_ISR_ENTRY_CALLBACK
        USB_EP_7_ISR_EntryCallback();
    #endif /* (USB_EP_7_ISR_ENTRY_CALLBACK) */

        /* `#START EP7_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    
        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP7_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP7].addr & USB_DIR_IN))
    #endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */
        {           
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP7].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP7) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP7].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }
            
            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP7].apiEpState = USB_EVENT_PENDING;
        }


    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if(USB_midi_out_ep == USB_EP7)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP7_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_7_ISR_EXIT_CALLBACK
        USB_EP_7_ISR_ExitCallback();
    #endif /* (USB_EP_7_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USB_EP7_ISR_ACTIVE) */


#if (USB_EP8_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_EP_8_ISR
    ****************************************************************************//**
    *
    *  Endpoint 8 Interrupt Service Routine
    *
    *
    *******************************************************************************/
    CY_ISR(USB_EP_8_ISR)
    {
    #ifdef USB_EP_8_ISR_ENTRY_CALLBACK
        USB_EP_8_ISR_EntryCallback();
    #endif /* (USB_EP_8_ISR_ENTRY_CALLBACK) */

        /* `#START EP8_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */

        USB_ClearSieEpInterruptSource(USB_SIE_INT_EP8_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USB_EP[USB_EP8].addr & USB_DIR_IN))
    #endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USB_SIE_EP_BASE.sieEp[USB_EP8].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USB_GET_EP_TYPE(USB_EP8) != USB_EP_TYPE_ISOC)
            {
                USB_EP[USB_EP8].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USB_EP[USB_EP8].apiEpState = USB_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USB_ENABLE_MIDI_STREAMING) && \
            !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
            if (USB_midi_out_ep == USB_EP8)
            {
                USB_MIDI_OUT_Service();
            }
        #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP8_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_EP_8_ISR_EXIT_CALLBACK
        USB_EP_8_ISR_ExitCallback();
    #endif /* (USB_EP_8_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USB_ENABLE_MIDI_STREAMING) && \
        !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USB_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USB_EP8_ISR_ACTIVE) */


#if (USB_SOF_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USB_SOF_ISR
    ****************************************************************************//**
    *
    *  Start of Frame Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(USB_SOF_ISR)
    {
    #ifdef USB_SOF_ISR_ENTRY_CALLBACK
        USB_SOF_ISR_EntryCallback();
    #endif /* (USB_SOF_ISR_ENTRY_CALLBACK) */

        /* `#START SOF_USER_CODE` Place your code here */

        /* `#END` */

        USB_ClearSieInterruptSource(USB_INTR_SIE_SOF_INTR);

    #ifdef USB_SOF_ISR_EXIT_CALLBACK
        USB_SOF_ISR_ExitCallback();
    #endif /* (USB_SOF_ISR_EXIT_CALLBACK) */
    }
#endif /* (USB_SOF_ISR_ACTIVE) */


#if (USB_BUS_RESET_ISR_ACTIVE)
/*******************************************************************************
* Function Name: USB_BUS_RESET_ISR
****************************************************************************//**
*
*  USB Bus Reset Interrupt Service Routine.  Calls _Start with the same
*  parameters as the last USER call to _Start
*
*
*******************************************************************************/
CY_ISR(USB_BUS_RESET_ISR)
{
#ifdef USB_BUS_RESET_ISR_ENTRY_CALLBACK
    USB_BUS_RESET_ISR_EntryCallback();
#endif /* (USB_BUS_RESET_ISR_ENTRY_CALLBACK) */

    /* `#START BUS_RESET_USER_CODE` Place your code here */

    /* `#END` */

    USB_ClearSieInterruptSource(USB_INTR_SIE_BUS_RESET_INTR);

    USB_ReInitComponent();

#ifdef USB_BUS_RESET_ISR_EXIT_CALLBACK
    USB_BUS_RESET_ISR_ExitCallback();
#endif /* (USB_BUS_RESET_ISR_EXIT_CALLBACK) */
}
#endif /* (USB_BUS_RESET_ISR_ACTIVE) */


#if (USB_LPM_ACTIVE)
/***************************************************************************
* Function Name: USB_INTR_LPM_ISR
************************************************************************//**
*
*   Interrupt Service Routine for LPM of the interrupt sources.
*
*
***************************************************************************/
CY_ISR(USB_LPM_ISR)
{
#ifdef USB_LPM_ISR_ENTRY_CALLBACK
    USB_LPM_ISR_EntryCallback();
#endif /* (USB_LPM_ISR_ENTRY_CALLBACK) */

    /* `#START LPM_BEGIN_USER_CODE` Place your code here */

    /* `#END` */

    USB_ClearSieInterruptSource(USB_INTR_SIE_LPM_INTR);

    /* `#START LPM_END_USER_CODE` Place your code here */

    /* `#END` */

#ifdef USB_LPM_ISR_EXIT_CALLBACK
    USB_LPM_ISR_ExitCallback();
#endif /* (USB_LPM_ISR_EXIT_CALLBACK) */
}
#endif /* (USB_LPM_ACTIVE) */


#if (USB_EP_MANAGEMENT_DMA && USB_ARB_ISR_ACTIVE)
    /***************************************************************************
    * Function Name: USB_ARB_ISR
    ************************************************************************//**
    *
    *  Arbiter Interrupt Service Routine.
    *
    *
    ***************************************************************************/
    CY_ISR(USB_ARB_ISR)
    {
        uint8 arbIntrStatus;
        uint8 epStatus;
        uint8 ep = USB_EP1;

    #ifdef USB_ARB_ISR_ENTRY_CALLBACK
        USB_ARB_ISR_EntryCallback();
    #endif /* (USB_ARB_ISR_ENTRY_CALLBACK) */

        /* `#START ARB_BEGIN_USER_CODE` Place your code here */

        /* `#END` */

        /* Get pending ARB interrupt sources. */
        arbIntrStatus = USB_ARB_INT_SR_REG;

        while (0u != arbIntrStatus)
        {
            /* Check which EP is interrupt source. */
            if (0u != (arbIntrStatus & 0x01u))
            {
                /* Get endpoint enable interrupt sources. */
                epStatus = (USB_ARB_EP_BASE.arbEp[ep].epSr & USB_ARB_EP_BASE.arbEp[ep].epIntEn);

                /* Handle IN endpoint buffer full event: happens only once when endpoint buffer is loaded. */
                if (0u != (epStatus & USB_ARB_EPX_INT_IN_BUF_FULL))
                {
                    if (0u != (USB_EP[ep].addr & USB_DIR_IN))
                    {
                        /* Clear data ready status. */
                        USB_ARB_EP_BASE.arbEp[ep].epCfg &= (uint8) ~USB_ARB_EPX_CFG_IN_DATA_RDY;

                    #if (CY_PSOC3 || CY_PSOC5LP)
                        #if (USB_EP_MANAGEMENT_DMA_AUTO && (USB_EP_DMA_AUTO_OPT == 0u))
                            /* Set up common area DMA with rest of data. */
                            if(USB_inLength[ep] > USB_DMA_BYTES_PER_BURST)
                            {
                                USB_LoadNextInEP(ep, 0u);
                            }
                            else
                            {
                                USB_inBufFull[ep] = 1u;
                            }
                        #endif /* (USB_EP_MANAGEMENT_DMA_AUTO && (USB_EP_DMA_AUTO_OPT == 0u)) */
                    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

                        /* Arm IN endpoint. */
                        USB_SIE_EP_BASE.sieEp[ep].epCr0 = USB_EP[ep].epMode;

                    #if (defined(USB_ENABLE_MIDI_STREAMING) && USB_ISR_SERVICE_MIDI_IN)
                        if (ep == USB_midi_in_ep)
                        {
                            /* Clear MIDI input pointer. */
                            USB_midiInPointer = 0u;
                        }
                    #endif /* (USB_ENABLE_MIDI_STREAMING) */
                    }
                }

            #if (USB_EP_MANAGEMENT_DMA_MANUAL)
                /* Handle DMA completion event for OUT endpoints. */
                if (0u != (epStatus & USB_ARB_EPX_SR_DMA_GNT))
                {
                    if (0u == (USB_EP[ep].addr & USB_DIR_IN))
                    {
                        /* Notify user that data has been copied from endpoint buffer. */
                        USB_EP[ep].apiEpState = USB_NO_EVENT_PENDING;

                        /* DMA done coping data: OUT endpoint has to be re-armed by user. */
                    }
                }
            #endif /* (USB_EP_MANAGEMENT_DMA_MANUAL) */

            #if (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO)
                /* Handle DMA completion event for OUT endpoints. */
                if (0u != (epStatus & USB_ARB_EPX_INT_DMA_TERMIN))
                {
                    uint32 channelNum = USB_DmaChan[ep];

                    /* Restore burst counter for endpoint. */
                    USB_DmaEpBurstCnt[ep] = USB_DMA_GET_BURST_CNT(USB_DmaEpBurstCntBackup[ep]);

                    /* Disable DMA channel to restore descriptor configuration. The on-going transfer is aborted. */
                    USB_CyDmaChDisable(channelNum);

                    /* Generate DMA tr_out signal to notify USB IP that DMA is done. This signal is not generated
                    * when transfer was aborted (it occurs when host writes less bytes than buffer size).
                    */
                    USB_CyDmaTriggerOut(USB_DmaBurstEndOut[ep]);

                    /* Restore destination address for output endpoint. */
                    USB_CyDmaSetDstAddress(channelNum, USB_DMA_DESCR0, (void*) ((uint32) USB_DmaEpBufferAddrBackup[ep]));
                    USB_CyDmaSetDstAddress(channelNum, USB_DMA_DESCR1, (void*) ((uint32) USB_DmaEpBufferAddrBackup[ep] +
                                                                                                                   USB_DMA_BYTES_PER_BURST));

                    /* Restore number of data elements to transfer which was adjusted for last burst. */
                    if (0u != (USB_DmaEpLastBurstEl[ep] & USB_DMA_DESCR_REVERT))
                    {
                        USB_CyDmaSetNumDataElements(channelNum, (USB_DmaEpLastBurstEl[ep] >> USB_DMA_DESCR_SHIFT),
                                                                             USB_DMA_GET_MAX_ELEM_PER_BURST(USB_DmaEpLastBurstEl[ep]));
                    }

                    /* Validate descriptor 0 and 1 (also reset current state). Command to start with descriptor 0. */
                    USB_CyDmaValidateDescriptor(channelNum, USB_DMA_DESCR0);
                    if (USB_DmaEpBurstCntBackup[ep] > 1u)
                    {
                        USB_CyDmaValidateDescriptor(channelNum, USB_DMA_DESCR1);
                    }
                    USB_CyDmaSetDescriptor0Next(channelNum);

                    /* Enable DMA channel: configuration complete. */
                    USB_CyDmaChEnable(channelNum);
                    
                    
                    /* Read CR0 register to clear SIE lock. */
                    (void) USB_SIE_EP_BASE.sieEp[ep].epCr0;
                    
                    /* Toggle all endpoint types except ISOC. */
                    if (USB_GET_EP_TYPE(ep) != USB_EP_TYPE_ISOC)
                    {
                        USB_EP[ep].epToggle ^= USB_EPX_CNT_DATA_TOGGLE;
                    }
            
                    /* Notify user that data has been copied from endpoint buffer. */
                    USB_EP[ep].apiEpState = USB_EVENT_PENDING;
                    
                #if (defined(USB_ENABLE_MIDI_STREAMING) && \
                    !defined(USB_MAIN_SERVICE_MIDI_OUT) && USB_ISR_SERVICE_MIDI_OUT)
                    if (USB_midi_out_ep == ep)
                    {
                        USB_MIDI_OUT_Service();
                    }
                #endif /* (USB_ISR_SERVICE_MIDI_OUT) */
                }
            #endif /* (CY_PSOC4 && USB_EP_MANAGEMENT_DMA_AUTO) */


                /* `#START ARB_USER_CODE` Place your code here for handle Buffer Underflow/Overflow */

                /* `#END` */

            #ifdef USB_ARB_ISR_CALLBACK
                USB_ARB_ISR_Callback(ep, epStatus);
            #endif /* (USB_ARB_ISR_CALLBACK) */

                /* Clear serviced endpoint interrupt sources. */
                USB_ARB_EP_BASE.arbEp[ep].epSr = epStatus;
            }

            ++ep;
            arbIntrStatus >>= 1u;
        }

        /* `#START ARB_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USB_ARB_ISR_EXIT_CALLBACK
        USB_ARB_ISR_ExitCallback();
    #endif /* (USB_ARB_ISR_EXIT_CALLBACK) */
    }

#endif /*  (USB_ARB_ISR_ACTIVE && USB_EP_MANAGEMENT_DMA) */


#if (USB_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)

    /******************************************************************************
    * Function Name: USB_EPxDmaDone
    ***************************************************************************//**
    *
    * \internal
    *  Endpoint  DMA Done Interrupt Service Routine basic function .
    *  
    *  \param dmaCh
    *  number of DMA channel
    *  
    *  \param ep
    *  number of USB end point
    *  
    *  \param dmaDone
    *  transfer completion flag
    *  
    *  \return
    *   updated transfer completion flag
    *
    ******************************************************************************/
    CY_INLINE static void USB_EPxDmaDone(uint8 dmaCh, uint8 ep)
    {
        uint32 nextAddr;

        /* Manage data elements which remain to transfer. */
        if (0u != USB_DmaEpBurstCnt[ep])
        {
            if(USB_DmaEpBurstCnt[ep] <= 2u)
            {
                /* Adjust length of last burst. */
                USB_CyDmaSetNumDataElements(dmaCh,
                                                    ((uint32) USB_DmaEpLastBurstEl[ep] >> USB_DMA_DESCR_SHIFT),
                                                    ((uint32) USB_DmaEpLastBurstEl[ep] &  USB_DMA_BURST_BYTES_MASK));
            }
            

            /* Advance source for input endpoint or destination for output endpoint. */
            if (0u != (USB_EP[ep].addr & USB_DIR_IN))
            {
                /* Change source for descriptor 0. */
                nextAddr = (uint32) USB_CyDmaGetSrcAddress(dmaCh, USB_DMA_DESCR0);
                nextAddr += (2u * USB_DMA_BYTES_PER_BURST);
                USB_CyDmaSetSrcAddress(dmaCh, USB_DMA_DESCR0, (void *) nextAddr);

                /* Change source for descriptor 1. */
                nextAddr += USB_DMA_BYTES_PER_BURST;
                USB_CyDmaSetSrcAddress(dmaCh, USB_DMA_DESCR1, (void *) nextAddr);
            }
            else
            {
                /* Change destination for descriptor 0. */
                nextAddr  = (uint32) USB_CyDmaGetDstAddress(dmaCh, USB_DMA_DESCR0);
                nextAddr += (2u * USB_DMA_BYTES_PER_BURST);
                USB_CyDmaSetDstAddress(dmaCh, USB_DMA_DESCR0, (void *) nextAddr);

                /* Change destination for descriptor 1. */
                nextAddr += USB_DMA_BYTES_PER_BURST;
                USB_CyDmaSetDstAddress(dmaCh, USB_DMA_DESCR1, (void *) nextAddr);
            }

            /* Enable DMA to execute transfer as it was disabled because there were no valid descriptor. */
            USB_CyDmaValidateDescriptor(dmaCh, USB_DMA_DESCR0);
            
            --USB_DmaEpBurstCnt[ep];
            if (0u != USB_DmaEpBurstCnt[ep])
            {
                USB_CyDmaValidateDescriptor(dmaCh, USB_DMA_DESCR1);
                --USB_DmaEpBurstCnt[ep];
            }
            
            USB_CyDmaChEnable (dmaCh);
            USB_CyDmaTriggerIn(USB_DmaReqOut[ep]);
        }
        else
        {
            /* No data to transfer. False DMA trig. Ignore.  */
        }

    }

    #if (USB_DMA1_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP1_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 1 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP1_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP1_DMA_CH,
                                                  USB_EP1);
                
        }
    #endif /* (USB_DMA1_ACTIVE) */


    #if (USB_DMA2_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP2_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 2 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP2_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP2_DMA_CH,
                                                  USB_EP2);
        }
    #endif /* (USB_DMA2_ACTIVE) */


    #if (USB_DMA3_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP3_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 3 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP3_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP3_DMA_CH,
                                                  USB_EP3);
        }
    #endif /* (USB_DMA3_ACTIVE) */


    #if (USB_DMA4_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP4_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 4 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP4_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP4_DMA_CH,
                                                  USB_EP4);
        }
    #endif /* (USB_DMA4_ACTIVE) */


    #if (USB_DMA5_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP5_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 5 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP5_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP5_DMA_CH,
                                                  USB_EP5);
        }
    #endif /* (USB_DMA5_ACTIVE) */


    #if (USB_DMA6_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP6_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 6 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP6_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP6_DMA_CH,
                                                  USB_EP6);
        }
    #endif /* (USB_DMA6_ACTIVE) */


    #if (USB_DMA7_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP7_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 7 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP7_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP7_DMA_CH,
                                                  USB_EP7);
        }
    #endif /* (USB_DMA7_ACTIVE) */


    #if (USB_DMA8_ACTIVE)
        /******************************************************************************
        * Function Name: USB_EP8_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 8 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USB_EP8_DMA_DONE_ISR(void)
        {

            USB_EPxDmaDone((uint8)USB_EP8_DMA_CH,
                                                  USB_EP8);
        }
    #endif /* (USB_DMA8_ACTIVE) */


#else
    #if (USB_EP_DMA_AUTO_OPT == 0u)
        /******************************************************************************
        * Function Name: USB_EP_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        CY_ISR(USB_EP_DMA_DONE_ISR)
        {
            uint8 int8Status;
            uint8 int17Status;
            uint8 ep_status;
            uint8 ep = USB_EP1;

        #ifdef USB_EP_DMA_DONE_ISR_ENTRY_CALLBACK
            USB_EP_DMA_DONE_ISR_EntryCallback();
        #endif /* (USB_EP_DMA_DONE_ISR_ENTRY_CALLBACK) */

            /* `#START EP_DMA_DONE_BEGIN_USER_CODE` Place your code here */

            /* `#END` */

            /* Read clear on read status register with EP source of interrupt. */
            int17Status = USB_EP17_DMA_Done_SR_Read() & USB_EP17_SR_MASK;
            int8Status  = USB_EP8_DMA_Done_SR_Read()  & USB_EP8_SR_MASK;

            while (int8Status != 0u)
            {
                while (int17Status != 0u)
                {
                    if ((int17Status & 1u) != 0u)  /* If EpX interrupt present. */
                    {
                        /* Read Endpoint Status Register. */
                        ep_status = USB_ARB_EP_BASE.arbEp[ep].epSr;

                        if ((0u == (ep_status & USB_ARB_EPX_SR_IN_BUF_FULL)) &&
                            (0u ==USB_inBufFull[ep]))
                        {
                            /* `#START EP_DMA_DONE_USER_CODE` Place your code here */

                            /* `#END` */

                        #ifdef USB_EP_DMA_DONE_ISR_CALLBACK
                            USB_EP_DMA_DONE_ISR_Callback();
                        #endif /* (USB_EP_DMA_DONE_ISR_CALLBACK) */

                            /* Transfer again 2 last bytes into pre-fetch endpoint area. */
                            USB_ARB_EP_BASE.arbEp[ep].rwWaMsb = 0u;
                            USB_ARB_EP_BASE.arbEp[ep].rwWa = (USB_DMA_BYTES_PER_BURST * ep) - USB_DMA_BYTES_REPEAT;
                            USB_LoadNextInEP(ep, 1u);

                            /* Set Data ready status to generate DMA request. */
                            USB_ARB_EP_BASE.arbEp[ep].epCfg |= USB_ARB_EPX_CFG_IN_DATA_RDY;
                        }
                    }

                    ep++;
                    int17Status >>= 1u;
                }

                int8Status >>= 1u;

                if (int8Status != 0u)
                {
                    /* Prepare pointer for EP8. */
                    ep = USB_EP8;
                    int17Status = int8Status & 0x01u;
                }
            }

            /* `#START EP_DMA_DONE_END_USER_CODE` Place your code here */

            /* `#END` */

    #ifdef USB_EP_DMA_DONE_ISR_EXIT_CALLBACK
        USB_EP_DMA_DONE_ISR_ExitCallback();
    #endif /* (USB_EP_DMA_DONE_ISR_EXIT_CALLBACK) */
        }
    #endif /* (USB_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (USB_EP_MANAGEMENT_DMA_AUTO) */


#if (CY_PSOC4)
    /***************************************************************************
    * Function Name: USB_IntrHandler
    ************************************************************************//**
    *
    *   Interrupt handler for Hi/Mid/Low ISRs.
    *
    *  regCause - The cause register of interrupt. One of the three variants:
    *       USB_INTR_CAUSE_LO_REG - Low interrupts.
    *       USB_INTR_CAUSE_MED_REG - Med interrupts.
    *       USB_INTR_CAUSE_HI_REG - - High interrupts.
    *
    *
    ***************************************************************************/
    CY_INLINE static void USB_IntrHandler(uint32 intrCause)
    {
        /* Array of pointers to component interrupt handlers. */
        static const cyisraddress USB_isrCallbacks[] =
        {

        };

        uint32 cbIdx = 0u;

        /* Check arbiter interrupt source first. */
        if (0u != (intrCause & USB_INTR_CAUSE_ARB_INTR))
        {
            USB_isrCallbacks[USB_ARB_EP_INTR_NUM]();
        }

        /* Check all other interrupt sources (except arbiter and resume). */
        intrCause = (intrCause  & USB_INTR_CAUSE_CTRL_INTR_MASK) |
                    ((intrCause & USB_INTR_CAUSE_EP1_8_INTR_MASK) >>
                                  USB_INTR_CAUSE_EP_INTR_SHIFT);

        /* Call interrupt handlers for active interrupt sources. */
        while (0u != intrCause)
        {
            if (0u != (intrCause & 0x1u))
            {
                 USB_isrCallbacks[cbIdx]();
            }

            intrCause >>= 1u;
            ++cbIdx;
        }
    }


    /***************************************************************************
    * Function Name: USB_INTR_HI_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the high group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(USB_INTR_HI_ISR)
    {
        USB_IntrHandler(USB_INTR_CAUSE_HI_REG);
    }

    /***************************************************************************
    * Function Name: USB_INTR_MED_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the medium group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(USB_INTR_MED_ISR)
    {
       USB_IntrHandler(USB_INTR_CAUSE_MED_REG);
    }

    /***************************************************************************
    * Function Name: USB_INTR_LO_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the low group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(USB_INTR_LO_ISR)
    {
        USB_IntrHandler(USB_INTR_CAUSE_LO_REG);
    }
#endif /* (CY_PSOC4) */


/* [] END OF FILE */
