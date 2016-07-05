/*******************************************************************************
* File Name: ColReg2_PM.c
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

#include "ColReg2.h"

/* Check for removal by optimization */
#if !defined(ColReg2_Sync_ctrl_reg__REMOVED)

static ColReg2_BACKUP_STRUCT  ColReg2_backup = {0u};

    
/*******************************************************************************
* Function Name: ColReg2_SaveConfig
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
void ColReg2_SaveConfig(void) 
{
    ColReg2_backup.controlState = ColReg2_Control;
}


/*******************************************************************************
* Function Name: ColReg2_RestoreConfig
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
void ColReg2_RestoreConfig(void) 
{
     ColReg2_Control = ColReg2_backup.controlState;
}


/*******************************************************************************
* Function Name: ColReg2_Sleep
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
void ColReg2_Sleep(void) 
{
    ColReg2_SaveConfig();
}


/*******************************************************************************
* Function Name: ColReg2_Wakeup
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
void ColReg2_Wakeup(void)  
{
    ColReg2_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
