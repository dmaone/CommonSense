/*******************************************************************************
* File Name: R0_PM.c
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

#include "R0.h"

/* Check for removal by optimization */
#if !defined(R0_Sync_ctrl_reg__REMOVED)

static R0_BACKUP_STRUCT  R0_backup = {0u};

    
/*******************************************************************************
* Function Name: R0_SaveConfig
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
void R0_SaveConfig(void) 
{
    R0_backup.controlState = R0_Control;
}


/*******************************************************************************
* Function Name: R0_RestoreConfig
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
void R0_RestoreConfig(void) 
{
     R0_Control = R0_backup.controlState;
}


/*******************************************************************************
* Function Name: R0_Sleep
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
void R0_Sleep(void) 
{
    R0_SaveConfig();
}


/*******************************************************************************
* Function Name: R0_Wakeup
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
void R0_Wakeup(void)  
{
    R0_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
