/*******************************************************************************
* File Name: SetupDelay.c
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

#include "SetupDelay.h"

/* Error message for removed <resource> through optimization */
#ifdef SetupDelay_PWMUDB_genblk1_ctrlreg__REMOVED
    #error PWM_v3_30 detected with a constant 0 for the enable or \
         constant 1 for reset. This will prevent the component from operating.
#endif /* SetupDelay_PWMUDB_genblk1_ctrlreg__REMOVED */

uint8 SetupDelay_initVar = 0u;


/*******************************************************************************
* Function Name: SetupDelay_Start
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
*  SetupDelay_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void SetupDelay_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(SetupDelay_initVar == 0u)
    {
        SetupDelay_Init();
        SetupDelay_initVar = 1u;
    }
    SetupDelay_Enable();

}


/*******************************************************************************
* Function Name: SetupDelay_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  SetupDelay_Start().
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void SetupDelay_Init(void) 
{
    #if (SetupDelay_UsingFixedFunction || SetupDelay_UseControl)
        uint8 ctrl;
    #endif /* (SetupDelay_UsingFixedFunction || SetupDelay_UseControl) */

    #if(!SetupDelay_UsingFixedFunction)
        #if(SetupDelay_UseStatus)
            /* Interrupt State Backup for Critical Region*/
            uint8 SetupDelay_interruptState;
        #endif /* (SetupDelay_UseStatus) */
    #endif /* (!SetupDelay_UsingFixedFunction) */

    #if (SetupDelay_UsingFixedFunction)
        /* You are allowed to write the compare value (FF only) */
        SetupDelay_CONTROL |= SetupDelay_CFG0_MODE;
        #if (SetupDelay_DeadBand2_4)
            SetupDelay_CONTROL |= SetupDelay_CFG0_DB;
        #endif /* (SetupDelay_DeadBand2_4) */

        ctrl = SetupDelay_CONTROL3 & ((uint8 )(~SetupDelay_CTRL_CMPMODE1_MASK));
        SetupDelay_CONTROL3 = ctrl | SetupDelay_DEFAULT_COMPARE1_MODE;

         /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
        SetupDelay_RT1 &= ((uint8)(~SetupDelay_RT1_MASK));
        SetupDelay_RT1 |= SetupDelay_SYNC;

        /*Enable DSI Sync all all inputs of the PWM*/
        SetupDelay_RT1 &= ((uint8)(~SetupDelay_SYNCDSI_MASK));
        SetupDelay_RT1 |= SetupDelay_SYNCDSI_EN;

    #elif (SetupDelay_UseControl)
        /* Set the default compare mode defined in the parameter */
        ctrl = SetupDelay_CONTROL & ((uint8)(~SetupDelay_CTRL_CMPMODE2_MASK)) &
                ((uint8)(~SetupDelay_CTRL_CMPMODE1_MASK));
        SetupDelay_CONTROL = ctrl | SetupDelay_DEFAULT_COMPARE2_MODE |
                                   SetupDelay_DEFAULT_COMPARE1_MODE;
    #endif /* (SetupDelay_UsingFixedFunction) */

    #if (!SetupDelay_UsingFixedFunction)
        #if (SetupDelay_Resolution == 8)
            /* Set FIFO 0 to 1 byte register for period*/
            SetupDelay_AUX_CONTROLDP0 |= (SetupDelay_AUX_CTRL_FIFO0_CLR);
        #else /* (SetupDelay_Resolution == 16)*/
            /* Set FIFO 0 to 1 byte register for period */
            SetupDelay_AUX_CONTROLDP0 |= (SetupDelay_AUX_CTRL_FIFO0_CLR);
            SetupDelay_AUX_CONTROLDP1 |= (SetupDelay_AUX_CTRL_FIFO0_CLR);
        #endif /* (SetupDelay_Resolution == 8) */

        SetupDelay_WriteCounter(SetupDelay_INIT_PERIOD_VALUE);
    #endif /* (!SetupDelay_UsingFixedFunction) */

    SetupDelay_WritePeriod(SetupDelay_INIT_PERIOD_VALUE);

        #if (SetupDelay_UseOneCompareMode)
            SetupDelay_WriteCompare(SetupDelay_INIT_COMPARE_VALUE1);
        #else
            SetupDelay_WriteCompare1(SetupDelay_INIT_COMPARE_VALUE1);
            SetupDelay_WriteCompare2(SetupDelay_INIT_COMPARE_VALUE2);
        #endif /* (SetupDelay_UseOneCompareMode) */

        #if (SetupDelay_KillModeMinTime)
            SetupDelay_WriteKillTime(SetupDelay_MinimumKillTime);
        #endif /* (SetupDelay_KillModeMinTime) */

        #if (SetupDelay_DeadBandUsed)
            SetupDelay_WriteDeadTime(SetupDelay_INIT_DEAD_TIME);
        #endif /* (SetupDelay_DeadBandUsed) */

    #if (SetupDelay_UseStatus || SetupDelay_UsingFixedFunction)
        SetupDelay_SetInterruptMode(SetupDelay_INIT_INTERRUPTS_MODE);
    #endif /* (SetupDelay_UseStatus || SetupDelay_UsingFixedFunction) */

    #if (SetupDelay_UsingFixedFunction)
        /* Globally Enable the Fixed Function Block chosen */
        SetupDelay_GLOBAL_ENABLE |= SetupDelay_BLOCK_EN_MASK;
        /* Set the Interrupt source to come from the status register */
        SetupDelay_CONTROL2 |= SetupDelay_CTRL2_IRQ_SEL;
    #else
        #if(SetupDelay_UseStatus)

            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            SetupDelay_interruptState = CyEnterCriticalSection();
            /* Use the interrupt output of the status register for IRQ output */
            SetupDelay_STATUS_AUX_CTRL |= SetupDelay_STATUS_ACTL_INT_EN_MASK;

             /* Exit Critical Region*/
            CyExitCriticalSection(SetupDelay_interruptState);

            /* Clear the FIFO to enable the SetupDelay_STATUS_FIFOFULL
                   bit to be set on FIFO full. */
            SetupDelay_ClearFIFO();
        #endif /* (SetupDelay_UseStatus) */
    #endif /* (SetupDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SetupDelay_Enable
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
void SetupDelay_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (SetupDelay_UsingFixedFunction)
        SetupDelay_GLOBAL_ENABLE |= SetupDelay_BLOCK_EN_MASK;
        SetupDelay_GLOBAL_STBY_ENABLE |= SetupDelay_BLOCK_STBY_EN_MASK;
    #endif /* (SetupDelay_UsingFixedFunction) */

    /* Enable the PWM from the control register  */
    #if (SetupDelay_UseControl || SetupDelay_UsingFixedFunction)
        SetupDelay_CONTROL |= SetupDelay_CTRL_ENABLE;
    #endif /* (SetupDelay_UseControl || SetupDelay_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: SetupDelay_Stop
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
void SetupDelay_Stop(void) 
{
    #if (SetupDelay_UseControl || SetupDelay_UsingFixedFunction)
        SetupDelay_CONTROL &= ((uint8)(~SetupDelay_CTRL_ENABLE));
    #endif /* (SetupDelay_UseControl || SetupDelay_UsingFixedFunction) */

    /* Globally disable the Fixed Function Block chosen */
    #if (SetupDelay_UsingFixedFunction)
        SetupDelay_GLOBAL_ENABLE &= ((uint8)(~SetupDelay_BLOCK_EN_MASK));
        SetupDelay_GLOBAL_STBY_ENABLE &= ((uint8)(~SetupDelay_BLOCK_STBY_EN_MASK));
    #endif /* (SetupDelay_UsingFixedFunction) */
}

