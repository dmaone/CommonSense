/*******************************************************************************
* File Name: ChargeDelay.c
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

#include "ChargeDelay.h"

/* Error message for removed <resource> through optimization */
#ifdef ChargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* ChargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 ChargeDelay_initVar = 0u;


/*******************************************************************************
* Function Name: ChargeDelay_Start
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
*  ChargeDelay_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void ChargeDelay_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(ChargeDelay_initVar == 0u)
    {
        ChargeDelay_Init();
        ChargeDelay_initVar = 1u;
    }
    ChargeDelay_Enable();

}


/*******************************************************************************
* Function Name: ChargeDelay_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  ChargeDelay_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ChargeDelay_Init(void) 
{
    #if (ChargeDelay_UsingFixedFunction || ChargeDelay_UseControl)
        uint8 ctrl;
    #endif /* (ChargeDelay_UsingFixedFunction || ChargeDelay_UseControl) */

    #if(!ChargeDelay_UsingFixedFunction)
        #if(ChargeDelay_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 ChargeDelay_interruptState;
        #endif /* (ChargeDelay_UseStatus) */
    #endif /* (!ChargeDelay_UsingFixedFunction) */

    #if (ChargeDelay_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        ChargeDelay_CONTROL |= ChargeDelay_CFG0_MODE;
        #if (ChargeDelay_DeadBand2_4)
            ChargeDelay_CONTROL |= ChargeDelay_CFG0_DB;
        #endif /* (ChargeDelay_DeadBand2_4) */

        ctrl = ChargeDelay_CONTROL3 & ((uint8 )(~ChargeDelay_CTRL_CMPMODE1_MASK));
        ChargeDelay_CONTROL3 = ctrl | ChargeDelay_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        ChargeDelay_RT1 &= ((uint8)(~ChargeDelay_RT1_MASK));
        ChargeDelay_RT1 |= ChargeDelay_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        ChargeDelay_RT1 &= ((uint8)(~ChargeDelay_SYNCDSI_MASK));
        ChargeDelay_RT1 |= ChargeDelay_SYNCDSI_EN;

    #elif (ChargeDelay_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = ChargeDelay_CONTROL & ((uint8)(~ChargeDelay_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~ChargeDelay_CTRL_CMPMODE1_MASK));
        ChargeDelay_CONTROL = ctrl | ChargeDelay_DEFAULT_COMPARE2_MODE |
                                   ChargeDelay_DEFAULT_COMPARE1_MODE;
    #endif /* (ChargeDelay_UsingFixedFunction) */

    #if (!ChargeDelay_UsingFixedFunction)
        #if (ChargeDelay_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            ChargeDelay_AUX_CONTROLDP0 |= (ChargeDelay_AUX_CTRL_FIFO0_CLR);
        #else /* (ChargeDelay_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            ChargeDelay_AUX_CONTROLDP0 |= (ChargeDelay_AUX_CTRL_FIFO0_CLR);
            ChargeDelay_AUX_CONTROLDP1 |= (ChargeDelay_AUX_CTRL_FIFO0_CLR);
        #endif /* (ChargeDelay_Resolution == 8) */

        ChargeDelay_WriteCounter(ChargeDelay_INIT_PERIOD_VALUE);
    #endif /* (!ChargeDelay_UsingFixedFunction) */

    ChargeDelay_WritePeriod(ChargeDelay_INIT_PERIOD_VALUE);

        #if (ChargeDelay_UseOneCompareMode)
            ChargeDelay_WriteCompare(ChargeDelay_INIT_COMPARE_VALUE1);
        #else
            ChargeDelay_WriteCompare1(ChargeDelay_INIT_COMPARE_VALUE1);
            ChargeDelay_WriteCompare2(ChargeDelay_INIT_COMPARE_VALUE2);
        #endif /* (ChargeDelay_UseOneCompareMode) */

        #if (ChargeDelay_KillModeMinTime)
            ChargeDelay_WriteKillTime(ChargeDelay_MinimumKillTime);
        #endif /* (ChargeDelay_KillModeMinTime) */

        #if (ChargeDelay_DeadBandUsed)
            ChargeDelay_WriteDeadTime(ChargeDelay_INIT_DEAD_TIME);
        #endif /* (ChargeDelay_DeadBandUsed) */

    #if (ChargeDelay_UseStatus || ChargeDelay_UsingFixedFunction)
        ChargeDelay_SetInterruptMode(ChargeDelay_INIT_INTERRUPTS_MODE);
    #endif /* (ChargeDelay_UseStatus || ChargeDelay_UsingFixedFunction) */

    #if (ChargeDelay_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        ChargeDelay_GLOBAL_ENABLE |= ChargeDelay_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        ChargeDelay_CONTROL2 |= ChargeDelay_CTRL2_IRQ_SEL;
    #else
        #if(ChargeDelay_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            ChargeDelay_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            ChargeDelay_STATUS_AUX_CTRL |= ChargeDelay_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(ChargeDelay_interruptState);

            /* Clear the FIFO to enable the ChargeDelay_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            ChargeDelay_ClearFIFO();
        #endif /* (ChargeDelay_UseStatus) */
    #endif /* (ChargeDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: ChargeDelay_Enable
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
void ChargeDelay_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (ChargeDelay_UsingFixedFunction)
        ChargeDelay_GLOBAL_ENABLE |= ChargeDelay_BLOCK_EN_MASK;
        ChargeDelay_GLOBAL_STBY_ENABLE |= ChargeDelay_BLOCK_STBY_EN_MASK;
    #endif /* (ChargeDelay_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (ChargeDelay_UseControl || ChargeDelay_UsingFixedFunction)
        ChargeDelay_CONTROL |= ChargeDelay_CTRL_ENABLE;
    #endif /* (ChargeDelay_UseControl || ChargeDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: ChargeDelay_Stop
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
void ChargeDelay_Stop(void) 
{
    #if (ChargeDelay_UseControl || ChargeDelay_UsingFixedFunction)
        ChargeDelay_CONTROL &= ((uint8)(~ChargeDelay_CTRL_ENABLE));
    #endif /* (ChargeDelay_UseControl || ChargeDelay_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (ChargeDelay_UsingFixedFunction)
        ChargeDelay_GLOBAL_ENABLE &= ((uint8)(~ChargeDelay_BLOCK_EN_MASK));
        ChargeDelay_GLOBAL_STBY_ENABLE &= ((uint8)(~ChargeDelay_BLOCK_STBY_EN_MASK));
    #endif /* (ChargeDelay_UsingFixedFunction) */
}

