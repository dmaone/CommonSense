/*******************************************************************************
* File Name: ADC0_PM.c
* Version 3.10
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "ADC0.h"


/***************************************
* Local data allocation
***************************************/

static ADC0_BACKUP_STRUCT  ADC0_backup =
{
    ADC0_DISABLED
};


/*******************************************************************************
* Function Name: ADC0_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC0_SaveConfig(void)
{
    /* All configuration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC0_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC0_RestoreConfig(void)
{
    /* All congiguration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC0_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred routine to prepare the component for sleep.
*  The ADC0_Sleep() routine saves the current component state,
*  then it calls the ADC_Stop() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC0_backup - The structure field 'enableState' is modified
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void ADC0_Sleep(void)
{
    if((ADC0_PWRMGR_SAR_REG  & ADC0_ACT_PWR_SAR_EN) != 0u)
    {
        if((ADC0_SAR_CSR0_REG & ADC0_SAR_SOF_START_CONV) != 0u)
        {
            ADC0_backup.enableState = ADC0_ENABLED | ADC0_STARTED;
        }
        else
        {
            ADC0_backup.enableState = ADC0_ENABLED;
        }
        ADC0_Stop();
    }
    else
    {
        ADC0_backup.enableState = ADC0_DISABLED;
    }
}


/*******************************************************************************
* Function Name: ADC0_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred routine to restore the component to the state when
*  ADC0_Sleep() was called. If the component was enabled before the
*  ADC0_Sleep() function was called, the
*  ADC0_Wakeup() function also re-enables the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC0_backup - The structure field 'enableState' is used to
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void ADC0_Wakeup(void)
{
    if(ADC0_backup.enableState != ADC0_DISABLED)
    {
        ADC0_Enable();
        #if(ADC0_DEFAULT_CONV_MODE != ADC0__HARDWARE_TRIGGER)
            if((ADC0_backup.enableState & ADC0_STARTED) != 0u)
            {
                ADC0_StartConvert();
            }
        #endif /* End ADC0_DEFAULT_CONV_MODE != ADC0__HARDWARE_TRIGGER */
    }
}


/* [] END OF FILE */
