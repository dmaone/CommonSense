/*******************************************************************************
* File Name: SenseReg0_PM.c
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

#include "SenseReg0.h"

/* Check for removal by optimization */
#if !defined(SenseReg0_Sync_ctrl_reg__REMOVED)

static SenseReg0_BACKUP_STRUCT  SenseReg0_backup = {0u};

    
/*******************************************************************************
* Function Name: SenseReg0_SaveConfig
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
void SenseReg0_SaveConfig(void) 
{
    SenseReg0_backup.controlState = SenseReg0_Control;
}


/*******************************************************************************
* Function Name: SenseReg0_RestoreConfig
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
void SenseReg0_RestoreConfig(void) 
{
     SenseReg0_Control = SenseReg0_backup.controlState;
}


/*******************************************************************************
* Function Name: SenseReg0_Sleep
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
void SenseReg0_Sleep(void) 
{
    SenseReg0_SaveConfig();
}


/*******************************************************************************
* Function Name: SenseReg0_Wakeup
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
void SenseReg0_Wakeup(void)  
{
    SenseReg0_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
