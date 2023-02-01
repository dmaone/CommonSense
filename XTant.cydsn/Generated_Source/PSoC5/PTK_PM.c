/*******************************************************************************
* File Name: PTK_PM.c
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

#include "PTK.h"


PTK_BACKUP_STRUCT PTK_backup;


/*******************************************************************************
* Function Name: PTK_SaveConfig
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
*  PTK_backup - used to save component configuration and non-
*  retention registers before enter sleep mode.
*
*******************************************************************************/
void PTK_SaveConfig(void) 
{
    PTK_backup.count = PTK_COUNT_REG;
}


/*******************************************************************************
* Function Name: PTK_Sleep
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
void PTK_Sleep(void) 
{
    if(0u != (PTK_AUX_CONTROL_REG & PTK_COUNTER_START))
    {
        PTK_backup.enableState = 1u;
        PTK_Stop();
    }
    else
    {
        PTK_backup.enableState = 0u;
    }

    PTK_SaveConfig();
}


/*******************************************************************************
* Function Name: PTK_RestoreConfig
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
*  PTK_backup - used to save component configuration and
*  non-retention registers before exit sleep mode.
*
*******************************************************************************/
void PTK_RestoreConfig(void) 
{
    PTK_COUNT_REG = PTK_backup.count;
}


/*******************************************************************************
* Function Name: PTK_Wakeup
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
void PTK_Wakeup(void) 
{
    PTK_RestoreConfig();

    /* Restore enable state */
    if (PTK_backup.enableState != 0u)
    {
        PTK_Enable();
    }
}


/* [] END OF FILE */
