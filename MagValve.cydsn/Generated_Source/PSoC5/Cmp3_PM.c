/*******************************************************************************
* File Name: Cmp3.c
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

#include "Cmp3.h"

static Cmp3_backupStruct Cmp3_backup;


/*******************************************************************************
* Function Name: Cmp3_SaveConfig
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
void Cmp3_SaveConfig(void) 
{
    /* Empty since all are system reset for retention flops */
}


/*******************************************************************************
* Function Name: Cmp3_RestoreConfig
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
void Cmp3_RestoreConfig(void) 
{
    /* Empty since all are system reset for retention flops */    
}


/*******************************************************************************
* Function Name: Cmp3_Sleep
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
*  Cmp3_backup.enableState:  Is modified depending on the enable 
*   state of the block before entering sleep mode.
*
*******************************************************************************/
void Cmp3_Sleep(void) 
{
    /* Save Comp's enable state */    
    if(Cmp3_ACT_PWR_EN == (Cmp3_PWRMGR & Cmp3_ACT_PWR_EN))
    {
        /* Comp is enabled */
        Cmp3_backup.enableState = 1u;
    }
    else
    {
        /* Comp is disabled */
        Cmp3_backup.enableState = 0u;
    }    
    
    Cmp3_Stop();
    Cmp3_SaveConfig();
}


/*******************************************************************************
* Function Name: Cmp3_Wakeup
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
*  Cmp3_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Cmp3_Wakeup(void) 
{
    Cmp3_RestoreConfig();
    
    if(Cmp3_backup.enableState == 1u)
    {
        /* Enable Comp's operation */
        Cmp3_Enable();

    } /* Do nothing if Comp was disabled before */ 
}


/* [] END OF FILE */
