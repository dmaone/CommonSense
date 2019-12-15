/*******************************************************************************
* File Name: SysTimer_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "SysTimer.h"

static SysTimer_backupStruct SysTimer_backup;


/*******************************************************************************
* Function Name: SysTimer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  SysTimer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void SysTimer_SaveConfig(void) 
{
    #if (!SysTimer_UsingFixedFunction)
        SysTimer_backup.TimerUdb = SysTimer_ReadCounter();
        SysTimer_backup.InterruptMaskValue = SysTimer_STATUS_MASK;
        #if (SysTimer_UsingHWCaptureCounter)
            SysTimer_backup.TimerCaptureCounter = SysTimer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!SysTimer_UDB_CONTROL_REG_REMOVED)
            SysTimer_backup.TimerControlRegister = SysTimer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: SysTimer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  SysTimer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void SysTimer_RestoreConfig(void) 
{   
    #if (!SysTimer_UsingFixedFunction)

        SysTimer_WriteCounter(SysTimer_backup.TimerUdb);
        SysTimer_STATUS_MASK =SysTimer_backup.InterruptMaskValue;
        #if (SysTimer_UsingHWCaptureCounter)
            SysTimer_SetCaptureCount(SysTimer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!SysTimer_UDB_CONTROL_REG_REMOVED)
            SysTimer_WriteControlRegister(SysTimer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: SysTimer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  SysTimer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void SysTimer_Sleep(void) 
{
    #if(!SysTimer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(SysTimer_CTRL_ENABLE == (SysTimer_CONTROL & SysTimer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            SysTimer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            SysTimer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    SysTimer_Stop();
    SysTimer_SaveConfig();
}


/*******************************************************************************
* Function Name: SysTimer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  SysTimer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void SysTimer_Wakeup(void) 
{
    SysTimer_RestoreConfig();
    #if(!SysTimer_UDB_CONTROL_REG_REMOVED)
        if(SysTimer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                SysTimer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