#if (SetupDelay_UseOneCompareMode)
    #if (SetupDelay_CompareMode1SW)


        /*******************************************************************************
        * Function Name: SetupDelay_SetCompareMode
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
        void SetupDelay_SetCompareMode(uint8 comparemode) 
        {
            #if(SetupDelay_UsingFixedFunction)

                #if(0 != SetupDelay_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemodemasked = ((uint8)((uint8)comparemode << SetupDelay_CTRL_CMPMODE1_SHIFT));
                #else
                    uint8 comparemodemasked = comparemode;
                #endif /* (0 != SetupDelay_CTRL_CMPMODE1_SHIFT) */

                SetupDelay_CONTROL3 &= ((uint8)(~SetupDelay_CTRL_CMPMODE1_MASK)); /*Clear Existing Data */
                SetupDelay_CONTROL3 |= comparemodemasked;

            #elif (SetupDelay_UseControl)

                #if(0 != SetupDelay_CTRL_CMPMODE1_SHIFT)
                    uint8 comparemode1masked = ((uint8)((uint8)comparemode << SetupDelay_CTRL_CMPMODE1_SHIFT)) &
                                                SetupDelay_CTRL_CMPMODE1_MASK;
                #else
                    uint8 comparemode1masked = comparemode & SetupDelay_CTRL_CMPMODE1_MASK;
                #endif /* (0 != SetupDelay_CTRL_CMPMODE1_SHIFT) */

                #if(0 != SetupDelay_CTRL_CMPMODE2_SHIFT)
                    uint8 comparemode2masked = ((uint8)((uint8)comparemode << SetupDelay_CTRL_CMPMODE2_SHIFT)) &
                                               SetupDelay_CTRL_CMPMODE2_MASK;
                #else
                    uint8 comparemode2masked = comparemode & SetupDelay_CTRL_CMPMODE2_MASK;
                #endif /* (0 != SetupDelay_CTRL_CMPMODE2_SHIFT) */

                /*Clear existing mode */
                SetupDelay_CONTROL &= ((uint8)(~(SetupDelay_CTRL_CMPMODE1_MASK |
                                            SetupDelay_CTRL_CMPMODE2_MASK)));
                SetupDelay_CONTROL |= (comparemode1masked | comparemode2masked);

            #else
                uint8 temp = comparemode;
            #endif /* (SetupDelay_UsingFixedFunction) */
        }
    #endif /* SetupDelay_CompareMode1SW */

