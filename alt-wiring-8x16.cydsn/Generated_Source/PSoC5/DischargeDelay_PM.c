/*******************************************************************************
* File Name: DischargeDelay_PM.c
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

#include "DischargeDelay.h"

static DischargeDelay_backupStruct DischargeDelay_backup;


/*******************************************************************************
* Function Name: DischargeDelay_SaveConfig
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
*  DischargeDelay_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void DischargeDelay_SaveConfig(void) 
{

    #if(!DischargeDelay_UsingFixedFunction)
        #if(!DischargeDelay_PWMModeIsCenterAligned)
            DischargeDelay_backup.PWMPeriod = DischargeDelay_ReadPeriod();
        #endif /* (!DischargeDelay_PWMModeIsCenterAligned) */
        DischargeDelay_backup.PWMUdb = DischargeDelay_ReadCounter();
        #if (DischargeDelay_UseStatus)
            DischargeDelay_backup.InterruptMaskValue = DischargeDelay_STATUS_MASK;
        #endif /* (DischargeDelay_UseStatus) */

        #if(DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_256_CLOCKS || \
            DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_2_4_CLOCKS)
            DischargeDelay_backup.PWMdeadBandValue = DischargeDelay_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(DischargeDelay_KillModeMinTime)
             DischargeDelay_backup.PWMKillCounterPeriod = DischargeDelay_ReadKillTime();
        #endif /* (DischargeDelay_KillModeMinTime) */

        #if(DischargeDelay_UseControl)
            DischargeDelay_backup.PWMControlRegister = DischargeDelay_ReadControlRegister();
        #endif /* (DischargeDelay_UseControl) */
    #endif  /* (!DischargeDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: DischargeDelay_RestoreConfig
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
*  DischargeDelay_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void DischargeDelay_RestoreConfig(void) 
{
        #if(!DischargeDelay_UsingFixedFunction)
            #if(!DischargeDelay_PWMModeIsCenterAligned)
                DischargeDelay_WritePeriod(DischargeDelay_backup.PWMPeriod);
            #endif /* (!DischargeDelay_PWMModeIsCenterAligned) */

            DischargeDelay_WriteCounter(DischargeDelay_backup.PWMUdb);

            #if (DischargeDelay_UseStatus)
                DischargeDelay_STATUS_MASK = DischargeDelay_backup.InterruptMaskValue;
            #endif /* (DischargeDelay_UseStatus) */

            #if(DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_256_CLOCKS || \
                DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_2_4_CLOCKS)
                DischargeDelay_WriteDeadTime(DischargeDelay_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(DischargeDelay_KillModeMinTime)
                DischargeDelay_WriteKillTime(DischargeDelay_backup.PWMKillCounterPeriod);
            #endif /* (DischargeDelay_KillModeMinTime) */

            #if(DischargeDelay_UseControl)
                DischargeDelay_WriteControlRegister(DischargeDelay_backup.PWMControlRegister);
            #endif /* (DischargeDelay_UseControl) */
        #endif  /* (!DischargeDelay_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: DischargeDelay_Sleep
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
*  DischargeDelay_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void DischargeDelay_Sleep(void) 
{
    #if(DischargeDelay_UseControl)
        if(DischargeDelay_CTRL_ENABLE == (DischargeDelay_CONTROL & DischargeDelay_CTRL_ENABLE))
        {
            /*Component is enabled */
            DischargeDelay_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            DischargeDelay_backup.PWMEnableState = 0u;
        }
    #endif /* (DischargeDelay_UseControl) */

    /* Stop component */
    DischargeDelay_Stop();

    /* Save registers configuration */
    DischargeDelay_SaveConfig();
}


/*******************************************************************************
* Function Name: DischargeDelay_Wakeup
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
*  DischargeDelay_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void DischargeDelay_Wakeup(void) 
{
     /* Restore registers values */
    DischargeDelay_RestoreConfig();

    if(DischargeDelay_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        DischargeDelay_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
