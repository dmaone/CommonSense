/*******************************************************************************
* File Name: SetupDelay_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SetupDelay.h"

static SetupDelay_backupStruct SetupDelay_backup;


/*******************************************************************************
* Function Name: SetupDelay_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  SetupDelay_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void SetupDelay_SaveConfig(void) 
{

    #if(!SetupDelay_UsingFixedFunction)
        #if(!SetupDelay_PWMModeIsCenterAligned)
            SetupDelay_backup.PWMPeriod = SetupDelay_ReadPeriod();
        #endif /* (!SetupDelay_PWMModeIsCenterAligned) */
        SetupDelay_backup.PWMUdb = SetupDelay_ReadCounter();
        #if (SetupDelay_UseStatus)
            SetupDelay_backup.InterruptMaskValue = SetupDelay_STATUS_MASK;
        #endif /* (SetupDelay_UseStatus) */

        #if(SetupDelay_DeadBandMode == SetupDelay__B_PWM__DBM_256_CLOCKS || \
            SetupDelay_DeadBandMode == SetupDelay__B_PWM__DBM_2_4_CLOCKS)
            SetupDelay_backup.PWMdeadBandValue = SetupDelay_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(SetupDelay_KillModeMinTime)
             SetupDelay_backup.PWMKillCounterPeriod = SetupDelay_ReadKillTime();
        #endif /* (SetupDelay_KillModeMinTime) */

        #if(SetupDelay_UseControl)
            SetupDelay_backup.PWMControlRegister = SetupDelay_ReadControlRegister();
        #endif /* (SetupDelay_UseControl) */
    #endif  /* (!SetupDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SetupDelay_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  SetupDelay_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void SetupDelay_RestoreConfig(void) 
{
        #if(!SetupDelay_UsingFixedFunction)
            #if(!SetupDelay_PWMModeIsCenterAligned)
                SetupDelay_WritePeriod(SetupDelay_backup.PWMPeriod);
            #endif /* (!SetupDelay_PWMModeIsCenterAligned) */

            SetupDelay_WriteCounter(SetupDelay_backup.PWMUdb);

            #if (SetupDelay_UseStatus)
                SetupDelay_STATUS_MASK = SetupDelay_backup.InterruptMaskValue;
            #endif /* (SetupDelay_UseStatus) */

            #if(SetupDelay_DeadBandMode == SetupDelay__B_PWM__DBM_256_CLOCKS || \
                SetupDelay_DeadBandMode == SetupDelay__B_PWM__DBM_2_4_CLOCKS)
                SetupDelay_WriteDeadTime(SetupDelay_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(SetupDelay_KillModeMinTime)
                SetupDelay_WriteKillTime(SetupDelay_backup.PWMKillCounterPeriod);
            #endif /* (SetupDelay_KillModeMinTime) */

            #if(SetupDelay_UseControl)
                SetupDelay_WriteControlRegister(SetupDelay_backup.PWMControlRegister);
            #endif /* (SetupDelay_UseControl) */
        #endif  /* (!SetupDelay_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: SetupDelay_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  SetupDelay_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void SetupDelay_Sleep(void) 
{
    #if(SetupDelay_UseControl)
        if(SetupDelay_CTRL_ENABLE == (SetupDelay_CONTROL & SetupDelay_CTRL_ENABLE))
        {
            /*Component is enabled */
            SetupDelay_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            SetupDelay_backup.PWMEnableState = 0u;
        }
    #endif /* (SetupDelay_UseControl) */

    /* Stop component */
    SetupDelay_Stop();

    /* Save registers configuration */
    SetupDelay_SaveConfig();
}


/*******************************************************************************
* Function Name: SetupDelay_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  SetupDelay_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void SetupDelay_Wakeup(void) 
{
     /* Restore registers values */
    SetupDelay_RestoreConfig();

    if(SetupDelay_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        SetupDelay_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
