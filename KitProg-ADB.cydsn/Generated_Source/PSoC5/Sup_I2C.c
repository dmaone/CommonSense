/*******************************************************************************
* File Name: Sup_I2C.c
* Version 3.50
*
* Description:
*  This file provides the source code of APIs for the I2C component.
*  The actual protocol and operation code resides in the interrupt service
*  routine file.
*
*******************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Sup_I2C_PVT.h"


/**********************************
*      System variables
**********************************/

uint8 Sup_I2C_initVar = 0u; /* Defines if component was initialized */

volatile uint8 Sup_I2C_state;  /* Current state of I2C FSM */


/*******************************************************************************
* Function Name: Sup_I2C_Init
********************************************************************************
*
* Summary:
*  Initializes I2C registers with initial values provided from customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sup_I2C_Init(void) 
{
#if (Sup_I2C_FF_IMPLEMENTED)
    /* Configure fixed function block */
    Sup_I2C_CFG_REG  = Sup_I2C_DEFAULT_CFG;
    Sup_I2C_XCFG_REG = Sup_I2C_DEFAULT_XCFG;
    Sup_I2C_ADDR_REG = Sup_I2C_DEFAULT_ADDR;
    Sup_I2C_CLKDIV1_REG = LO8(Sup_I2C_DEFAULT_DIVIDE_FACTOR);
    Sup_I2C_CLKDIV2_REG = HI8(Sup_I2C_DEFAULT_DIVIDE_FACTOR);

#else
    uint8 intState;

    /* Configure control and interrupt sources */
    Sup_I2C_CFG_REG      = Sup_I2C_DEFAULT_CFG;
    Sup_I2C_INT_MASK_REG = Sup_I2C_DEFAULT_INT_MASK;

    /* Enable interrupt generation in status */
    intState = CyEnterCriticalSection();
    Sup_I2C_INT_ENABLE_REG |= Sup_I2C_INTR_ENABLE;
    CyExitCriticalSection(intState);

    /* Configure bit counter */
    #if (Sup_I2C_MODE_SLAVE_ENABLED)
        Sup_I2C_PERIOD_REG = Sup_I2C_DEFAULT_PERIOD;
    #endif  /* (Sup_I2C_MODE_SLAVE_ENABLED) */

    /* Configure clock generator */
    #if (Sup_I2C_MODE_MASTER_ENABLED)
        Sup_I2C_MCLK_PRD_REG = Sup_I2C_DEFAULT_MCLK_PRD;
        Sup_I2C_MCLK_CMP_REG = Sup_I2C_DEFAULT_MCLK_CMP;
    #endif /* (Sup_I2C_MODE_MASTER_ENABLED) */
#endif /* (Sup_I2C_FF_IMPLEMENTED) */

#if (Sup_I2C_TIMEOUT_ENABLED)
    Sup_I2C_TimeoutInit();
#endif /* (Sup_I2C_TIMEOUT_ENABLED) */

    /* Configure internal interrupt */
    CyIntDisable    (Sup_I2C_ISR_NUMBER);
    CyIntSetPriority(Sup_I2C_ISR_NUMBER, Sup_I2C_ISR_PRIORITY);
    #if (Sup_I2C_INTERN_I2C_INTR_HANDLER)
        (void) CyIntSetVector(Sup_I2C_ISR_NUMBER, &Sup_I2C_ISR);
    #endif /* (Sup_I2C_INTERN_I2C_INTR_HANDLER) */

    /* Set FSM to default state */
    Sup_I2C_state = Sup_I2C_SM_IDLE;

#if (Sup_I2C_MODE_SLAVE_ENABLED)
    /* Clear status and buffers index */
    Sup_I2C_slStatus = 0u;
    Sup_I2C_slRdBufIndex = 0u;
    Sup_I2C_slWrBufIndex = 0u;

    /* Configure matched address */
    Sup_I2C_SlaveSetAddress(Sup_I2C_DEFAULT_ADDR);
#endif /* (Sup_I2C_MODE_SLAVE_ENABLED) */

#if (Sup_I2C_MODE_MASTER_ENABLED)
    /* Clear status and buffers index */
    Sup_I2C_mstrStatus = 0u;
    Sup_I2C_mstrRdBufIndex = 0u;
    Sup_I2C_mstrWrBufIndex = 0u;
#endif /* (Sup_I2C_MODE_MASTER_ENABLED) */
}


