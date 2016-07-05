/*******************************************************************************
* File Name: ColReg1_PM.c
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

#include "ColReg1.h"

/* Check for removal by optimization */
#if !defined(ColReg1_Sync_ctrl_reg__REMOVED)

static ColReg1_BACKUP_STRUCT  ColReg1_backup = {0u};

    
/*******************************************************************************
* Function Name: ColReg1_SaveConfig
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
void ColReg1_SaveConfig(void) 
{
    ColReg1_backup.controlState = ColReg1_Control;
}


/*******************************************************************************
* Function Name: ColReg1_RestoreConfig
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
void ColReg1_RestoreConfig(void) 
{
     ColReg1_Control = ColReg1_backup.controlState;
}


/*******************************************************************************
* Function Name: ColReg1_Sleep
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
void ColReg1_Sleep(void) 
{
    ColReg1_SaveConfig();
}


/*******************************************************************************
* Function Name: ColReg1_Wakeup
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
void ColReg1_Wakeup(void)  
{
    ColReg1_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
