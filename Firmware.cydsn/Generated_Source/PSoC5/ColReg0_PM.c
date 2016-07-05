/*******************************************************************************
* File Name: ColReg0_PM.c
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

#include "ColReg0.h"

/* Check for removal by optimization */
#if !defined(ColReg0_Sync_ctrl_reg__REMOVED)

static ColReg0_BACKUP_STRUCT  ColReg0_backup = {0u};

    
/*******************************************************************************
* Function Name: ColReg0_SaveConfig
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
void ColReg0_SaveConfig(void) 
{
    ColReg0_backup.controlState = ColReg0_Control;
}


/*******************************************************************************
* Function Name: ColReg0_RestoreConfig
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
void ColReg0_RestoreConfig(void) 
{
     ColReg0_Control = ColReg0_backup.controlState;
}


/*******************************************************************************
* Function Name: ColReg0_Sleep
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
void ColReg0_Sleep(void) 
{
    ColReg0_SaveConfig();
}


/*******************************************************************************
* Function Name: ColReg0_Wakeup
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
void ColReg0_Wakeup(void)  
{
    ColReg0_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
