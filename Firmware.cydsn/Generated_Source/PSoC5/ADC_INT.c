/*******************************************************************************
* File Name: ADC_INT.c
* Version 2.0
*
*  Description:
*    This file contains the code that operates during the interrupt
*    service routine.
*
*   Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ADC.h"
#include "cyapicallbacks.h"


/******************************************************************************
* Custom Declarations and Variables
* - add user inlcude files, prototypes and variables between the following
*   #START and #END tags
******************************************************************************/
/* `#START SEQ_ADC_SYS_VAR`  */

/* `#END`  */

#if(ADC_IRQ_REMOVE == 0u)


    /******************************************************************************
    * Function Name: ADC_ISR
    *******************************************************************************
    *
    * Summary:
    *  Handle Interrupt Service Routine.
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
    ******************************************************************************/
    CY_ISR( ADC_ISR )
    {
        #ifdef ADC_ISR_INTERRUPT_CALLBACK
            ADC_ISR_InterruptCallback();
        #endif /* ADC_ISR_INTERRUPT_CALLBACK */
        
        
        /************************************************************************
        *  Custom Code
        *  - add user ISR code between the following #START and #END tags
        *************************************************************************/
          /* `#START MAIN_SEQ_ADC_ISR`  */

          /* `#END`  */
    }

#endif   /* End ADC_IRQ_REMOVE */

/* [] END OF FILE */
