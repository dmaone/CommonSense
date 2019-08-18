/*******************************************************************************
* File Name: PFN_PM.c
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

#include "PFN.h"

static PFN_backupStruct PFN_backup;


/*******************************************************************************
* Function Name: PFN_SaveConfig
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
*  PFN_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void PFN_SaveConfig(void) 
{

    #if(!PFN_UsingFixedFunction)
        #if(!PFN_PWMModeIsCenterAligned)
            PFN_backup.PWMPeriod = PFN_ReadPeriod();
        #endif /* (!PFN_PWMModeIsCenterAligned) */
        PFN_backup.PWMUdb = PFN_ReadCounter();
        #if (PFN_UseStatus)
            PFN_backup.InterruptMaskValue = PFN_STATUS_MASK;
        #endif /* (PFN_UseStatus) */

        #if(PFN_DeadBandMode == PFN__B_PWM__DBM_256_CLOCKS || \
            PFN_DeadBandMode == PFN__B_PWM__DBM_2_4_CLOCKS)
            PFN_backup.PWMdeadBandValue = PFN_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(PFN_KillModeMinTime)
             PFN_backup.PWMKillCounterPeriod = PFN_ReadKillTime();
        #endif /* (PFN_KillModeMinTime) */

        #if(PFN_UseControl)
            PFN_backup.PWMControlRegister = PFN_ReadControlRegister();
        #endif /* (PFN_UseControl) */
    #endif  /* (!PFN_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PFN_RestoreConfig
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
*  PFN_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void PFN_RestoreConfig(void) 
{
        #if(!PFN_UsingFixedFunction)
            #if(!PFN_PWMModeIsCenterAligned)
                PFN_WritePeriod(PFN_backup.PWMPeriod);
            #endif /* (!PFN_PWMModeIsCenterAligned) */

            PFN_WriteCounter(PFN_backup.PWMUdb);

            #if (PFN_UseStatus)
                PFN_STATUS_MASK = PFN_backup.InterruptMaskValue;
            #endif /* (PFN_UseStatus) */

            #if(PFN_DeadBandMode == PFN__B_PWM__DBM_256_CLOCKS || \
                PFN_DeadBandMode == PFN__B_PWM__DBM_2_4_CLOCKS)
                PFN_WriteDeadTime(PFN_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(PFN_KillModeMinTime)
                PFN_WriteKillTime(PFN_backup.PWMKillCounterPeriod);
            #endif /* (PFN_KillModeMinTime) */

            #if(PFN_UseControl)
                PFN_WriteControlRegister(PFN_backup.PWMControlRegister);
            #endif /* (PFN_UseControl) */
        #endif  /* (!PFN_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: PFN_Sleep
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
*  PFN_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void PFN_Sleep(void) 
{
    #if(PFN_UseControl)
        if(PFN_CTRL_ENABLE == (PFN_CONTROL & PFN_CTRL_ENABLE))
        {
            /*Component is enabled */
            PFN_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            PFN_backup.PWMEnableState = 0u;
        }
    #endif /* (PFN_UseControl) */

    /* Stop component */
    PFN_Stop();

    /* Save registers configuration */
    PFN_SaveConfig();
}


/*******************************************************************************
* Function Name: PFN_Wakeup
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
*  PFN_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void PFN_Wakeup(void) 
{
     /* Restore registers values */
    PFN_RestoreConfig();

    if(PFN_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        PFN_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
