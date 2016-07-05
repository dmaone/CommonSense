/*******************************************************************************
* File Name: RampPWM_PM.c
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

#include "RampPWM.h"

static RampPWM_backupStruct RampPWM_backup;


/*******************************************************************************
* Function Name: RampPWM_SaveConfig
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
*  RampPWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void RampPWM_SaveConfig(void) 
{

    #if(!RampPWM_UsingFixedFunction)
        #if(!RampPWM_PWMModeIsCenterAligned)
            RampPWM_backup.PWMPeriod = RampPWM_ReadPeriod();
        #endif /* (!RampPWM_PWMModeIsCenterAligned) */
        RampPWM_backup.PWMUdb = RampPWM_ReadCounter();
        #if (RampPWM_UseStatus)
            RampPWM_backup.InterruptMaskValue = RampPWM_STATUS_MASK;
        #endif /* (RampPWM_UseStatus) */

        #if(RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_256_CLOCKS || \
            RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_2_4_CLOCKS)
            RampPWM_backup.PWMdeadBandValue = RampPWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(RampPWM_KillModeMinTime)
             RampPWM_backup.PWMKillCounterPeriod = RampPWM_ReadKillTime();
        #endif /* (RampPWM_KillModeMinTime) */

        #if(RampPWM_UseControl)
            RampPWM_backup.PWMControlRegister = RampPWM_ReadControlRegister();
        #endif /* (RampPWM_UseControl) */
    #endif  /* (!RampPWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: RampPWM_RestoreConfig
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
*  RampPWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void RampPWM_RestoreConfig(void) 
{
        #if(!RampPWM_UsingFixedFunction)
            #if(!RampPWM_PWMModeIsCenterAligned)
                RampPWM_WritePeriod(RampPWM_backup.PWMPeriod);
            #endif /* (!RampPWM_PWMModeIsCenterAligned) */

            RampPWM_WriteCounter(RampPWM_backup.PWMUdb);

            #if (RampPWM_UseStatus)
                RampPWM_STATUS_MASK = RampPWM_backup.InterruptMaskValue;
            #endif /* (RampPWM_UseStatus) */

            #if(RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_256_CLOCKS || \
                RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_2_4_CLOCKS)
                RampPWM_WriteDeadTime(RampPWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(RampPWM_KillModeMinTime)
                RampPWM_WriteKillTime(RampPWM_backup.PWMKillCounterPeriod);
            #endif /* (RampPWM_KillModeMinTime) */

            #if(RampPWM_UseControl)
                RampPWM_WriteControlRegister(RampPWM_backup.PWMControlRegister);
            #endif /* (RampPWM_UseControl) */
        #endif  /* (!RampPWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: RampPWM_Sleep
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
*  RampPWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void RampPWM_Sleep(void) 
{
    #if(RampPWM_UseControl)
        if(RampPWM_CTRL_ENABLE == (RampPWM_CONTROL & RampPWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            RampPWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            RampPWM_backup.PWMEnableState = 0u;
        }
    #endif /* (RampPWM_UseControl) */

    /* Stop component */
    RampPWM_Stop();

    /* Save registers configuration */
    RampPWM_SaveConfig();
}


/*******************************************************************************
* Function Name: RampPWM_Wakeup
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
*  RampPWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void RampPWM_Wakeup(void) 
{
     /* Restore registers values */
    RampPWM_RestoreConfig();

    if(RampPWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        RampPWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
