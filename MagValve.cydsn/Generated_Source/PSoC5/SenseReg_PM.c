/*******************************************************************************
* File Name: SenseReg_PM.c
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

#include "SenseReg.h"

/* Check for removal by optimization */
#if !defined(SenseReg_Sync_ctrl_reg__REMOVED)

static SenseReg_BACKUP_STRUCT  SenseReg_backup = {0u};

    
/*******************************************************************************
* Function Name: SenseReg_SaveConfig
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
void SenseReg_SaveConfig(void) 
{
    SenseReg_backup.controlState = SenseReg_Control;
}


/*******************************************************************************
* Function Name: SenseReg_RestoreConfig
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
void SenseReg_RestoreConfig(void) 
{
     SenseReg_Control = SenseReg_backup.controlState;
}


/*******************************************************************************
* Function Name: SenseReg_Sleep
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
void SenseReg_Sleep(void) 
{
    SenseReg_SaveConfig();
}


/*******************************************************************************
* Function Name: SenseReg_Wakeup
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
void SenseReg_Wakeup(void)  
{
    SenseReg_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
