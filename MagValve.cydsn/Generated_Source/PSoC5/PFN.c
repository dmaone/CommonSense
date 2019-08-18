/*******************************************************************************
* File Name: PFN.c
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

#include "PFN.h"

/* Error message for removed <resource> through optimization */
#ifdef PFN_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* PFN_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 PFN_initVar = 0u;


/*******************************************************************************
* Function Name: PFN_Start
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
*  PFN_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void PFN_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(PFN_initVar == 0u)
    {
        PFN_Init();
        PFN_initVar = 1u;
    }
    PFN_Enable();

}


/*******************************************************************************
* Function Name: PFN_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  PFN_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void PFN_Init(void) 
{
    #if (PFN_UsingFixedFunction || PFN_UseControl)
        uint8 ctrl;
    #endif /* (PFN_UsingFixedFunction || PFN_UseControl) */

    #if(!PFN_UsingFixedFunction)
        #if(PFN_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 PFN_interruptState;
        #endif /* (PFN_UseStatus) */
    #endif /* (!PFN_UsingFixedFunction) */

    #if (PFN_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        PFN_CONTROL |= PFN_CFG0_MODE;
        #if (PFN_DeadBand2_4)
            PFN_CONTROL |= PFN_CFG0_DB;
        #endif /* (PFN_DeadBand2_4) */

        ctrl = PFN_CONTROL3 & ((uint8 )(~PFN_CTRL_CMPMODE1_MASK));
        PFN_CONTROL3 = ctrl | PFN_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        PFN_RT1 &= ((uint8)(~PFN_RT1_MASK));
        PFN_RT1 |= PFN_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        PFN_RT1 &= ((uint8)(~PFN_SYNCDSI_MASK));
        PFN_RT1 |= PFN_SYNCDSI_EN;

    #elif (PFN_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = PFN_CONTROL & ((uint8)(~PFN_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~PFN_CTRL_CMPMODE1_MASK));
        PFN_CONTROL = ctrl | PFN_DEFAULT_COMPARE2_MODE |
                                   PFN_DEFAULT_COMPARE1_MODE;
    #endif /* (PFN_UsingFixedFunction) */

    #if (!PFN_UsingFixedFunction)
        #if (PFN_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            PFN_AUX_CONTROLDP0 |= (PFN_AUX_CTRL_FIFO0_CLR);
        #else /* (PFN_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            PFN_AUX_CONTROLDP0 |= (PFN_AUX_CTRL_FIFO0_CLR);
            PFN_AUX_CONTROLDP1 |= (PFN_AUX_CTRL_FIFO0_CLR);
        #endif /* (PFN_Resolution == 8) */

        PFN_WriteCounter(PFN_INIT_PERIOD_VALUE);
    #endif /* (!PFN_UsingFixedFunction) */

    PFN_WritePeriod(PFN_INIT_PERIOD_VALUE);

        #if (PFN_UseOneCompareMode)
            PFN_WriteCompare(PFN_INIT_COMPARE_VALUE1);
        #else
            PFN_WriteCompare1(PFN_INIT_COMPARE_VALUE1);
            PFN_WriteCompare2(PFN_INIT_COMPARE_VALUE2);
        #endif /* (PFN_UseOneCompareMode) */

        #if (PFN_KillModeMinTime)
            PFN_WriteKillTime(PFN_MinimumKillTime);
        #endif /* (PFN_KillModeMinTime) */

        #if (PFN_DeadBandUsed)
            PFN_WriteDeadTime(PFN_INIT_DEAD_TIME);
        #endif /* (PFN_DeadBandUsed) */

    #if (PFN_UseStatus || PFN_UsingFixedFunction)
        PFN_SetInterruptMode(PFN_INIT_INTERRUPTS_MODE);
    #endif /* (PFN_UseStatus || PFN_UsingFixedFunction) */

    #if (PFN_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        PFN_GLOBAL_ENABLE |= PFN_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        PFN_CONTROL2 |= PFN_CTRL2_IRQ_SEL;
    #else
        #if(PFN_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            PFN_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            PFN_STATUS_AUX_CTRL |= PFN_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(PFN_interruptState);

            /* Clear the FIFO to enable the PFN_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            PFN_ClearFIFO();
        #endif /* (PFN_UseStatus) */
    #endif /* (PFN_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PFN_Enable
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
void PFN_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (PFN_UsingFixedFunction)
        PFN_GLOBAL_ENABLE |= PFN_BLOCK_EN_MASK;
        PFN_GLOBAL_STBY_ENABLE |= PFN_BLOCK_STBY_EN_MASK;
    #endif /* (PFN_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (PFN_UseControl || PFN_UsingFixedFunction)
        PFN_CONTROL |= PFN_CTRL_ENABLE;
    #endif /* (PFN_UseControl || PFN_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PFN_Stop
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
void PFN_Stop(void) 
{
    #if (PFN_UseControl || PFN_UsingFixedFunction)
        PFN_CONTROL &= ((uint8)(~PFN_CTRL_ENABLE));
    #endif /* (PFN_UseControl || PFN_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (PFN_UsingFixedFunction)
        PFN_GLOBAL_ENABLE &= ((uint8)(~PFN_BLOCK_EN_MASK));
        PFN_GLOBAL_STBY_ENABLE &= ((uint8)(~PFN_BLOCK_STBY_EN_MASK));
    #endif /* (PFN_UsingFixedFunction) */
}