#else /* UseOneCompareMode */

    #if (SetupDelay_CompareMode1SW)


        /*******************************************************************************
        * Function Name: SetupDelay_SetCompareMode1
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
        void SetupDelay_SetCompareMode1(uint8 comparemode) 
        {
            #if(0 != SetupDelay_CTRL_CMPMODE1_SHIFT)
                uint8 comparemodemasked = ((uint8)((uint8)comparemode << SetupDelay_CTRL_CMPMODE1_SHIFT)) &
                                           SetupDelay_CTRL_CMPMODE1_MASK;
            #else
                uint8 comparemodemasked = comparemode & SetupDelay_CTRL_CMPMODE1_MASK;
            #endif /* (0 != SetupDelay_CTRL_CMPMODE1_SHIFT) */

            #if (SetupDelay_UseControl)
                SetupDelay_CONTROL &= ((uint8)(~SetupDelay_CTRL_CMPMODE1_MASK)); /*Clear existing mode */
                SetupDelay_CONTROL |= comparemodemasked;
            #endif /* (SetupDelay_UseControl) */
        }
    #endif /* SetupDelay_CompareMode1SW */

#if (SetupDelay_CompareMode2SW)


    /*******************************************************************************
    * Function Name: SetupDelay_SetCompareMode2
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
    void SetupDelay_SetCompareMode2(uint8 comparemode) 
    {

        #if(0 != SetupDelay_CTRL_CMPMODE2_SHIFT)
            uint8 comparemodemasked = ((uint8)((uint8)comparemode << SetupDelay_CTRL_CMPMODE2_SHIFT)) &
                                                 SetupDelay_CTRL_CMPMODE2_MASK;
        #else
            uint8 comparemodemasked = comparemode & SetupDelay_CTRL_CMPMODE2_MASK;
        #endif /* (0 != SetupDelay_CTRL_CMPMODE2_SHIFT) */

        #if (SetupDelay_UseControl)
            SetupDelay_CONTROL &= ((uint8)(~SetupDelay_CTRL_CMPMODE2_MASK)); /*Clear existing mode */
            SetupDelay_CONTROL |= comparemodemasked;
        #endif /* (SetupDelay_UseControl) */
    }
    #endif /*SetupDelay_CompareMode2SW */

#endif /* UseOneCompareMode */


#if (!SetupDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: SetupDelay_WriteCounter
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
    void SetupDelay_WriteCounter(uint8 counter) \
                                       
    {
        CY_SET_REG8(SetupDelay_COUNTER_LSB_PTR, counter);
    }


    /*******************************************************************************
    * Function Name: SetupDelay_ReadCounter
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
    uint8 SetupDelay_ReadCounter(void) 
    {
        /* Force capture by reading Accumulator */
        /* Must first do a software capture to be able to read the counter */
        /* It is up to the user code to make sure there isn't already captured data in the FIFO */
          (void)CY_GET_REG8(SetupDelay_COUNTERCAP_LSB_PTR_8BIT);

        /* Read the data from the FIFO */
        return (CY_GET_REG8(SetupDelay_CAPTURE_LSB_PTR));
    }

    #if (SetupDelay_UseStatus)


        /*******************************************************************************
        * Function Name: SetupDelay_ClearFIFO
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
        void SetupDelay_ClearFIFO(void) 
        {
            while(0u != (SetupDelay_ReadStatusRegister() & SetupDelay_STATUS_FIFONEMPTY))
            {
                (void)SetupDelay_ReadCapture();
            }
        }

    #endif /* SetupDelay_UseStatus */

