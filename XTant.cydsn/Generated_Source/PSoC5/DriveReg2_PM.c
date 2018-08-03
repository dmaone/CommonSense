/*******************************************************************************
* File Name: DriveReg2_PM.c
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

#include "DriveReg2.h"

/* Check for removal by optimization */
#if !defined(DriveReg2_Sync_ctrl_reg__REMOVED)

static DriveReg2_BACKUP_STRUCT  DriveReg2_backup = {0u};

    
/*******************************************************************************
* Function Name: DriveReg2_SaveConfig
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
void DriveReg2_SaveConfig(void) 
{
    DriveReg2_backup.controlState = DriveReg2_Control;
}


/*******************************************************************************
* Function Name: DriveReg2_RestoreConfig
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
void DriveReg2_RestoreConfig(void) 
{
     DriveReg2_Control = DriveReg2_backup.controlState;
}


/*******************************************************************************
* Function Name: DriveReg2_Sleep
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
void DriveReg2_Sleep(void) 
{
    DriveReg2_SaveConfig();
}


/*******************************************************************************
* Function Name: DriveReg2_Wakeup
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
void DriveReg2_Wakeup(void)  
{
    DriveReg2_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
