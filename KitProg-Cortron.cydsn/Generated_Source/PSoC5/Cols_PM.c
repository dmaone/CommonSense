/*******************************************************************************
* File Name: Cols_PM.c
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

#include "Cols.h"

/* Check for removal by optimization */
#if !defined(Cols_Sync_ctrl_reg__REMOVED)

static Cols_BACKUP_STRUCT  Cols_backup = {0u};

    
/*******************************************************************************
* Function Name: Cols_SaveConfig
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
void Cols_SaveConfig(void) 
{
    Cols_backup.controlState = Cols_Control;
}


/*******************************************************************************
* Function Name: Cols_RestoreConfig
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
void Cols_RestoreConfig(void) 
{
     Cols_Control = Cols_backup.controlState;
}


/*******************************************************************************
* Function Name: Cols_Sleep
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
void Cols_Sleep(void) 
{
    Cols_SaveConfig();
}


/*******************************************************************************
* Function Name: Cols_Wakeup
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
void Cols_Wakeup(void)  
{
    Cols_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
