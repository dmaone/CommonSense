/*******************************************************************************
* File Name: DischargeDelay.c
* Version 3.30
*
* Description:
*  The PWM User Module consist of an 8 or 16-bit counter with two 8 or 16-bit
*  comparitors. Each instance of this user module is capable of generating
*  two PWM outputs with the same period. The pulse width is selectable between
*  1 and 255/65535. The period is selectable between 2 and 255/65536 clocks.
*  The compare value output may be configured to be active when the present
*  counter is less than or less than/equal to the compare value.
*  A terminal count output is also provided. It generates a pulse one clock
*  width wide when the counter is equal to zero.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "DischargeDelay.h"

/* Error message for removed <resource> through optimization */
#ifdef DischargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* DischargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 DischargeDelay_initVar = 0u;


/*******************************************************************************
* Function Name: DischargeDelay_Start
********************************************************************************
*
* Summary:
*  The start function initializes the pwm with the default values, the
*  enables the counter to begin counting.  It does not enable interrupts,
*  the EnableInt command should be called if interrupt generation is required.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  DischargeDelay_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void DischargeDelay_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(DischargeDelay_initVar == 0u)
    {
        DischargeDelay_Init();
        DischargeDelay_initVar = 1u;
    }
    DischargeDelay_Enable();

}


/*******************************************************************************
* Function Name: DischargeDelay_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  DischargeDelay_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void DischargeDelay_Init(void) 
{
    #if (DischargeDelay_UsingFixedFunction || DischargeDelay_UseControl)
        uint8 ctrl;
    #endif /* (DischargeDelay_UsingFixedFunction || DischargeDelay_UseControl) */

    #if(!DischargeDelay_UsingFixedFunction)
        #if(DischargeDelay_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 DischargeDelay_interruptState;
        #endif /* (DischargeDelay_UseStatus) */
    #endif /* (!DischargeDelay_UsingFixedFunction) */

    #if (DischargeDelay_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        DischargeDelay_CONTROL |= DischargeDelay_CFG0_MODE;
        #if (DischargeDelay_DeadBand2_4)
            DischargeDelay_CONTROL |= DischargeDelay_CFG0_DB;
        #endif /* (DischargeDelay_DeadBand2_4) */

        ctrl = DischargeDelay_CONTROL3 & ((uint8 )(~DischargeDelay_CTRL_CMPMODE1_MASK));
        DischargeDelay_CONTROL3 = ctrl | DischargeDelay_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        DischargeDelay_RT1 &= ((uint8)(~DischargeDelay_RT1_MASK));
        DischargeDelay_RT1 |= DischargeDelay_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        DischargeDelay_RT1 &= ((uint8)(~DischargeDelay_SYNCDSI_MASK));
        DischargeDelay_RT1 |= DischargeDelay_SYNCDSI_EN;

    #elif (DischargeDelay_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = DischargeDelay_CONTROL & ((uint8)(~DischargeDelay_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~DischargeDelay_CTRL_CMPMODE1_MASK));
        DischargeDelay_CONTROL = ctrl | DischargeDelay_DEFAULT_COMPARE2_MODE |
                                   DischargeDelay_DEFAULT_COMPARE1_MODE;
    #endif /* (DischargeDelay_UsingFixedFunction) */

    #if (!DischargeDelay_UsingFixedFunction)
        #if (DischargeDelay_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            DischargeDelay_AUX_CONTROLDP0 |= (DischargeDelay_AUX_CTRL_FIFO0_CLR);
        #else /* (DischargeDelay_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            DischargeDelay_AUX_CONTROLDP0 |= (DischargeDelay_AUX_CTRL_FIFO0_CLR);
            DischargeDelay_AUX_CONTROLDP1 |= (DischargeDelay_AUX_CTRL_FIFO0_CLR);
        #endif /* (DischargeDelay_Resolution == 8) */

        DischargeDelay_WriteCounter(DischargeDelay_INIT_PERIOD_VALUE);
    #endif /* (!DischargeDelay_UsingFixedFunction) */

    DischargeDelay_WritePeriod(DischargeDelay_INIT_PERIOD_VALUE);

        #if (DischargeDelay_UseOneCompareMode)
            DischargeDelay_WriteCompare(DischargeDelay_INIT_COMPARE_VALUE1);
        #else
            DischargeDelay_WriteCompare1(DischargeDelay_INIT_COMPARE_VALUE1);
            DischargeDelay_WriteCompare2(DischargeDelay_INIT_COMPARE_VALUE2);
        #endif /* (DischargeDelay_UseOneCompareMode) */

        #if (DischargeDelay_KillModeMinTime)
            DischargeDelay_WriteKillTime(DischargeDelay_MinimumKillTime);
        #endif /* (DischargeDelay_KillModeMinTime) */

        #if (DischargeDelay_DeadBandUsed)
            DischargeDelay_WriteDeadTime(DischargeDelay_INIT_DEAD_TIME);
        #endif /* (DischargeDelay_DeadBandUsed) */

    #if (DischargeDelay_UseStatus || DischargeDelay_UsingFixedFunction)
        DischargeDelay_SetInterruptMode(DischargeDelay_INIT_INTERRUPTS_MODE);
    #endif /* (DischargeDelay_UseStatus || DischargeDelay_UsingFixedFunction) */

    #if (DischargeDelay_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        DischargeDelay_GLOBAL_ENABLE |= DischargeDelay_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        DischargeDelay_CONTROL2 |= DischargeDelay_CTRL2_IRQ_SEL;
    #else
        #if(DischargeDelay_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            DischargeDelay_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            DischargeDelay_STATUS_AUX_CTRL |= DischargeDelay_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(DischargeDelay_interruptState);

            /* Clear the FIFO to enable the DischargeDelay_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            DischargeDelay_ClearFIFO();
        #endif /* (DischargeDelay_UseStatus) */
    #endif /* (DischargeDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: DischargeDelay_Enable
