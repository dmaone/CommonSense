/*******************************************************************************
* File Name: SCB_sda.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "SCB_sda.h"

static SCB_sda_BACKUP_STRUCT  SCB_sda_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: SCB_sda_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet SCB_sda_SUT.c usage_SCB_sda_Sleep_Wakeup
*******************************************************************************/
void SCB_sda_Sleep(void)
{
    #if defined(SCB_sda__PC)
        SCB_sda_backup.pcState = SCB_sda_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            SCB_sda_backup.usbState = SCB_sda_CR1_REG;
            SCB_sda_USB_POWER_REG |= SCB_sda_USBIO_ENTER_SLEEP;
            SCB_sda_CR1_REG &= SCB_sda_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(SCB_sda__SIO)
        SCB_sda_backup.sioState = SCB_sda_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        SCB_sda_SIO_REG &= (uint32)(~SCB_sda_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: SCB_sda_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to SCB_sda_Sleep() for an example usage.
*******************************************************************************/
void SCB_sda_Wakeup(void)
{
    #if defined(SCB_sda__PC)
        SCB_sda_PC = SCB_sda_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            SCB_sda_USB_POWER_REG &= SCB_sda_USBIO_EXIT_SLEEP_PH1;
            SCB_sda_CR1_REG = SCB_sda_backup.usbState;
            SCB_sda_USB_POWER_REG &= SCB_sda_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(SCB_sda__SIO)
        SCB_sda_SIO_REG = SCB_sda_backup.sioState;
    #endif
}


/* [] END OF FILE */
