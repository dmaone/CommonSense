/*******************************************************************************
* File Name: Cmp1.c
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

#include "Cmp1.h"

static Cmp1_backupStruct Cmp1_backup;


/*******************************************************************************
* Function Name: Cmp1_SaveConfig
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
void Cmp1_SaveConfig(void) 
{
    /* Empty since all are system reset for retention flops */
}


/*******************************************************************************
* Function Name: Cmp1_RestoreConfig
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
void Cmp1_RestoreConfig(void) 
{
    /* Empty since all are system reset for retention flops */    
}


/*******************************************************************************
* Function Name: Cmp1_Sleep
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
*  Cmp1_backup.enableState:  Is modified depending on the enable 
*   state of the block before entering sleep mode.
*
*******************************************************************************/
void Cmp1_Sleep(void) 
{
    /* Save Comp's enable state */    
    if(Cmp1_ACT_PWR_EN == (Cmp1_PWRMGR & Cmp1_ACT_PWR_EN))
    {
        /* Comp is enabled */
        Cmp1_backup.enableState = 1u;
    }
    else
    {
        /* Comp is disabled */
        Cmp1_backup.enableState = 0u;
    }    
    
    Cmp1_Stop();
    Cmp1_SaveConfig();
}


/*******************************************************************************
* Function Name: Cmp1_Wakeup
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
*  Cmp1_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Cmp1_Wakeup(void) 
{
    Cmp1_RestoreConfig();
    
    if(Cmp1_backup.enableState == 1u)
    {
        /* Enable Comp's operation */
        Cmp1_Enable();

    } /* Do nothing if Comp was disabled before */ 
}


/* [] END OF FILE */
