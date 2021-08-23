/*******************************************************************************
* File Name: Sup_I2C_PM.c
* Version 3.50
*
* Description:
*  This file provides low power mode APIs for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Sup_I2C_PVT.h"

Sup_I2C_BACKUP_STRUCT Sup_I2C_backup =
{
    Sup_I2C_DISABLE,

#if (Sup_I2C_FF_IMPLEMENTED)
    Sup_I2C_DEFAULT_XCFG,
    Sup_I2C_DEFAULT_CFG,
    Sup_I2C_DEFAULT_ADDR,
    LO8(Sup_I2C_DEFAULT_DIVIDE_FACTOR),
    HI8(Sup_I2C_DEFAULT_DIVIDE_FACTOR),
#else  /* (Sup_I2C_UDB_IMPLEMENTED) */
    Sup_I2C_DEFAULT_CFG,
#endif /* (Sup_I2C_FF_IMPLEMENTED) */

#if (Sup_I2C_TIMEOUT_ENABLED)
    Sup_I2C_DEFAULT_TMOUT_PERIOD,
    Sup_I2C_DEFAULT_TMOUT_INTR_MASK,
#endif /* (Sup_I2C_TIMEOUT_ENABLED) */
};

#if ((Sup_I2C_FF_IMPLEMENTED) && (Sup_I2C_WAKEUP_ENABLED))
    volatile uint8 Sup_I2C_wakeupSource;
#endif /* ((Sup_I2C_FF_IMPLEMENTED) && (Sup_I2C_WAKEUP_ENABLED)) */


/*******************************************************************************
* Function Name: Sup_I2C_SaveConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Stores the component non-retention configuration registers.
*   Checked:   Disables the master, if it was enabled before, and enables
*              backup regulator of the I2C hardware. If a transaction intended
*              for component executes during this function call, it waits until
*              the current transaction is completed and I2C hardware is ready
*              to enter sleep mode. All subsequent I2C traffic is NAKed until
*              the device is put into sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Sup_I2C_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            entering the sleep mode.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sup_I2C_SaveConfig(void) 
{
#if (Sup_I2C_FF_IMPLEMENTED)
    #if (Sup_I2C_WAKEUP_ENABLED)
        uint8 intState;
    #endif /* (Sup_I2C_WAKEUP_ENABLED) */

    /* Store registers before enter low power mode */
    Sup_I2C_backup.cfg     = Sup_I2C_CFG_REG;
    Sup_I2C_backup.xcfg    = Sup_I2C_XCFG_REG;
    Sup_I2C_backup.addr    = Sup_I2C_ADDR_REG;
    Sup_I2C_backup.clkDiv1 = Sup_I2C_CLKDIV1_REG;
    Sup_I2C_backup.clkDiv2 = Sup_I2C_CLKDIV2_REG;

#if (Sup_I2C_WAKEUP_ENABLED)
    /* Disable master */
    Sup_I2C_CFG_REG &= (uint8) ~Sup_I2C_ENABLE_MASTER;

    /* Enable backup regulator to keep block powered in low power mode */
    intState = CyEnterCriticalSection();
    Sup_I2C_PWRSYS_CR1_REG |= Sup_I2C_PWRSYS_CR1_I2C_REG_BACKUP;
    CyExitCriticalSection(intState);

    /* 1) Set force NACK to ignore I2C transactions;
    *  2) Wait unti I2C is ready go to Sleep; !!
    *  3) These bits are cleared on wake up.
    */
    /* Wait when block is ready for sleep */
    Sup_I2C_XCFG_REG |= Sup_I2C_XCFG_FORCE_NACK;
    while (0u == (Sup_I2C_XCFG_REG & Sup_I2C_XCFG_RDY_TO_SLEEP))
    {
    }

    /* Setup wakeup interrupt */
    Sup_I2C_DisableInt();
    (void) CyIntSetVector(Sup_I2C_ISR_NUMBER, &Sup_I2C_WAKEUP_ISR);
    Sup_I2C_wakeupSource = 0u;
    Sup_I2C_EnableInt();
#endif /* (Sup_I2C_WAKEUP_ENABLED) */

#else
    /* Store only address match bit */
    Sup_I2C_backup.control = (Sup_I2C_CFG_REG & Sup_I2C_CTRL_ANY_ADDRESS_MASK);
#endif /* (Sup_I2C_FF_IMPLEMENTED) */

#if (Sup_I2C_TIMEOUT_ENABLED)
    Sup_I2C_TimeoutSaveConfig();