#if (PFN_UseOneCompareMode)
    #if (PFN_CompareMode1SW)


        /*******************************************************************************
        * Function Name: PFN_SetCompareMode
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
        void PFN_SetCompareMode(uint8 comparemode) 
        {
            #if(PFN_UsingFixedFunction)

                #if(0 != PFN_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << PFN_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != PFN_CTRL_CMPMODE1_SHIFT) */

                PFN_CONTROL3 &= ((uint8)(~PFN_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                PFN_CONTROL3 |= comparemodemasked;

            #elif (PFN_UseControl)

                #if(0 != PFN_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << PFN_CTRL_CMPMODE1_SHIFT)) &
                                                PFN_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & PFN_CTRL_CMPMODE1_MASK;
                #endif /* (0 != PFN_CTRL_CMPMODE1_SHIFT) */

                #if(0 != PFN_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << PFN_CTRL_CMPMODE2_SHIFT)) &
                                               PFN_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & PFN_CTRL_CMPMODE2_MASK;
                #endif /* (0 != PFN_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                PFN_CONTROL &= ((uint8)(~(PFN_CTRL_CMPMODE1_MASK |
                                            PFN_CTRL_CMPMODE2_MASK)));
                PFN_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (PFN_UsingFixedFunction) */
        }
    #endif /* PFN_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (PFN_CompareMode1SW)


        /*******************************************************************************
        * Function Name: PFN_SetCompareMode1
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
        void PFN_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != PFN_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << PFN_CTRL_CMPMODE1_SHIFT)) &
                                           PFN_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & PFN_CTRL_CMPMODE1_MASK;
            #endif /* (0 != PFN_CTRL_CMPMODE1_SHIFT) */

            #if (PFN_UseControl)
                PFN_CONTROL &= ((uint8)(~PFN_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                PFN_CONTROL |= comparemodemasked;
            #endif /* (PFN_UseControl) */
        }
    #endif /* PFN_CompareMode1SW */

#if (PFN_CompareMode2SW)


    /*******************************************************************************
    * Function Name: PFN_SetCompareMode2
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
    void PFN_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != PFN_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << PFN_CTRL_CMPMODE2_SHIFT)) &
                                                 PFN_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & PFN_CTRL_CMPMODE2_MASK;
        #endif /* (0 != PFN_CTRL_CMPMODE2_SHIFT) */

        #if (PFN_UseControl)
            PFN_CONTROL &= ((uint8)(~PFN_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            PFN_CONTROL |= comparemodemasked;
        #endif /* (PFN_UseControl) */
    }
    #endif /*PFN_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!PFN_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: PFN_WriteCounter
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
    void PFN_WriteCounter(uint8 counter) \
                                       
    {
        CY_SET_REG8(PFN_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: PFN_ReadCounter
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
    uint8 PFN_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(PFN_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG8(PFN_CAPTURE_LSB_PTR));
    }

    #if (PFN_UseStatus)


        /*******************************************************************************
        * Function Name: PFN_ClearFIFO
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
        void PFN_ClearFIFO(void) 
        {
            while(0u != (PFN_ReadStatusRegister() & PFN_STATUS_FIFONEMPTY))
            {
                (void)PFN_ReadCapture();
            }
        }

    #endif /* PFN_UseStatus */

#endif /* !PFN_UsingFixedFunction */


/*******************************************************************************
* Function Name: PFN_WritePeriod
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
void PFN_WritePeriod(uint8 period) 
{
    #if(PFN_UsingFixedFunction)
        CY_SET_REG16(PFN_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG8(PFN_PERIOD_LSB_PTR, period);
    #endif /* (PFN_UsingFixedFunction) */
}

#if (PFN_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: PFN_WriteCompare
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
    void PFN_WriteCompare(uint8 compare) \
                                       
    {
        #if(PFN_UsingFixedFunction)
            CY_SET_REG16(PFN_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(PFN_COMPARE1_LSB_PTR, compare);
        #endif /* (PFN_UsingFixedFunction) */

        #if (PFN_PWMMode == PFN__B_PWM__DITHER)
            #if(PFN_UsingFixedFunction)
                CY_SET_REG16(PFN_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG8(PFN_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (PFN_UsingFixedFunction) */
        #endif /* (PFN_PWMMode == PFN__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: PFN_WriteCompare1
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
    void PFN_WriteCompare1(uint8 compare) \
                                        
    {
        #if(PFN_UsingFixedFunction)
            CY_SET_REG16(PFN_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(PFN_COMPARE1_LSB_PTR, compare);
        #endif /* (PFN_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: PFN_WriteCompare2
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
    void PFN_WriteCompare2(uint8 compare) \
                                        
    {
        #if(PFN_UsingFixedFunction)
            CY_SET_REG16(PFN_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG8(PFN_COMPARE2_LSB_PTR, compare);
        #endif /* (PFN_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (PFN_DeadBandUsed)


    /*******************************************************************************
    * Function Name: PFN_WriteDeadTime
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
    void PFN_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!PFN_DeadBand2_4)
            CY_SET_REG8(PFN_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            PFN_DEADBAND_COUNT &= ((uint8)(~PFN_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(PFN_DEADBAND_COUNT_SHIFT)
                PFN_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << PFN_DEADBAND_COUNT_SHIFT)) &
                                                    PFN_DEADBAND_COUNT_MASK;
            #else
                PFN_DEADBAND_COUNT |= deadtime & PFN_DEADBAND_COUNT_MASK;
            #endif /* (PFN_DEADBAND_COUNT_SHIFT) */

        #endif /* (!PFN_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: PFN_ReadDeadTime
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
    uint8 PFN_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!PFN_DeadBand2_4)
            return (CY_GET_REG8(PFN_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(PFN_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(PFN_DEADBAND_COUNT & PFN_DEADBAND_COUNT_MASK)) >>
                                                                           PFN_DEADBAND_COUNT_SHIFT));
            #else
                return (PFN_DEADBAND_COUNT & PFN_DEADBAND_COUNT_MASK);
            #endif /* (PFN_DEADBAND_COUNT_SHIFT) */
        #endif /* (!PFN_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (PFN_UseStatus || PFN_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: PFN_SetInterruptMode
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
    void PFN_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(PFN_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: PFN_ReadStatusRegister
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
    uint8 PFN_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(PFN_STATUS_PTR));
    }

#endif /* (PFN_UseStatus || PFN_UsingFixedFunction) */


#if (PFN_UseControl)


    /*******************************************************************************
    * Function Name: PFN_ReadControlRegister
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
    uint8 PFN_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(PFN_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: PFN_WriteControlRegister
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
    void PFN_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(PFN_CONTROL_PTR, control);
    }

#endif /* (PFN_UseControl) */


#if (!PFN_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: PFN_ReadCapture
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
    uint8 PFN_ReadCapture(void) 
    {
        return (CY_GET_REG8(PFN_CAPTURE_LSB_PTR));
    }

#endif /* (!PFN_UsingFixedFunction) */


#if (PFN_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: PFN_ReadCompare
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
    uint8 PFN_ReadCompare(void) 
    {
        #if(PFN_UsingFixedFunction)
            return ((uint8)CY_GET_REG16(PFN_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG8(PFN_COMPARE1_LSB_PTR));
        #endif /* (PFN_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: PFN_ReadCompare1
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
    uint8 PFN_ReadCompare1(void) 
    {
        return (CY_GET_REG8(PFN_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: PFN_ReadCompare2
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
    uint8 PFN_ReadCompare2(void) 
    {
        return (CY_GET_REG8(PFN_COMPARE2_LSB_PTR));
    }

#endif /* (PFN_UseOneCompareMode) */


/*******************************************************************************
* Function Name: PFN_ReadPeriod
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
uint8 PFN_ReadPeriod(void) 
{
    #if(PFN_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(PFN_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(PFN_PERIOD_LSB_PTR));
    #endif /* (PFN_UsingFixedFunction) */
}

#if ( PFN_KillModeMinTime)


    /*******************************************************************************
    * Function Name: PFN_WriteKillTime
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
    void PFN_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(PFN_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: PFN_ReadKillTime
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
    uint8 PFN_ReadKillTime(void) 
    {
        return (CY_GET_REG8(PFN_KILLMODEMINTIME_PTR));
    }

#endif /* ( PFN_KillModeMinTime) */

/* [] END OF FILE */
