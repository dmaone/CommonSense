/*******************************************************************************
* File Name: SensorReg_PM.c
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

#include "SensorReg.h"

/* Check for removal by optimization */
#if !defined(SensorReg_Sync_ctrl_reg__REMOVED)

static SensorReg_BACKUP_STRUCT  SensorReg_backup = {0u};

    
/*******************************************************************************
* Function Name: SensorReg_SaveConfig
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
void SensorReg_SaveConfig(void) 
{
    SensorReg_backup.controlState = SensorReg_Control;
}


/*******************************************************************************
* Function Name: SensorReg_RestoreConfig
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
void SensorReg_RestoreConfig(void) 
{
     SensorReg_Control = SensorReg_backup.controlState;
}


/*******************************************************************************
* Function Name: SensorReg_Sleep
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
void SensorReg_Sleep(void) 
{
    SensorReg_SaveConfig();
}


/*******************************************************************************
* Function Name: SensorReg_Wakeup
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
void SensorReg_Wakeup(void)  
{
    SensorReg_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
