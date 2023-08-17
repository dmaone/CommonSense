/*******************************************************************************
* File Name: PS2_CLK_En_PM.c
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

#include "PS2_CLK_En.h"

/* Check for removal by optimization */
#if !defined(PS2_CLK_En_Sync_ctrl_reg__REMOVED)

static PS2_CLK_En_BACKUP_STRUCT  PS2_CLK_En_backup = {0u};

    
/*******************************************************************************
* Function Name: PS2_CLK_En_SaveConfig
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
void PS2_CLK_En_SaveConfig(void) 
{
    PS2_CLK_En_backup.controlState = PS2_CLK_En_Control;
}


/*******************************************************************************
* Function Name: PS2_CLK_En_RestoreConfig
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
void PS2_CLK_En_RestoreConfig(void) 
{
     PS2_CLK_En_Control = PS2_CLK_En_backup.controlState;
}


/*******************************************************************************
* Function Name: PS2_CLK_En_Sleep
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
void PS2_CLK_En_Sleep(void) 
{
    PS2_CLK_En_SaveConfig();
}


/*******************************************************************************
* Function Name: PS2_CLK_En_Wakeup
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
void PS2_CLK_En_Wakeup(void)  
{
    PS2_CLK_En_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
