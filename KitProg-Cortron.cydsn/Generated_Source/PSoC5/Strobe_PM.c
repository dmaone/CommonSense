/*******************************************************************************
* File Name: Strobe_PM.c
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

#include "Strobe.h"

/* Check for removal by optimization */
#if !defined(Strobe_Sync_ctrl_reg__REMOVED)

static Strobe_BACKUP_STRUCT  Strobe_backup = {0u};

    
/*******************************************************************************
* Function Name: Strobe_SaveConfig
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
void Strobe_SaveConfig(void) 
{
    Strobe_backup.controlState = Strobe_Control;
}


/*******************************************************************************
* Function Name: Strobe_RestoreConfig
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
void Strobe_RestoreConfig(void) 
{
     Strobe_Control = Strobe_backup.controlState;
}


/*******************************************************************************
* Function Name: Strobe_Sleep
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
void Strobe_Sleep(void) 
{
    Strobe_SaveConfig();
}


/*******************************************************************************
* Function Name: Strobe_Wakeup
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
void Strobe_Wakeup(void)  
{
    Strobe_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
