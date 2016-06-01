/*******************************************************************************
* File Name: KeyboardTimer_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "KeyboardTimer.h"

static KeyboardTimer_backupStruct KeyboardTimer_backup;


/*******************************************************************************
* Function Name: KeyboardTimer_SaveConfig
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
*  KeyboardTimer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void KeyboardTimer_SaveConfig(void) 
{
    #if (!KeyboardTimer_UsingFixedFunction)
        KeyboardTimer_backup.TimerUdb = KeyboardTimer_ReadCounter();
        KeyboardTimer_backup.InterruptMaskValue = KeyboardTimer_STATUS_MASK;
        #if (KeyboardTimer_UsingHWCaptureCounter)
            KeyboardTimer_backup.TimerCaptureCounter = KeyboardTimer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!KeyboardTimer_UDB_CONTROL_REG_REMOVED)
            KeyboardTimer_backup.TimerControlRegister = KeyboardTimer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: KeyboardTimer_RestoreConfig
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
*  KeyboardTimer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void KeyboardTimer_RestoreConfig(void) 
{   
    #if (!KeyboardTimer_UsingFixedFunction)

        KeyboardTimer_WriteCounter(KeyboardTimer_backup.TimerUdb);
        KeyboardTimer_STATUS_MASK =KeyboardTimer_backup.InterruptMaskValue;
        #if (KeyboardTimer_UsingHWCaptureCounter)
            KeyboardTimer_SetCaptureCount(KeyboardTimer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!KeyboardTimer_UDB_CONTROL_REG_REMOVED)
            KeyboardTimer_WriteControlRegister(KeyboardTimer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: KeyboardTimer_Sleep
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
*  KeyboardTimer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void KeyboardTimer_Sleep(void) 
{
    #if(!KeyboardTimer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(KeyboardTimer_CTRL_ENABLE == (KeyboardTimer_CONTROL & KeyboardTimer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            KeyboardTimer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            KeyboardTimer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    KeyboardTimer_Stop();
    KeyboardTimer_SaveConfig();
}


/*******************************************************************************
* Function Name: KeyboardTimer_Wakeup
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
*  KeyboardTimer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void KeyboardTimer_Wakeup(void) 
{
    KeyboardTimer_RestoreConfig();
    #if(!KeyboardTimer_UDB_CONTROL_REG_REMOVED)
        if(KeyboardTimer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                KeyboardTimer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
