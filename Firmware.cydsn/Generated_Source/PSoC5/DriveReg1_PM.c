/*******************************************************************************
* File Name: DriveReg1_PM.c
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

#include "DriveReg1.h"

/* Check for removal by optimization */
#if !defined(DriveReg1_Sync_ctrl_reg__REMOVED)

static DriveReg1_BACKUP_STRUCT  DriveReg1_backup = {0u};

    
/*******************************************************************************
* Function Name: DriveReg1_SaveConfig
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
void DriveReg1_SaveConfig(void) 
{
    DriveReg1_backup.controlState = DriveReg1_Control;
}


/*******************************************************************************
* Function Name: DriveReg1_RestoreConfig
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
void DriveReg1_RestoreConfig(void) 
{
     DriveReg1_Control = DriveReg1_backup.controlState;
}


/*******************************************************************************
* Function Name: DriveReg1_Sleep
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
void DriveReg1_Sleep(void) 
{
    DriveReg1_SaveConfig();
}


/*******************************************************************************
* Function Name: DriveReg1_Wakeup
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
void DriveReg1_Wakeup(void)  
{
    DriveReg1_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
