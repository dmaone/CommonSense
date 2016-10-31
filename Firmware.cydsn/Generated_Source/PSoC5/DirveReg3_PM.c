/*******************************************************************************
* File Name: DirveReg3_PM.c
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

#include "DirveReg3.h"

/* Check for removal by optimization */
#if !defined(DirveReg3_Sync_ctrl_reg__REMOVED)

static DirveReg3_BACKUP_STRUCT  DirveReg3_backup = {0u};

    
/*******************************************************************************
* Function Name: DirveReg3_SaveConfig
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
void DirveReg3_SaveConfig(void) 
{
    DirveReg3_backup.controlState = DirveReg3_Control;
}


/*******************************************************************************
* Function Name: DirveReg3_RestoreConfig
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
void DirveReg3_RestoreConfig(void) 
{
     DirveReg3_Control = DirveReg3_backup.controlState;
}


/*******************************************************************************
* Function Name: DirveReg3_Sleep
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
void DirveReg3_Sleep(void) 
{
    DirveReg3_SaveConfig();
}


/*******************************************************************************
* Function Name: DirveReg3_Wakeup
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
void DirveReg3_Wakeup(void)  
{
    DirveReg3_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