#endif /* !SetupDelay_UsingFixedFunction */


/*******************************************************************************
* Function Name: SetupDelay_WritePeriod
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
void SetupDelay_WritePeriod(uint8 period) 
{
    #if(SetupDelay_UsingFixedFunction)
        CY_SET_REG16(SetupDelay_PERIOD_LSB_PTR, (uint16)period);
    #else
        CY_SET_REG8(SetupDelay_PERIOD_LSB_PTR, period);
    #endif /* (SetupDelay_UsingFixedFunction) */
}

#if (SetupDelay_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: SetupDelay_WriteCompare
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
    void SetupDelay_WriteCompare(uint8 compare) \
                                       
    {
        #if(SetupDelay_UsingFixedFunction)
            CY_SET_REG16(SetupDelay_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(SetupDelay_COMPARE1_LSB_PTR, compare);
        #endif /* (SetupDelay_UsingFixedFunction) */

        #if (SetupDelay_PWMMode == SetupDelay__B_PWM__DITHER)
            #if(SetupDelay_UsingFixedFunction)
                CY_SET_REG16(SetupDelay_COMPARE2_LSB_PTR, (uint16)(compare + 1u));
            #else
                CY_SET_REG8(SetupDelay_COMPARE2_LSB_PTR, (compare + 1u));
            #endif /* (SetupDelay_UsingFixedFunction) */
        #endif /* (SetupDelay_PWMMode == SetupDelay__B_PWM__DITHER) */
    }


#else


    /*******************************************************************************
    * Function Name: SetupDelay_WriteCompare1
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
    void SetupDelay_WriteCompare1(uint8 compare) \
                                        
    {
        #if(SetupDelay_UsingFixedFunction)
            CY_SET_REG16(SetupDelay_COMPARE1_LSB_PTR, (uint16)compare);
        #else
            CY_SET_REG8(SetupDelay_COMPARE1_LSB_PTR, compare);
        #endif /* (SetupDelay_UsingFixedFunction) */
    }


    /*******************************************************************************
    * Function Name: SetupDelay_WriteCompare2
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
    void SetupDelay_WriteCompare2(uint8 compare) \
                                        
    {
        #if(SetupDelay_UsingFixedFunction)
            CY_SET_REG16(SetupDelay_COMPARE2_LSB_PTR, compare);
        #else
            CY_SET_REG8(SetupDelay_COMPARE2_LSB_PTR, compare);
        #endif /* (SetupDelay_UsingFixedFunction) */
    }
#endif /* UseOneCompareMode */

#if (SetupDelay_DeadBandUsed)


    /*******************************************************************************
    * Function Name: SetupDelay_WriteDeadTime
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
    void SetupDelay_WriteDeadTime(uint8 deadtime) 
    {
        /* If using the Dead Band 1-255 mode then just write the register */
        #if(!SetupDelay_DeadBand2_4)
            CY_SET_REG8(SetupDelay_DEADBAND_COUNT_PTR, deadtime);
        #else
            /* Otherwise the data has to be masked and offset */
            /* Clear existing data */
            SetupDelay_DEADBAND_COUNT &= ((uint8)(~SetupDelay_DEADBAND_COUNT_MASK));

            /* Set new dead time */
            #if(SetupDelay_DEADBAND_COUNT_SHIFT)
                SetupDelay_DEADBAND_COUNT |= ((uint8)((uint8)deadtime << SetupDelay_DEADBAND_COUNT_SHIFT)) &
                                                    SetupDelay_DEADBAND_COUNT_MASK;
            #else
                SetupDelay_DEADBAND_COUNT |= deadtime & SetupDelay_DEADBAND_COUNT_MASK;
            #endif /* (SetupDelay_DEADBAND_COUNT_SHIFT) */

        #endif /* (!SetupDelay_DeadBand2_4) */
    }


    /*******************************************************************************
    * Function Name: SetupDelay_ReadDeadTime
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
    uint8 SetupDelay_ReadDeadTime(void) 
    {
        /* If using the Dead Band 1-255 mode then just read the register */
        #if(!SetupDelay_DeadBand2_4)
            return (CY_GET_REG8(SetupDelay_DEADBAND_COUNT_PTR));
        #else

            /* Otherwise the data has to be masked and offset */
            #if(SetupDelay_DEADBAND_COUNT_SHIFT)
                return ((uint8)(((uint8)(SetupDelay_DEADBAND_COUNT & SetupDelay_DEADBAND_COUNT_MASK)) >>
                                                                           SetupDelay_DEADBAND_COUNT_SHIFT));
            #else
                return (SetupDelay_DEADBAND_COUNT & SetupDelay_DEADBAND_COUNT_MASK);
            #endif /* (SetupDelay_DEADBAND_COUNT_SHIFT) */
        #endif /* (!SetupDelay_DeadBand2_4) */
    }