********************************************************************************
*
* Summary:
*  Enables the PWM block operation
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This works only if software enable mode is chosen
*
*******************************************************************************/
void DischargeDelay_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (DischargeDelay_UsingFixedFunction)
        DischargeDelay_GLOBAL_ENABLE |= DischargeDelay_BLOCK_EN_MASK;
        DischargeDelay_GLOBAL_STBY_ENABLE |= DischargeDelay_BLOCK_STBY_EN_MASK;
    #endif /* (DischargeDelay_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (DischargeDelay_UseControl || DischargeDelay_UsingFixedFunction)
        DischargeDelay_CONTROL |= DischargeDelay_CTRL_ENABLE;
    #endif /* (DischargeDelay_UseControl || DischargeDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: DischargeDelay_Stop
********************************************************************************
*
* Summary:
*  The stop function halts the PWM, but does not change any modes or disable
*  interrupts.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  If the Enable mode is set to Hardware only then this function
*  has no effect on the operation of the PWM
*
*******************************************************************************/
void DischargeDelay_Stop(void) 
{
    #if (DischargeDelay_UseControl || DischargeDelay_UsingFixedFunction)
        DischargeDelay_CONTROL &= ((uint8)(~DischargeDelay_CTRL_ENABLE));
    #endif /* (DischargeDelay_UseControl || DischargeDelay_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (DischargeDelay_UsingFixedFunction)
        DischargeDelay_GLOBAL_ENABLE &= ((uint8)(~DischargeDelay_BLOCK_EN_MASK));
        DischargeDelay_GLOBAL_STBY_ENABLE &= ((uint8)(~DischargeDelay_BLOCK_STBY_EN_MASK));
    #endif /* (DischargeDelay_UsingFixedFunction) */
}

