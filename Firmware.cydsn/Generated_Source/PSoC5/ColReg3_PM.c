/*******************************************************************************
* File Name: ColReg3_PM.c
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

#include "ColReg3.h"

/* Check for removal by optimization */
#if !defined(ColReg3_Sync_ctrl_reg__REMOVED)

static ColReg3_BACKUP_STRUCT  ColReg3_backup = {0u};

    
/*******************************************************************************
* Function Name: ColReg3_SaveConfig
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
void ColReg3_SaveConfig(void) 
{
    ColReg3_backup.controlState = ColReg3_Control;
}


/*******************************************************************************
* Function Name: ColReg3_RestoreConfig
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
void ColReg3_RestoreConfig(void) 
{
     ColReg3_Control = ColReg3_backup.controlState;
}


/*******************************************************************************
* Function Name: ColReg3_Sleep
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
void ColReg3_Sleep(void) 
{
    ColReg3_SaveConfig();
}


/*******************************************************************************
* Function Name: ColReg3_Wakeup
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
void ColReg3_Wakeup(void)  
{
    ColReg3_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
