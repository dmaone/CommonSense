/*******************************************************************************
* File Name: ChannelReg_PM.c
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

#include "ChannelReg.h"

/* Check for removal by optimization */
#if !defined(ChannelReg_Sync_ctrl_reg__REMOVED)

static ChannelReg_BACKUP_STRUCT  ChannelReg_backup = {0u};

    
/*******************************************************************************
* Function Name: ChannelReg_SaveConfig
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
void ChannelReg_SaveConfig(void) 
{
    ChannelReg_backup.controlState = ChannelReg_Control;
}


/*******************************************************************************
* Function Name: ChannelReg_RestoreConfig
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
void ChannelReg_RestoreConfig(void) 
{
     ChannelReg_Control = ChannelReg_backup.controlState;
}


/*******************************************************************************
* Function Name: ChannelReg_Sleep
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
void ChannelReg_Sleep(void) 
{
    ChannelReg_SaveConfig();
}


/*******************************************************************************
* Function Name: ChannelReg_Wakeup
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
void ChannelReg_Wakeup(void)  
{
    ChannelReg_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