#if (DischargeDelay_UseOneCompareMode)
    #if (DischargeDelay_CompareMode1SW)


        /*******************************************************************************
        * Function Name: DischargeDelay_SetCompareMode
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm output when in Dither mode,
        *  Center Align Mode or One Output Mode.
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void DischargeDelay_SetCompareMode(uint8 comparemode) 
        {
            #if(DischargeDelay_UsingFixedFunction)

                #if(0 != DischargeDelay_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << DischargeDelay_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != DischargeDelay_CTRL_CMPMODE1_SHIFT) */

                DischargeDelay_CONTROL3 &= ((uint8)(~DischargeDelay_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                DischargeDelay_CONTROL3 |= comparemodemasked;

            #elif (DischargeDelay_UseControl)

                #if(0 != DischargeDelay_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << DischargeDelay_CTRL_CMPMODE1_SHIFT)) &
                                                DischargeDelay_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & DischargeDelay_CTRL_CMPMODE1_MASK;
                #endif /* (0 != DischargeDelay_CTRL_CMPMODE1_SHIFT) */

                #if(0 != DischargeDelay_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << DischargeDelay_CTRL_CMPMODE2_SHIFT)) &
                                               DischargeDelay_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & DischargeDelay_CTRL_CMPMODE2_MASK;
                #endif /* (0 != DischargeDelay_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                DischargeDelay_CONTROL &= ((uint8)(~(DischargeDelay_CTRL_CMPMODE1_MASK |
                                            DischargeDelay_CTRL_CMPMODE2_MASK)));
                DischargeDelay_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (DischargeDelay_UsingFixedFunction) */
        }
    #endif /* DischargeDelay_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (DischargeDelay_CompareMode1SW)


        /*******************************************************************************
        * Function Name: DischargeDelay_SetCompareMode1
        ********************************************************************************
        *
        * Summary:
        *  This function writes the Compare Mode for the pwm or pwm1 output
        *
        * Parameters:
        *  comparemode:  The new compare mode for the PWM output. Use the compare types
        *                defined in the H file as input arguments.
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void DischargeDelay_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != DischargeDelay_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << DischargeDelay_CTRL_CMPMODE1_SHIFT)) &
                                           DischargeDelay_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & DischargeDelay_CTRL_CMPMODE1_MASK;
            #endif /* (0 != DischargeDelay_CTRL_CMPMODE1_SHIFT) */

            #if (DischargeDelay_UseControl)
                DischargeDelay_CONTROL &= ((uint8)(~DischargeDelay_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                DischargeDelay_CONTROL |= comparemodemasked;
            #endif /* (DischargeDelay_UseControl) */
        }
    #endif /* DischargeDelay_CompareMode1SW */

#if (DischargeDelay_CompareMode2SW)


    /*******************************************************************************
    * Function Name: DischargeDelay_SetCompareMode2
    ********************************************************************************
    *
    * Summary:
    *  This function writes the Compare Mode for the pwm or pwm2 output
    *
    * Parameters:
    *  comparemode:  The new compare mode for the PWM output. Use the compare types
    *                defined in the H file as input arguments.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void DischargeDelay_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != DischargeDelay_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << DischargeDelay_CTRL_CMPMODE2_SHIFT)) &
                                                 DischargeDelay_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & DischargeDelay_CTRL_CMPMODE2_MASK;
        #endif /* (0 != DischargeDelay_CTRL_CMPMODE2_SHIFT) */

        #if (DischargeDelay_UseControl)
            DischargeDelay_CONTROL &= ((uint8)(~DischargeDelay_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            DischargeDelay_CONTROL |= comparemodemasked;
        #endif /* (DischargeDelay_UseControl) */
    }
    #endif /*DischargeDelay_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!DischargeDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: DischargeDelay_WriteCounter
    ********************************************************************************
    *
    * Summary:
    *  Writes a new counter value directly to the counter register. This will be
    *  implemented for that currently running period and only that period. This API
    *  is valid only for UDB implementation and not available for fixed function
    *  PWM implementation.
    *
    * Parameters:
    *  counter:  The period new period counter value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  The PWM Period will be reloaded when a counter value will be a zero
    *
    *******************************************************************************/
    void DischargeDelay_WriteCounter(uint8 counter) \
                                       
    {
        CY_SET_REG8(DischargeDelay_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadCounter
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current value of the counter.  It doesn't matter
    *  if the counter is enabled or running.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  The current value of the counter.
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(DischargeDelay_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG8(DischargeDelay_CAPTURE_LSB_PTR));
    }

    #if (DischargeDelay_UseStatus)


        /*******************************************************************************
        * Function Name: DischargeDelay_ClearFIFO
        ********************************************************************************
        *
        * Summary:
        *  This function clears all capture data from the capture FIFO
        *
        * Parameters:
        *  None
        *
        * Return:
        *  None
        *
        *******************************************************************************/
        void DischargeDelay_ClearFIFO(void) 
        {
            while(0u != (DischargeDelay_ReadStatusRegister() & DischargeDelay_STATUS_FIFONEMPTY))
            {
                (void)DischargeDelay_ReadCapture();
            }
        }

    #endif /* DischargeDelay_UseStatus */

#endif /* !DischargeDelay_UsingFixedFunction */


/*******************************************************************************
* Function Name: DischargeDelay_WritePeriod
********************************************************************************
*
* Summary:
*  This function is used to change the period of the counter.  The new period
*  will be loaded the next time terminal count is detected.
*
* Parameters:
*  period:  Period value. May be between 1 and (2^Resolution)-1.  A value of 0
*           will result in the counter remaining at zero.
*
* Return:
*  None
*
*******************************************************************************/
void DischargeDelay_WritePeriod(uint8 period) 
{
    #if(DischargeDelay_UsingFixedFunction)
        CY_SET_REG16(DischargeDelay_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG8(DischargeDelay_PERIOD_LSB_PTR, period);
    #endif /* (DischargeDelay_UsingFixedFunction) */
}

#if (DischargeDelay_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: DischargeDelay_WriteCompare
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value when the PWM is in Dither
    *  mode. The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  compared to the compare value based on the compare mode defined in
    *  Dither Mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    * Side Effects:
    *  This function is only available if the PWM mode parameter is set to
    *  Dither Mode, Center Aligned Mode or One Output Mode
    *
    *******************************************************************************/
    void DischargeDelay_WriteCompare(uint8 compare) \
                                       
    {
        #if(DischargeDelay_UsingFixedFunction)
            CY_SET_REG16(DischargeDelay_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(DischargeDelay_COMPARE1_LSB_PTR, compare);
        #endif /* (DischargeDelay_UsingFixedFunction) */

        #if (DischargeDelay_PWMMode == DischargeDelay__B_PWM__DITHER)
            #if(DischargeDelay_UsingFixedFunction)
                CY_SET_REG16(DischargeDelay_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG8(DischargeDelay_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (DischargeDelay_UsingFixedFunction) */
        #endif /* (DischargeDelay_PWMMode == DischargeDelay__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: DischargeDelay_WriteCompare1
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare1 value.  The compare output will
    *  reflect the new value on the next UDB clock.  The compare output will be
    *  driven high when the present counter value is less than or less than or
    *  equal to the compare register, depending on the mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void DischargeDelay_WriteCompare1(uint8 compare) \
                                        
    {
        #if(DischargeDelay_UsingFixedFunction)
            CY_SET_REG16(DischargeDelay_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(DischargeDelay_COMPARE1_LSB_PTR, compare);
        #endif /* (DischargeDelay_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: DischargeDelay_WriteCompare2
    ********************************************************************************
    *
    * Summary:
    *  This funtion is used to change the compare value, for compare1 output.
    *  The compare output will reflect the new value on the next UDB clock.
    *  The compare output will be driven high when the present counter value is
    *  less than or less than or equal to the compare register, depending on the
    *  mode.
    *
    * Parameters:
    *  compare:  New compare value.
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void DischargeDelay_WriteCompare2(uint8 compare) \
                                        
    {
        #if(DischargeDelay_UsingFixedFunction)
            CY_SET_REG16(DischargeDelay_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG8(DischargeDelay_COMPARE2_LSB_PTR, compare);
        #endif /* (DischargeDelay_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (DischargeDelay_DeadBandUsed)


    /*******************************************************************************
    * Function Name: DischargeDelay_WriteDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function writes the dead-band counts to the corresponding register
    *
    * Parameters:
    *  deadtime:  Number of counts for dead time
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void DischargeDelay_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!DischargeDelay_DeadBand2_4)
            CY_SET_REG8(DischargeDelay_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            DischargeDelay_DEADBAND_COUNT &= ((uint8)(~DischargeDelay_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(DischargeDelay_DEADBAND_COUNT_SHIFT)
                DischargeDelay_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << DischargeDelay_DEADBAND_COUNT_SHIFT)) &
                                                    DischargeDelay_DEADBAND_COUNT_MASK;
            #else
                DischargeDelay_DEADBAND_COUNT |= deadtime & DischargeDelay_DEADBAND_COUNT_MASK;
            #endif /* (DischargeDelay_DEADBAND_COUNT_SHIFT) */

        #endif /* (!DischargeDelay_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadDeadTime
    ********************************************************************************
    *
    * Summary:
    *  This function reads the dead-band counts from the corresponding register
    *
    * Parameters:
    *  None
    *
    * Return:
    *  Dead Band Counts
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!DischargeDelay_DeadBand2_4)
            return (CY_GET_REG8(DischargeDelay_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(DischargeDelay_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(DischargeDelay_DEADBAND_COUNT & DischargeDelay_DEADBAND_COUNT_MASK)) >>
                                                                           DischargeDelay_DEADBAND_COUNT_SHIFT));
            #else
                return (DischargeDelay_DEADBAND_COUNT & DischargeDelay_DEADBAND_COUNT_MASK);
            #endif /* (DischargeDelay_DEADBAND_COUNT_SHIFT) */
        #endif /* (!DischargeDelay_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (DischargeDelay_UseStatus || DischargeDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: DischargeDelay_SetInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  This function configures the interrupts mask control of theinterrupt
    *  source status register.
    *
    * Parameters:
    *  uint8 interruptMode: Bit field containing the interrupt sources enabled
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void DischargeDelay_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(DischargeDelay_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadStatusRegister
    ********************************************************************************
    *
    * Summary:
    *  This function returns the current state of the status register.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current status register value. The status register bits are:
    *  [7:6] : Unused(0)
    *  [5]   : Kill event output
    *  [4]   : FIFO not empty
    *  [3]   : FIFO full
    *  [2]   : Terminal count
    *  [1]   : Compare output 2
    *  [0]   : Compare output 1
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(DischargeDelay_STATUS_PTR));
    }

#endif /* (DischargeDelay_UseStatus || DischargeDelay_UsingFixedFunction) */


#if (DischargeDelay_UseControl)


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the control register. This API is available
    *  only if the control register is not removed.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8 : Current control register value
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(DischargeDelay_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: DischargeDelay_WriteControlRegister
    ********************************************************************************
    *
    * Summary:
    *  Sets the bit field of the control register. This API is available only if
    *  the control register is not removed.
    *
    * Parameters:
    *  uint8 control: Control register bit field, The status register bits are:
    *  [7]   : PWM Enable
    *  [6]   : Reset
    *  [5:3] : Compare Mode2
    *  [2:0] : Compare Mode2
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void DischargeDelay_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(DischargeDelay_CONTROL_PTR, control);
    }

#endif /* (DischargeDelay_UseControl) */


#if (!DischargeDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadCapture
    ********************************************************************************
    *
    * Summary:
    *  Reads the capture value from the capture FIFO.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: The current capture value
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadCapture(void) 
    {
        return (CY_GET_REG8(DischargeDelay_CAPTURE_LSB_PTR));
    }

#endif /* (!DischargeDelay_UsingFixedFunction) */


#if (DischargeDelay_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadCompare
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare output when the PWM Mode parameter is
    *  set to Dither mode, Center Aligned mode, or One Output mode.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadCompare(void) 
    {
        #if(DischargeDelay_UsingFixedFunction)
            return ((uint8)CY_GET_REG16(DischargeDelay_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG8(DischargeDelay_COMPARE1_LSB_PTR));
        #endif /* (DischargeDelay_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadCompare1
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare1 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadCompare1(void) 
    {
        return (CY_GET_REG8(DischargeDelay_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadCompare2
    ********************************************************************************
    *
    * Summary:
    *  Reads the compare value for the compare2 output.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8/uint16: Current compare value.
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadCompare2(void) 
    {
        return (CY_GET_REG8(DischargeDelay_COMPARE2_LSB_PTR));
    }

#endif /* (DischargeDelay_UseOneCompareMode) */


/*******************************************************************************
* Function Name: DischargeDelay_ReadPeriod
********************************************************************************
*
* Summary:
*  Reads the period value used by the PWM hardware.
*
* Parameters:
*  None
*
* Return:
*  uint8/16: Period value
*
*******************************************************************************/
uint8 DischargeDelay_ReadPeriod(void) 
{
    #if(DischargeDelay_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(DischargeDelay_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(DischargeDelay_PERIOD_LSB_PTR));
    #endif /* (DischargeDelay_UsingFixedFunction) */
}

#if ( DischargeDelay_KillModeMinTime)


    /*******************************************************************************
    * Function Name: DischargeDelay_WriteKillTime
    ********************************************************************************
    *
    * Summary:
    *  Writes the kill time value used by the hardware when the Kill Mode
    *  is set to Minimum Time.
    *
    * Parameters:
    *  uint8: Minimum Time kill counts
    *
    * Return:
    *  None
    *
    *******************************************************************************/
    void DischargeDelay_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(DischargeDelay_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: DischargeDelay_ReadKillTime
    ********************************************************************************
    *
    * Summary:
    *  Reads the kill time value used by the hardware when the Kill Mode is set
    *  to Minimum Time.
    *
    * Parameters:
    *  None
    *
    * Return:
    *  uint8: The current Minimum Time kill counts
    *
    *******************************************************************************/
    uint8 DischargeDelay_ReadKillTime(void) 
    {
        return (CY_GET_REG8(DischargeDelay_KILLMODEMINTIME_PTR));
    }

#endif /* ( DischargeDelay_KillModeMinTime) */

/* [] END OF FILE */
