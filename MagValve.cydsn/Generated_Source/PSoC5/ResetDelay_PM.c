/*******************************************************************************
* File Name: ResetDelay_PM.c
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

#include "ResetDelay.h"

static ResetDelay_backupStruct ResetDelay_backup;


/*******************************************************************************
* Function Name: ResetDelay_SaveConfig
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
*  ResetDelay_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void ResetDelay_SaveConfig(void) 
{

    #if(!ResetDelay_UsingFixedFunction)
        #if(!ResetDelay_PWMModeIsCenterAligned)
            ResetDelay_backup.PWMPeriod = ResetDelay_ReadPeriod();
        #endif /* (!ResetDelay_PWMModeIsCenterAligned) */
        ResetDelay_backup.PWMUdb = ResetDelay_ReadCounter();
        #if (ResetDelay_UseStatus)
            ResetDelay_backup.InterruptMaskValue = ResetDelay_STATUS_MASK;
        #endif /* (ResetDelay_UseStatus) */

        #if(ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_256_CLOCKS || \
            ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_2_4_CLOCKS)
            ResetDelay_backup.PWMdeadBandValue = ResetDelay_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(ResetDelay_KillModeMinTime)
             ResetDelay_backup.PWMKillCounterPeriod = ResetDelay_ReadKillTime();
        #endif /* (ResetDelay_KillModeMinTime) */

        #if(ResetDelay_UseControl)
            ResetDelay_backup.PWMControlRegister = ResetDelay_ReadControlRegister();
        #endif /* (ResetDelay_UseControl) */
    #endif  /* (!ResetDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: ResetDelay_RestoreConfig
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
*  ResetDelay_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void ResetDelay_RestoreConfig(void) 
{
        #if(!ResetDelay_UsingFixedFunction)
            #if(!ResetDelay_PWMModeIsCenterAligned)
                ResetDelay_WritePeriod(ResetDelay_backup.PWMPeriod);
            #endif /* (!ResetDelay_PWMModeIsCenterAligned) */

            ResetDelay_WriteCounter(ResetDelay_backup.PWMUdb);

            #if (ResetDelay_UseStatus)
                ResetDelay_STATUS_MASK = ResetDelay_backup.InterruptMaskValue;
            #endif /* (ResetDelay_UseStatus) */

            #if(ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_256_CLOCKS || \
                ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_2_4_CLOCKS)
                ResetDelay_WriteDeadTime(ResetDelay_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(ResetDelay_KillModeMinTime)
                ResetDelay_WriteKillTime(ResetDelay_backup.PWMKillCounterPeriod);
            #endif /* (ResetDelay_KillModeMinTime) */

            #if(ResetDelay_UseControl)
                ResetDelay_WriteControlRegister(ResetDelay_backup.PWMControlRegister);
            #endif /* (ResetDelay_UseControl) */
        #endif  /* (!ResetDelay_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: ResetDelay_Sleep
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
*  ResetDelay_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void ResetDelay_Sleep(void) 
{
    #if(ResetDelay_UseControl)
        if(ResetDelay_CTRL_ENABLE == (ResetDelay_CONTROL & ResetDelay_CTRL_ENABLE))
        {
            /*Component is enabled */
            ResetDelay_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            ResetDelay_backup.PWMEnableState = 0u;
        }
    #endif /* (ResetDelay_UseControl) */

    /* Stop component */
    ResetDelay_Stop();

    /* Save registers configuration */
    ResetDelay_SaveConfig();
}


/*******************************************************************************
* Function Name: ResetDelay_Wakeup
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
*  ResetDelay_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void ResetDelay_Wakeup(void) 
{
     /* Restore registers values */
    ResetDelay_RestoreConfig();

    if(ResetDelay_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        ResetDelay_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
