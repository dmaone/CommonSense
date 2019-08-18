/*******************************************************************************
* File Name: Cmp0.c
* Version 2.0
*
* Description:
*  This file provides the power management source code APIs for the
*  Comparator.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Cmp0.h"

static Cmp0_backupStruct Cmp0_backup;


/*******************************************************************************
* Function Name: Cmp0_SaveConfig
********************************************************************************
*
* Summary:
*  Save the current user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
*******************************************************************************/
void Cmp0_SaveConfig(void) 
{
    /* Empty since all are system reset for retention flops */
}


/*******************************************************************************
* Function Name: Cmp0_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
********************************************************************************/
void Cmp0_RestoreConfig(void) 
{
    /* Empty since all are system reset for retention flops */    
}


/*******************************************************************************
* Function Name: Cmp0_Sleep
********************************************************************************
*
* Summary:
*  Stop and Save the user configuration
*
* Parameters:
*  void:
*
* Return:
*  void
*
* Global variables:
*  Cmp0_backup.enableState:  Is modified depending on the enable 
*   state of the block before entering sleep mode.
*
*******************************************************************************/
void Cmp0_Sleep(void) 
{
    /* Save Comp's enable state */    
    if(Cmp0_ACT_PWR_EN == (Cmp0_PWRMGR & Cmp0_ACT_PWR_EN))
    {
        /* Comp is enabled */
        Cmp0_backup.enableState = 1u;
    }
    else
    {
        /* Comp is disabled */
        Cmp0_backup.enableState = 0u;
    }    
    
    Cmp0_Stop();
    Cmp0_SaveConfig();
}


/*******************************************************************************
* Function Name: Cmp0_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Cmp0_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Cmp0_Wakeup(void) 
{
    Cmp0_RestoreConfig();
    
    if(Cmp0_backup.enableState == 1u)
    {
        /* Enable Comp's operation */
        Cmp0_Enable();

    } /* Do nothing if Comp was disabled before */ 
}


/* [] END OF FILE */
