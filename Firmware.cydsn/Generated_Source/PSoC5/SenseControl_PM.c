/*******************************************************************************
* File Name: SenseControl_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "SenseControl.h"

/* Check for removal by optimization */
#if !defined(SenseControl_Sync_ctrl_reg__REMOVED)

static SenseControl_BACKUP_STRUCT  SenseControl_backup = {0u};

    
/*******************************************************************************
* Function Name: SenseControl_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SenseControl_SaveConfig(void) 
{
    SenseControl_backup.controlState = SenseControl_Control;
}


/*******************************************************************************
* Function Name: SenseControl_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void SenseControl_RestoreConfig(void) 
{
     SenseControl_Control = SenseControl_backup.controlState;
}


/*******************************************************************************
* Function Name: SenseControl_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SenseControl_Sleep(void) 
{
    SenseControl_SaveConfig();
}


/*******************************************************************************
* Function Name: SenseControl_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SenseControl_Wakeup(void)  
{
    SenseControl_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
