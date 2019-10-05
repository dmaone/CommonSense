/*******************************************************************************
* File Name: SuspendWD_PM.c
* Version 1.0
*
* Description:
*  This file provides Low power mode APIs for Count7 component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SuspendWD.h"


SuspendWD_BACKUP_STRUCT SuspendWD_backup;


/*******************************************************************************
* Function Name: SuspendWD_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component configuration and non-retention registers.
*  This function is called by the Count7_Sleep() function.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  SuspendWD_backup - used to save component configuration and non-
*  retention registers before enter sleep mode.
*
*******************************************************************************/
void SuspendWD_SaveConfig(void) 
{
    SuspendWD_backup.count = SuspendWD_COUNT_REG;
}


/*******************************************************************************
* Function Name: SuspendWD_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for low power mode
*  operation. The Count7_Sleep() API saves the current component state using
*  Count7_SaveConfig() and disables the counter.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SuspendWD_Sleep(void) 
{
    if(0u != (SuspendWD_AUX_CONTROL_REG & SuspendWD_COUNTER_START))
    {
        SuspendWD_backup.enableState = 1u;
        SuspendWD_Stop();
    }
    else
    {
        SuspendWD_backup.enableState = 0u;
    }

    SuspendWD_SaveConfig();
}


/*******************************************************************************
* Function Name: SuspendWD_RestoreConfig
********************************************************************************
*
* Summary:
*  This function restores the component configuration and non-retention
*  registers. This function is called by the Count7_Wakeup() function.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global Variables:
*  SuspendWD_backup - used to save component configuration and
*  non-retention registers before exit sleep mode.
*
*******************************************************************************/
void SuspendWD_RestoreConfig(void) 
{
    SuspendWD_COUNT_REG = SuspendWD_backup.count;
}


/*******************************************************************************
* Function Name: SuspendWD_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when
*  Count7_Sleep() was called. The Count7_Wakeup() function calls the
*  Count7_RestoreConfig() function to restore the configuration.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SuspendWD_Wakeup(void) 
{
    SuspendWD_RestoreConfig();

    /* Restore enable state */
    if (SuspendWD_backup.enableState != 0u)
    {
        SuspendWD_Enable();
    }
}


/* [] END OF FILE */