#if (ChargeDelay_UseOneCompareMode)
    #if (ChargeDelay_CompareMode1SW)


        /*******************************************************************************
        * Function Name: ChargeDelay_SetCompareMode
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
        void ChargeDelay_SetCompareMode(uint8 comparemode) 
        {
            #if(ChargeDelay_UsingFixedFunction)

                #if(0 != ChargeDelay_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << ChargeDelay_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != ChargeDelay_CTRL_CMPMODE1_SHIFT) */

                ChargeDelay_CONTROL3 &= ((uint8)(~ChargeDelay_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                ChargeDelay_CONTROL3 |= comparemodemasked;

            #elif (ChargeDelay_UseControl)

                #if(0 != ChargeDelay_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << ChargeDelay_CTRL_CMPMODE1_SHIFT)) &
                                                ChargeDelay_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & ChargeDelay_CTRL_CMPMODE1_MASK;
                #endif /* (0 != ChargeDelay_CTRL_CMPMODE1_SHIFT) */

                #if(0 != ChargeDelay_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << ChargeDelay_CTRL_CMPMODE2_SHIFT)) &
                                               ChargeDelay_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & ChargeDelay_CTRL_CMPMODE2_MASK;
                #endif /* (0 != ChargeDelay_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                ChargeDelay_CONTROL &= ((uint8)(~(ChargeDelay_CTRL_CMPMODE1_MASK |
                                            ChargeDelay_CTRL_CMPMODE2_MASK)));
                ChargeDelay_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (ChargeDelay_UsingFixedFunction) */
        }
    #endif /* ChargeDelay_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (ChargeDelay_CompareMode1SW)


        /*******************************************************************************
        * Function Name: ChargeDelay_SetCompareMode1
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
        void ChargeDelay_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != ChargeDelay_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << ChargeDelay_CTRL_CMPMODE1_SHIFT)) &
                                           ChargeDelay_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & ChargeDelay_CTRL_CMPMODE1_MASK;
            #endif /* (0 != ChargeDelay_CTRL_CMPMODE1_SHIFT) */

            #if (ChargeDelay_UseControl)
                ChargeDelay_CONTROL &= ((uint8)(~ChargeDelay_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                ChargeDelay_CONTROL |= comparemodemasked;
            #endif /* (ChargeDelay_UseControl) */
        }
    #endif /* ChargeDelay_CompareMode1SW */

#if (ChargeDelay_CompareMode2SW)


    /*******************************************************************************
    * Function Name: ChargeDelay_SetCompareMode2
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
    void ChargeDelay_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != ChargeDelay_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << ChargeDelay_CTRL_CMPMODE2_SHIFT)) &
                                                 ChargeDelay_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & ChargeDelay_CTRL_CMPMODE2_MASK;
        #endif /* (0 != ChargeDelay_CTRL_CMPMODE2_SHIFT) */

        #if (ChargeDelay_UseControl)
            ChargeDelay_CONTROL &= ((uint8)(~ChargeDelay_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            ChargeDelay_CONTROL |= comparemodemasked;
        #endif /* (ChargeDelay_UseControl) */
    }
    #endif /*ChargeDelay_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!ChargeDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: ChargeDelay_WriteCounter
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
    void ChargeDelay_WriteCounter(uint8 counter) \
                                       
    {
        CY_SET_REG8(ChargeDelay_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadCounter
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
    uint8 ChargeDelay_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(ChargeDelay_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG8(ChargeDelay_CAPTURE_LSB_PTR));
    }

    #if (ChargeDelay_UseStatus)


        /*******************************************************************************
        * Function Name: ChargeDelay_ClearFIFO
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
        void ChargeDelay_ClearFIFO(void) 
        {
            while(0u != (ChargeDelay_ReadStatusRegister() & ChargeDelay_STATUS_FIFONEMPTY))
            {
                (void)ChargeDelay_ReadCapture();
            }
        }

    #endif /* ChargeDelay_UseStatus */

#endif /* !ChargeDelay_UsingFixedFunction */


/*******************************************************************************
* Function Name: ChargeDelay_WritePeriod
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
void ChargeDelay_WritePeriod(uint8 period) 
{
    #if(ChargeDelay_UsingFixedFunction)
        CY_SET_REG16(ChargeDelay_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG8(ChargeDelay_PERIOD_LSB_PTR, period);
    #endif /* (ChargeDelay_UsingFixedFunction) */
}

#if (ChargeDelay_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: ChargeDelay_WriteCompare
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
    void ChargeDelay_WriteCompare(uint8 compare) \
                                       
    {
        #if(ChargeDelay_UsingFixedFunction)
            CY_SET_REG16(ChargeDelay_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(ChargeDelay_COMPARE1_LSB_PTR, compare);
        #endif /* (ChargeDelay_UsingFixedFunction) */

        #if (ChargeDelay_PWMMode == ChargeDelay__B_PWM__DITHER)
            #if(ChargeDelay_UsingFixedFunction)
                CY_SET_REG16(ChargeDelay_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG8(ChargeDelay_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (ChargeDelay_UsingFixedFunction) */
        #endif /* (ChargeDelay_PWMMode == ChargeDelay__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: ChargeDelay_WriteCompare1
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
    void ChargeDelay_WriteCompare1(uint8 compare) \
                                        
    {
        #if(ChargeDelay_UsingFixedFunction)
            CY_SET_REG16(ChargeDelay_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(ChargeDelay_COMPARE1_LSB_PTR, compare);
        #endif /* (ChargeDelay_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: ChargeDelay_WriteCompare2
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
    void ChargeDelay_WriteCompare2(uint8 compare) \
                                        
    {
        #if(ChargeDelay_UsingFixedFunction)
            CY_SET_REG16(ChargeDelay_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG8(ChargeDelay_COMPARE2_LSB_PTR, compare);
        #endif /* (ChargeDelay_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (ChargeDelay_DeadBandUsed)


    /*******************************************************************************
    * Function Name: ChargeDelay_WriteDeadTime
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
    void ChargeDelay_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!ChargeDelay_DeadBand2_4)
            CY_SET_REG8(ChargeDelay_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            ChargeDelay_DEADBAND_COUNT &= ((uint8)(~ChargeDelay_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(ChargeDelay_DEADBAND_COUNT_SHIFT)
                ChargeDelay_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << ChargeDelay_DEADBAND_COUNT_SHIFT)) &
                                                    ChargeDelay_DEADBAND_COUNT_MASK;
            #else
                ChargeDelay_DEADBAND_COUNT |= deadtime & ChargeDelay_DEADBAND_COUNT_MASK;
            #endif /* (ChargeDelay_DEADBAND_COUNT_SHIFT) */

        #endif /* (!ChargeDelay_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadDeadTime
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
    uint8 ChargeDelay_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!ChargeDelay_DeadBand2_4)
            return (CY_GET_REG8(ChargeDelay_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(ChargeDelay_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(ChargeDelay_DEADBAND_COUNT & ChargeDelay_DEADBAND_COUNT_MASK)) >>
                                                                           ChargeDelay_DEADBAND_COUNT_SHIFT));
            #else
                return (ChargeDelay_DEADBAND_COUNT & ChargeDelay_DEADBAND_COUNT_MASK);
            #endif /* (ChargeDelay_DEADBAND_COUNT_SHIFT) */
        #endif /* (!ChargeDelay_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (ChargeDelay_UseStatus || ChargeDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: ChargeDelay_SetInterruptMode
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
    void ChargeDelay_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(ChargeDelay_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadStatusRegister
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
    uint8 ChargeDelay_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(ChargeDelay_STATUS_PTR));
    }

#endif /* (ChargeDelay_UseStatus || ChargeDelay_UsingFixedFunction) */


#if (ChargeDelay_UseControl)


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadControlRegister
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
    uint8 ChargeDelay_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(ChargeDelay_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: ChargeDelay_WriteControlRegister
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
    void ChargeDelay_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(ChargeDelay_CONTROL_PTR, control);
    }

#endif /* (ChargeDelay_UseControl) */


#if (!ChargeDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadCapture
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
    uint8 ChargeDelay_ReadCapture(void) 
    {
        return (CY_GET_REG8(ChargeDelay_CAPTURE_LSB_PTR));
    }

#endif /* (!ChargeDelay_UsingFixedFunction) */


#if (ChargeDelay_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadCompare
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
    uint8 ChargeDelay_ReadCompare(void) 
    {
        #if(ChargeDelay_UsingFixedFunction)
            return ((uint8)CY_GET_REG16(ChargeDelay_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG8(ChargeDelay_COMPARE1_LSB_PTR));
        #endif /* (ChargeDelay_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadCompare1
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
    uint8 ChargeDelay_ReadCompare1(void) 
    {
        return (CY_GET_REG8(ChargeDelay_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadCompare2
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
    uint8 ChargeDelay_ReadCompare2(void) 
    {
        return (CY_GET_REG8(ChargeDelay_COMPARE2_LSB_PTR));
    }

#endif /* (ChargeDelay_UseOneCompareMode) */


/*******************************************************************************
* Function Name: ChargeDelay_ReadPeriod
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
uint8 ChargeDelay_ReadPeriod(void) 
{
    #if(ChargeDelay_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(ChargeDelay_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(ChargeDelay_PERIOD_LSB_PTR));
    #endif /* (ChargeDelay_UsingFixedFunction) */
}

#if ( ChargeDelay_KillModeMinTime)


    /*******************************************************************************
    * Function Name: ChargeDelay_WriteKillTime
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
    void ChargeDelay_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(ChargeDelay_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: ChargeDelay_ReadKillTime
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
    uint8 ChargeDelay_ReadKillTime(void) 
    {
        return (CY_GET_REG8(ChargeDelay_KILLMODEMINTIME_PTR));
    }

#endif /* ( ChargeDelay_KillModeMinTime) */

/* [] END OF FILE */