#endif /* (Sup_I2C_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: Sup_I2C_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component before device enters
*  sleep mode. The Enable wakeup from Sleep Mode selection influences this
*  function implementation:
*   Unchecked: Checks current I2C component state, saves it, and disables the
*              component by calling I2C_Stop() if it is currently enabled.
*              I2C_SaveConfig() is then called to save the component
*              non-retention configuration registers.
*   Checked:   If a transaction intended for component executes during this
*              function call, it waits until the current transaction is
*              completed. All subsequent I2C traffic intended for component
*              is NAKed until the device is put to sleep mode. The address
*              match event wakes up the device.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sup_I2C_Sleep(void) 
{
#if (Sup_I2C_WAKEUP_ENABLED)
    /* Do not enable block after exit low power mode if it is wakeup source */
    Sup_I2C_backup.enableState = Sup_I2C_DISABLE;

    #if (Sup_I2C_TIMEOUT_ENABLED)
        Sup_I2C_TimeoutStop();
    #endif /* (Sup_I2C_TIMEOUT_ENABLED) */

#else
    /* Store enable state */
    Sup_I2C_backup.enableState = (uint8) Sup_I2C_IS_ENABLED;

    if (0u != Sup_I2C_backup.enableState)
    {
        Sup_I2C_Stop();
    }
#endif /* (Sup_I2C_WAKEUP_ENABLED) */

    Sup_I2C_SaveConfig();
}


/*******************************************************************************
* Function Name: Sup_I2C_RestoreConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              to the state they were in before I2C_Sleep() or I2C_SaveConfig()
*              was called.
*   Checked:   Disables the backup regulator of the I2C hardware. Sets up the
*              regular component interrupt handler and generates the component
*              interrupt if it was wake up source to release the bus and
*              continue in-coming I2C transaction.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Sup_I2C_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            exiting the sleep mode.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before Sup_I2C_SaveConfig() or
*  Sup_I2C_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void Sup_I2C_RestoreConfig(void) 
{
#if (Sup_I2C_FF_IMPLEMENTED)
    uint8 intState;

    if (Sup_I2C_CHECK_PWRSYS_I2C_BACKUP)
    /* Low power mode was Sleep - backup regulator is enabled */
    {
        /* Enable backup regulator in active mode */
        intState = CyEnterCriticalSection();
        Sup_I2C_PWRSYS_CR1_REG &= (uint8) ~Sup_I2C_PWRSYS_CR1_I2C_REG_BACKUP;
        CyExitCriticalSection(intState);

        /* Restore master */
        Sup_I2C_CFG_REG = Sup_I2C_backup.cfg;
    }
    else
    /* Low power mode was Hibernate - backup regulator is disabled. All registers are cleared */
    {
    #if (Sup_I2C_WAKEUP_ENABLED)
        /* Disable power to block before register restore */
        intState = CyEnterCriticalSection();
        Sup_I2C_ACT_PWRMGR_REG  &= (uint8) ~Sup_I2C_ACT_PWR_EN;
        Sup_I2C_STBY_PWRMGR_REG &= (uint8) ~Sup_I2C_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable component in I2C_Wakeup() after register restore */
        Sup_I2C_backup.enableState = Sup_I2C_ENABLE;
    #endif /* (Sup_I2C_WAKEUP_ENABLED) */

        /* Restore component registers after Hibernate */
        Sup_I2C_XCFG_REG    = Sup_I2C_backup.xcfg;
        Sup_I2C_CFG_REG     = Sup_I2C_backup.cfg;
        Sup_I2C_ADDR_REG    = Sup_I2C_backup.addr;
        Sup_I2C_CLKDIV1_REG = Sup_I2C_backup.clkDiv1;
        Sup_I2C_CLKDIV2_REG = Sup_I2C_backup.clkDiv2;
    }

#if (Sup_I2C_WAKEUP_ENABLED)
    Sup_I2C_DisableInt();
    (void) CyIntSetVector(Sup_I2C_ISR_NUMBER, &Sup_I2C_ISR);
    if (0u != Sup_I2C_wakeupSource)
    {
        /* Generate interrupt to process incoming transaction */
        Sup_I2C_SetPendingInt();
    }
    Sup_I2C_EnableInt();
#endif /* (Sup_I2C_WAKEUP_ENABLED) */

#else
    Sup_I2C_CFG_REG = Sup_I2C_backup.control;
#endif /* (Sup_I2C_FF_IMPLEMENTED) */

#if (Sup_I2C_TIMEOUT_ENABLED)
    Sup_I2C_TimeoutRestoreConfig();
#endif /* (Sup_I2C_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: Sup_I2C_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component for active mode
*  operation (when device exits sleep mode). The Enable wakeup from Sleep Mode
*  selection influences this function implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              by calling I2C_RestoreConfig(). If the component was enabled
*              before the I2C_Sleep() function was called, I2C_Wakeup()
*              re-enables it.
*   Checked:   Enables  master functionality if it was enabled before sleep,
*              and disables the backup regulator of the I2C hardware.
*              The incoming transaction continues as soon as the regular
*              I2C interrupt handler is set up (global interrupts has to be
*              enabled to service I2C component interrupt).
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before Sup_I2C_SaveConfig() or
*  Sup_I2C_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void Sup_I2C_Wakeup(void) 
{
    Sup_I2C_RestoreConfig();

    /* Restore component enable state */
    if (0u != Sup_I2C_backup.enableState)
    {
        Sup_I2C_Enable();
        Sup_I2C_EnableInt();
    }
    else
    {
    #if (Sup_I2C_TIMEOUT_ENABLED)
        Sup_I2C_TimeoutEnable();
    #endif /* (Sup_I2C_TIMEOUT_ENABLED) */
    }
}


/* [] END OF FILE */