/*******************************************************************************
* Function Name: Sup_I2C_Enable
********************************************************************************
*
* Summary:
*  Enables I2C operations.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  None.
*
*******************************************************************************/
void Sup_I2C_Enable(void) 
{
#if (Sup_I2C_FF_IMPLEMENTED)
    uint8 intState;

    /* Enable power to block */
    intState = CyEnterCriticalSection();
    Sup_I2C_ACT_PWRMGR_REG  |= Sup_I2C_ACT_PWR_EN;
    Sup_I2C_STBY_PWRMGR_REG |= Sup_I2C_STBY_PWR_EN;
    CyExitCriticalSection(intState);
#else
    #if (Sup_I2C_MODE_SLAVE_ENABLED)
        /* Enable bit counter */
        uint8 intState = CyEnterCriticalSection();
        Sup_I2C_COUNTER_AUX_CTL_REG |= Sup_I2C_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    #endif /* (Sup_I2C_MODE_SLAVE_ENABLED) */

    /* Enable slave or master bits */
    Sup_I2C_CFG_REG |= Sup_I2C_ENABLE_MS;
#endif /* (Sup_I2C_FF_IMPLEMENTED) */

#if (Sup_I2C_TIMEOUT_ENABLED)
    Sup_I2C_TimeoutEnable();
#endif /* (Sup_I2C_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: Sup_I2C_Start
********************************************************************************
*
* Summary:
*  Starts the I2C hardware. Enables Active mode power template bits or clock
*  gating as appropriate. It is required to be executed before I2C bus
*  operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This component automatically enables its interrupt.  If I2C is enabled !
*  without the interrupt enabled, it can lock up the I2C bus.
*
* Global variables:
*  Sup_I2C_initVar - This variable is used to check the initial
*                             configuration, modified on the first
*                             function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Sup_I2C_Start(void) 
{
    if (0u == Sup_I2C_initVar)
    {
        Sup_I2C_Init();
        Sup_I2C_initVar = 1u; /* Component initialized */
    }

    Sup_I2C_Enable();
    Sup_I2C_EnableInt();
}


/*******************************************************************************
* Function Name: Sup_I2C_Stop
********************************************************************************
*
* Summary:
*  Disables I2C hardware and disables I2C interrupt. Disables Active mode power
*  template bits or clock gating as appropriate.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Sup_I2C_Stop(void) 
{
    Sup_I2C_DisableInt();

#if (Sup_I2C_TIMEOUT_ENABLED)
    Sup_I2C_TimeoutStop();
#endif  /* End (Sup_I2C_TIMEOUT_ENABLED) */

#if (Sup_I2C_FF_IMPLEMENTED)
    {
        uint8 intState;
        uint16 blockResetCycles;

        /* Store registers effected by block disable */
        Sup_I2C_backup.addr    = Sup_I2C_ADDR_REG;
        Sup_I2C_backup.clkDiv1 = Sup_I2C_CLKDIV1_REG;
        Sup_I2C_backup.clkDiv2 = Sup_I2C_CLKDIV2_REG;

        /* Calculate number of cycles to reset block */
        blockResetCycles = ((uint16) ((uint16) Sup_I2C_CLKDIV2_REG << 8u) | Sup_I2C_CLKDIV1_REG) + 1u;

        /* Disable block */
        Sup_I2C_CFG_REG &= (uint8) ~Sup_I2C_CFG_EN_SLAVE;
        /* Wait for block reset before disable power */
        CyDelayCycles((uint32) blockResetCycles);

        /* Disable power to block */
        intState = CyEnterCriticalSection();
        Sup_I2C_ACT_PWRMGR_REG  &= (uint8) ~Sup_I2C_ACT_PWR_EN;
        Sup_I2C_STBY_PWRMGR_REG &= (uint8) ~Sup_I2C_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable block */
        Sup_I2C_CFG_REG |= (uint8) Sup_I2C_ENABLE_MS;

        /* Restore registers effected by block disable. Ticket ID#198004 */
        Sup_I2C_ADDR_REG    = Sup_I2C_backup.addr;
        Sup_I2C_ADDR_REG    = Sup_I2C_backup.addr;
        Sup_I2C_CLKDIV1_REG = Sup_I2C_backup.clkDiv1;
        Sup_I2C_CLKDIV2_REG = Sup_I2C_backup.clkDiv2;
    }
#else

    /* Disable slave or master bits */
    Sup_I2C_CFG_REG &= (uint8) ~Sup_I2C_ENABLE_MS;

#if (Sup_I2C_MODE_SLAVE_ENABLED)
    {
        /* Disable bit counter */
        uint8 intState = CyEnterCriticalSection();
        Sup_I2C_COUNTER_AUX_CTL_REG &= (uint8) ~Sup_I2C_CNT7_ENABLE;
        CyExitCriticalSection(intState);
    }
#endif /* (Sup_I2C_MODE_SLAVE_ENABLED) */

    /* Clear interrupt source register */
    (void) Sup_I2C_CSR_REG;
#endif /* (Sup_I2C_FF_IMPLEMENTED) */

    /* Disable interrupt on stop (enabled by write transaction) */
    Sup_I2C_DISABLE_INT_ON_STOP;
    Sup_I2C_ClearPendingInt();

    /* Reset FSM to default state */
    Sup_I2C_state = Sup_I2C_SM_IDLE;

    /* Clear busy statuses */
#if (Sup_I2C_MODE_SLAVE_ENABLED)
    Sup_I2C_slStatus &= (uint8) ~(Sup_I2C_SSTAT_RD_BUSY | Sup_I2C_SSTAT_WR_BUSY);
#endif /* (Sup_I2C_MODE_SLAVE_ENABLED) */
}


/* [] END OF FILE */
