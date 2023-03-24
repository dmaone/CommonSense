/*******************************************************************************
* File Name: ControlReg0_PM.c
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

#include "ControlReg0.h"

/* Check for removal by optimization */
#if !defined(ControlReg0_Sync_ctrl_reg__REMOVED)

static ControlReg0_BACKUP_STRUCT  ControlReg0_backup = {0u};

    
/*******************************************************************************
* Function Name: ControlReg0_SaveConfig
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
void ControlReg0_SaveConfig(void) 
{
    ControlReg0_backup.controlState = ControlReg0_Control;
}


/*******************************************************************************
* Function Name: ControlReg0_RestoreConfig
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
void ControlReg0_RestoreConfig(void) 
{
     ControlReg0_Control = ControlReg0_backup.controlState;
}


/*******************************************************************************
* Function Name: ControlReg0_Sleep
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
void ControlReg0_Sleep(void) 
{
    ControlReg0_SaveConfig();
}


/*******************************************************************************
* Function Name: ControlReg0_Wakeup
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
void ControlReg0_Wakeup(void)  
{
    ControlReg0_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
