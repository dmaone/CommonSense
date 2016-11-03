/*******************************************************************************
* File Name: ADC1_PM.c
* Version 3.0
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

#include "ADC1.h"


/***************************************
* Local data allocation
***************************************/

static ADC1_BACKUP_STRUCT  ADC1_backup =
{
    ADC1_DISABLED
};


/*******************************************************************************
* Function Name: ADC1_SaveConfig
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
void ADC1_SaveConfig(void)
{
    /* All configuration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC1_RestoreConfig
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
void ADC1_RestoreConfig(void)
{
    /* All congiguration registers are marked as [reset_all_retention] */
}


/*******************************************************************************
* Function Name: ADC1_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred routine to prepare the component for sleep.
*  The ADC1_Sleep() routine saves the current component state,
*  then it calls the ADC_Stop() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC1_backup - The structure field 'enableState' is modified
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void ADC1_Sleep(void)
{
    if((ADC1_PWRMGR_SAR_REG  & ADC1_ACT_PWR_SAR_EN) != 0u)
    {
        if((ADC1_SAR_CSR0_REG & ADC1_SAR_SOF_START_CONV) != 0u)
        {
            ADC1_backup.enableState = ADC1_ENABLED | ADC1_STARTED;
        }
        else
        {
            ADC1_backup.enableState = ADC1_ENABLED;
        }
        ADC1_Stop();
    }
    else
    {
        ADC1_backup.enableState = ADC1_DISABLED;
    }
}


/*******************************************************************************
* Function Name: ADC1_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred routine to restore the component to the state when
*  ADC1_Sleep() was called. If the component was enabled before the
*  ADC1_Sleep() function was called, the
*  ADC1_Wakeup() function also re-enables the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  ADC1_backup - The structure field 'enableState' is used to
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void ADC1_Wakeup(void)
{
    if(ADC1_backup.enableState != ADC1_DISABLED)
    {
        ADC1_Enable();
        #if(ADC1_DEFAULT_CONV_MODE != ADC1__HARDWARE_TRIGGER)
            if((ADC1_backup.enableState & ADC1_STARTED) != 0u)
            {
                ADC1_StartConvert();
            }
        #endif /* End ADC1_DEFAULT_CONV_MODE != ADC1__HARDWARE_TRIGGER */
    }
}


/* [] END OF FILE */
