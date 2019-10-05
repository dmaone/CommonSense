/*******************************************************************************
* File Name: Rows_PM.c
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

#include "Rows.h"

/* Check for removal by optimization */
#if !defined(Rows_Sync_ctrl_reg__REMOVED)

static Rows_BACKUP_STRUCT  Rows_backup = {0u};

    
/*******************************************************************************
* Function Name: Rows_SaveConfig
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
void Rows_SaveConfig(void) 
{
    Rows_backup.controlState = Rows_Control;
}


/*******************************************************************************
* Function Name: Rows_RestoreConfig
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
void Rows_RestoreConfig(void) 
{
     Rows_Control = Rows_backup.controlState;
}


/*******************************************************************************
* Function Name: Rows_Sleep
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
void Rows_Sleep(void) 
{
    Rows_SaveConfig();
}


/*******************************************************************************
* Function Name: Rows_Wakeup
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
void Rows_Wakeup(void)  
{
    Rows_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
