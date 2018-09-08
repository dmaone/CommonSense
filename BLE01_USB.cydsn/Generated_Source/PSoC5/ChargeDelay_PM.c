/*******************************************************************************
* File Name: ChargeDelay_PM.c
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

#include "ChargeDelay.h"

static ChargeDelay_backupStruct ChargeDelay_backup;


/*******************************************************************************
* Function Name: ChargeDelay_SaveConfig
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
*  ChargeDelay_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void ChargeDelay_SaveConfig(void) 
{

    #if(!ChargeDelay_UsingFixedFunction)
        #if(!ChargeDelay_PWMModeIsCenterAligned)
            ChargeDelay_backup.PWMPeriod = ChargeDelay_ReadPeriod();
        #endif /* (!ChargeDelay_PWMModeIsCenterAligned) */
        ChargeDelay_backup.PWMUdb = ChargeDelay_ReadCounter();
        #if (ChargeDelay_UseStatus)
            ChargeDelay_backup.InterruptMaskValue = ChargeDelay_STATUS_MASK;
        #endif /* (ChargeDelay_UseStatus) */

        #if(ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_256_CLOCKS || \
            ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_2_4_CLOCKS)
            ChargeDelay_backup.PWMdeadBandValue = ChargeDelay_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(ChargeDelay_KillModeMinTime)
             ChargeDelay_backup.PWMKillCounterPeriod = ChargeDelay_ReadKillTime();
        #endif /* (ChargeDelay_KillModeMinTime) */

        #if(ChargeDelay_UseControl)
            ChargeDelay_backup.PWMControlRegister = ChargeDelay_ReadControlRegister();
        #endif /* (ChargeDelay_UseControl) */
    #endif  /* (!ChargeDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: ChargeDelay_RestoreConfig
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
*  ChargeDelay_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void ChargeDelay_RestoreConfig(void) 
{
        #if(!ChargeDelay_UsingFixedFunction)
            #if(!ChargeDelay_PWMModeIsCenterAligned)
                ChargeDelay_WritePeriod(ChargeDelay_backup.PWMPeriod);
            #endif /* (!ChargeDelay_PWMModeIsCenterAligned) */

            ChargeDelay_WriteCounter(ChargeDelay_backup.PWMUdb);

            #if (ChargeDelay_UseStatus)
                ChargeDelay_STATUS_MASK = ChargeDelay_backup.InterruptMaskValue;
            #endif /* (ChargeDelay_UseStatus) */

            #if(ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_256_CLOCKS || \
                ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_2_4_CLOCKS)
                ChargeDelay_WriteDeadTime(ChargeDelay_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(ChargeDelay_KillModeMinTime)
                ChargeDelay_WriteKillTime(ChargeDelay_backup.PWMKillCounterPeriod);
            #endif /* (ChargeDelay_KillModeMinTime) */

            #if(ChargeDelay_UseControl)
                ChargeDelay_WriteControlRegister(ChargeDelay_backup.PWMControlRegister);
            #endif /* (ChargeDelay_UseControl) */
        #endif  /* (!ChargeDelay_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: ChargeDelay_Sleep
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
*  ChargeDelay_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void ChargeDelay_Sleep(void) 
{
    #if(ChargeDelay_UseControl)
        if(ChargeDelay_CTRL_ENABLE == (ChargeDelay_CONTROL & ChargeDelay_CTRL_ENABLE))
        {
            /*Component is enabled */
            ChargeDelay_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            ChargeDelay_backup.PWMEnableState = 0u;
        }
    #endif /* (ChargeDelay_UseControl) */

    /* Stop component */
    ChargeDelay_Stop();

    /* Save registers configuration */
    ChargeDelay_SaveConfig();
}


/*******************************************************************************
* Function Name: ChargeDelay_Wakeup
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
*  ChargeDelay_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void ChargeDelay_Wakeup(void) 
{
     /* Restore registers values */
    ChargeDelay_RestoreConfig();

    if(ChargeDelay_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        ChargeDelay_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
