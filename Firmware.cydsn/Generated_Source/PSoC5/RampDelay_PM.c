/*******************************************************************************
* File Name: RampDelay_PM.c
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

#include "RampDelay.h"

static RampDelay_backupStruct RampDelay_backup;


/*******************************************************************************
* Function Name: RampDelay_SaveConfig
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
*  RampDelay_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void RampDelay_SaveConfig(void) 
{

    #if(!RampDelay_UsingFixedFunction)
        #if(!RampDelay_PWMModeIsCenterAligned)
            RampDelay_backup.PWMPeriod = RampDelay_ReadPeriod();
        #endif /* (!RampDelay_PWMModeIsCenterAligned) */
        RampDelay_backup.PWMUdb = RampDelay_ReadCounter();
        #if (RampDelay_UseStatus)
            RampDelay_backup.InterruptMaskValue = RampDelay_STATUS_MASK;
        #endif /* (RampDelay_UseStatus) */

        #if(RampDelay_DeadBandMode == RampDelay__B_PWM__DBM_256_CLOCKS || \
            RampDelay_DeadBandMode == RampDelay__B_PWM__DBM_2_4_CLOCKS)
            RampDelay_backup.PWMdeadBandValue = RampDelay_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(RampDelay_KillModeMinTime)
             RampDelay_backup.PWMKillCounterPeriod = RampDelay_ReadKillTime();
        #endif /* (RampDelay_KillModeMinTime) */

        #if(RampDelay_UseControl)
            RampDelay_backup.PWMControlRegister = RampDelay_ReadControlRegister();
        #endif /* (RampDelay_UseControl) */
    #endif  /* (!RampDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: RampDelay_RestoreConfig
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
*  RampDelay_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void RampDelay_RestoreConfig(void) 
{
        #if(!RampDelay_UsingFixedFunction)
            #if(!RampDelay_PWMModeIsCenterAligned)
                RampDelay_WritePeriod(RampDelay_backup.PWMPeriod);
            #endif /* (!RampDelay_PWMModeIsCenterAligned) */

            RampDelay_WriteCounter(RampDelay_backup.PWMUdb);

            #if (RampDelay_UseStatus)
                RampDelay_STATUS_MASK = RampDelay_backup.InterruptMaskValue;
            #endif /* (RampDelay_UseStatus) */

            #if(RampDelay_DeadBandMode == RampDelay__B_PWM__DBM_256_CLOCKS || \
                RampDelay_DeadBandMode == RampDelay__B_PWM__DBM_2_4_CLOCKS)
                RampDelay_WriteDeadTime(RampDelay_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(RampDelay_KillModeMinTime)
                RampDelay_WriteKillTime(RampDelay_backup.PWMKillCounterPeriod);
            #endif /* (RampDelay_KillModeMinTime) */

            #if(RampDelay_UseControl)
                RampDelay_WriteControlRegister(RampDelay_backup.PWMControlRegister);
            #endif /* (RampDelay_UseControl) */
        #endif  /* (!RampDelay_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: RampDelay_Sleep
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
*  RampDelay_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void RampDelay_Sleep(void) 
{
    #if(RampDelay_UseControl)
        if(RampDelay_CTRL_ENABLE == (RampDelay_CONTROL & RampDelay_CTRL_ENABLE))
        {
            /*Component is enabled */
            RampDelay_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            RampDelay_backup.PWMEnableState = 0u;
        }
    #endif /* (RampDelay_UseControl) */

    /* Stop component */
    RampDelay_Stop();

    /* Save registers configuration */
    RampDelay_SaveConfig();
}


/*******************************************************************************
* Function Name: RampDelay_Wakeup
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
*  RampDelay_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void RampDelay_Wakeup(void) 
{
     /* Restore registers values */
    RampDelay_RestoreConfig();

    if(RampDelay_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        RampDelay_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
