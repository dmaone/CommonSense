/*******************************************************************************
* File Name: DriveReg0_PM.c
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

#include "DriveReg0.h"

/* Check for removal by optimization */
#if !defined(DriveReg0_Sync_ctrl_reg__REMOVED)

static DriveReg0_BACKUP_STRUCT  DriveReg0_backup = {0u};

    
/*******************************************************************************
* Function Name: DriveReg0_SaveConfig
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
void DriveReg0_SaveConfig(void) 
{
    DriveReg0_backup.controlState = DriveReg0_Control;
}


/*******************************************************************************
* Function Name: DriveReg0_RestoreConfig
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
void DriveReg0_RestoreConfig(void) 
{
     DriveReg0_Control = DriveReg0_backup.controlState;
}


/*******************************************************************************
* Function Name: DriveReg0_Sleep
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
void DriveReg0_Sleep(void) 
{
    DriveReg0_SaveConfig();
}


/*******************************************************************************
* Function Name: DriveReg0_Wakeup
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
void DriveReg0_Wakeup(void)  
{
    DriveReg0_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
