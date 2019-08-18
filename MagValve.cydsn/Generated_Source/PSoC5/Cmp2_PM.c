/*******************************************************************************
* File Name: Cmp2.c
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

#include "Cmp2.h"

static Cmp2_backupStruct Cmp2_backup;


/*******************************************************************************
* Function Name: Cmp2_SaveConfig
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
void Cmp2_SaveConfig(void) 
{
    /* Empty since all are system reset for retention flops */
}


/*******************************************************************************
* Function Name: Cmp2_RestoreConfig
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
void Cmp2_RestoreConfig(void) 
{
    /* Empty since all are system reset for retention flops */    
}


/*******************************************************************************
* Function Name: Cmp2_Sleep
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
*  Cmp2_backup.enableState:  Is modified depending on the enable 
*   state of the block before entering sleep mode.
*
*******************************************************************************/
void Cmp2_Sleep(void) 
{
    /* Save Comp's enable state */    
    if(Cmp2_ACT_PWR_EN == (Cmp2_PWRMGR & Cmp2_ACT_PWR_EN))
    {
        /* Comp is enabled */
        Cmp2_backup.enableState = 1u;
    }
    else
    {
        /* Comp is disabled */
        Cmp2_backup.enableState = 0u;
    }    
    
    Cmp2_Stop();
    Cmp2_SaveConfig();
}


/*******************************************************************************
* Function Name: Cmp2_Wakeup
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
*  Cmp2_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Cmp2_Wakeup(void) 
{
    Cmp2_RestoreConfig();
    
    if(Cmp2_backup.enableState == 1u)
    {
        /* Enable Comp's operation */
        Cmp2_Enable();

    } /* Do nothing if Comp was disabled before */ 
}


/* [] END OF FILE */