#endif /* DeadBandUsed */

#if (SetupDelay_UseStatus || SetupDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: SetupDelay_SetInterruptMode
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
    void SetupDelay_SetInterruptMode(uint8 interruptMode) 
    {
        CY_SET_REG8(SetupDelay_STATUS_MASK_PTR, interruptMode);
    }


    /*******************************************************************************
    * Function Name: SetupDelay_ReadStatusRegister
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
    uint8 SetupDelay_ReadStatusRegister(void) 
    {
        return (CY_GET_REG8(SetupDelay_STATUS_PTR));
    }

#endif /* (SetupDelay_UseStatus || SetupDelay_UsingFixedFunction) */


#if (SetupDelay_UseControl)


    /*******************************************************************************
    * Function Name: SetupDelay_ReadControlRegister
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
    uint8 SetupDelay_ReadControlRegister(void) 
    {
        uint8 result;

        result = CY_GET_REG8(SetupDelay_CONTROL_PTR);
        return (result);
    }


    /*******************************************************************************
    * Function Name: SetupDelay_WriteControlRegister
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
    void SetupDelay_WriteControlRegister(uint8 control) 
    {
        CY_SET_REG8(SetupDelay_CONTROL_PTR, control);
    }

#endif /* (SetupDelay_UseControl) */


#if (!SetupDelay_UsingFixedFunction)


    /*******************************************************************************
    * Function Name: SetupDelay_ReadCapture
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
    uint8 SetupDelay_ReadCapture(void) 
    {
        return (CY_GET_REG8(SetupDelay_CAPTURE_LSB_PTR));
    }

#endif /* (!SetupDelay_UsingFixedFunction) */


#if (SetupDelay_UseOneCompareMode)


    /*******************************************************************************
    * Function Name: SetupDelay_ReadCompare
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
    uint8 SetupDelay_ReadCompare(void) 
    {
        #if(SetupDelay_UsingFixedFunction)
            return ((uint8)CY_GET_REG16(SetupDelay_COMPARE1_LSB_PTR));
        #else
            return (CY_GET_REG8(SetupDelay_COMPARE1_LSB_PTR));
        #endif /* (SetupDelay_UsingFixedFunction) */
    }

#else


    /*******************************************************************************
    * Function Name: SetupDelay_ReadCompare1
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
    uint8 SetupDelay_ReadCompare1(void) 
    {
        return (CY_GET_REG8(SetupDelay_COMPARE1_LSB_PTR));
    }


    /*******************************************************************************
    * Function Name: SetupDelay_ReadCompare2
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
    uint8 SetupDelay_ReadCompare2(void) 
    {
        return (CY_GET_REG8(SetupDelay_COMPARE2_LSB_PTR));
    }

#endif /* (SetupDelay_UseOneCompareMode) */


/*******************************************************************************
* Function Name: SetupDelay_ReadPeriod
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
uint8 SetupDelay_ReadPeriod(void) 
{
    #if(SetupDelay_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(SetupDelay_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(SetupDelay_PERIOD_LSB_PTR));
    #endif /* (SetupDelay_UsingFixedFunction) */
}

#if ( SetupDelay_KillModeMinTime)


    /*******************************************************************************
    * Function Name: SetupDelay_WriteKillTime
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
    void SetupDelay_WriteKillTime(uint8 killtime) 
    {
        CY_SET_REG8(SetupDelay_KILLMODEMINTIME_PTR, killtime);
    }


    /*******************************************************************************
    * Function Name: SetupDelay_ReadKillTime
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
    uint8 SetupDelay_ReadKillTime(void) 
    {
        return (CY_GET_REG8(SetupDelay_KILLMODEMINTIME_PTR));
    }

#endif /* ( SetupDelay_KillModeMinTime) */

/* [] END OF FILE */
