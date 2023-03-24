/*******************************************************************************
* File Name: ADC0.c
* Version 3.10
*
* Description:
*  This file provides the source code to the API for the Successive
*  approximation ADC Component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CyLib.h"
#include "ADC0.h"

#if(ADC0_DEFAULT_INTERNAL_CLK)
    #include "ADC0_theACLK.h"
#endif /* End ADC0_DEFAULT_INTERNAL_CLK */


/***************************************
* Forward function references
***************************************/
static void ADC0_CalcGain(uint8 resolution);


/***************************************
* Global data allocation
***************************************/
uint8 ADC0_initVar = 0u;
volatile int16 ADC0_offset;
volatile int16 ADC0_countsPerVolt;     /* Obsolete Gain compensation */
volatile int32 ADC0_countsPer10Volt;   /* Gain compensation */
volatile int16 ADC0_shift;


/*******************************************************************************
* Function Name: ADC0_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  ADC0_Start() sets the initVar variable, calls the
*  ADC0_Init() function, and then calls the
*  ADC0_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The ADC0_initVar variable is used to indicate when/if initial
*  configuration of this component has happened. The variable is initialized to
*  zero and set to 1 the first time ADC_Start() is called. This allows for
*  component Re-Start without re-initialization in all subsequent calls to the
*  ADC0_Start() routine.
*  If re-initialization of the component is required the variable should be set
*  to zero before call of ADC0_Start() routine, or the user may call
*  ADC0_Init() and ADC0_Enable() as done in the
*  ADC0_Start() routine.
*
* Side Effect:
*  If the initVar variable is already set, this function only calls the
*  ADC0_Enable() function.
*
*******************************************************************************/
void ADC0_Start(void)
{

    /* If not Initialized then initialize all required hardware and software */
    if(ADC0_initVar == 0u)
    {
        ADC0_Init();
        ADC0_initVar = 1u;
    }
    ADC0_Enable();
}


/*******************************************************************************
* Function Name: ADC0_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  ADC0_Start().
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The ADC0_offset variable is initialized to 0.
*
*******************************************************************************/
void ADC0_Init(void)
{

    /* This is only valid if there is an internal clock */
    #if(ADC0_DEFAULT_INTERNAL_CLK)
        ADC0_theACLK_SetMode(CYCLK_DUTY);
    #endif /* End ADC0_DEFAULT_INTERNAL_CLK */

    #if(ADC0_IRQ_REMOVE == 0u)
        /* Start and set interrupt vector */
        CyIntSetPriority(ADC0_INTC_NUMBER, ADC0_INTC_PRIOR_NUMBER);
        (void)CyIntSetVector(ADC0_INTC_NUMBER, &ADC0_ISR);
    #endif   /* End ADC0_IRQ_REMOVE */

    /* Enable IRQ mode*/
    ADC0_SAR_CSR1_REG |= ADC0_SAR_IRQ_MASK_EN | ADC0_SAR_IRQ_MODE_EDGE;

    /*Set SAR ADC resolution ADC */
    ADC0_SetResolution(ADC0_DEFAULT_RESOLUTION);
    ADC0_offset = 0;
}


/*******************************************************************************
* Function Name: ADC0_Enable
********************************************************************************
*
* Summary:
*  Enables the reference, clock and power for SAR ADC.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC0_Enable(void)
{
    uint8 tmpReg;
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Enable the SAR ADC block in Active Power Mode */
    ADC0_PWRMGR_SAR_REG |= ADC0_ACT_PWR_SAR_EN;

     /* Enable the SAR ADC in Standby Power Mode*/
    ADC0_STBY_PWRMGR_SAR_REG |= ADC0_STBY_PWR_SAR_EN;

    /* This is only valid if there is an internal clock */
    #if(ADC0_DEFAULT_INTERNAL_CLK)
        ADC0_PWRMGR_CLK_REG |= ADC0_ACT_PWR_CLK_EN;
        ADC0_STBY_PWRMGR_CLK_REG |= ADC0_STBY_PWR_CLK_EN;
    #endif /* End ADC0_DEFAULT_INTERNAL_CLK */

    /* Enable VCM buffer and Enable Int Ref Amp */
    tmpReg = ADC0_SAR_CSR3_REG;
    tmpReg |= ADC0_SAR_EN_BUF_VCM_EN;
    /* PD_BUF_VREF is OFF in External reference or Vdda reference mode */
    #if((ADC0_DEFAULT_REFERENCE == ADC0__EXT_REF) || \
        (ADC0_DEFAULT_RANGE == ADC0__VNEG_VDDA_DIFF))
        tmpReg &= (uint8)~ADC0_SAR_EN_BUF_VREF_EN;
    #else /* In INTREF or INTREF Bypassed this buffer is ON */
        tmpReg |= ADC0_SAR_EN_BUF_VREF_EN;
    #endif /* ADC0_DEFAULT_REFERENCE == ADC0__EXT_REF */
    ADC0_SAR_CSR3_REG = tmpReg;

    /* Set reference for ADC */
    #if(ADC0_DEFAULT_RANGE == ADC0__VNEG_VDDA_DIFF)
        #if(ADC0_DEFAULT_REFERENCE == ADC0__EXT_REF)
            ADC0_SAR_CSR6_REG = ADC0_INT_BYPASS_EXT_VREF; /* S2 */
        #else /* Internal Vdda reference or obsolete bypass mode */
            ADC0_SAR_CSR6_REG = ADC0_VDDA_VREF;           /* S7 */
        #endif /* ADC0_DEFAULT_REFERENCE == ADC0__EXT_REF */
    #else  /* Reference goes through internal buffer */
        #if(ADC0_DEFAULT_REFERENCE == ADC0__INT_REF_NOT_BYPASSED)
            ADC0_SAR_CSR6_REG = ADC0_INT_VREF;            /* S3 + S4 */
        #else /* INTREF Bypassed of External */
            ADC0_SAR_CSR6_REG = ADC0_INT_BYPASS_EXT_VREF; /* S2 */
        #endif /* ADC0_DEFAULT_REFERENCE == ADC0__INT_REF_NOT_BYPASSED */
    #endif /* VNEG_VDDA_DIFF */

    /* Low non-overlap delay for sampling clock signals (for 1MSPS) */
    #if(ADC0_HIGH_POWER_PULSE == 0u) /* MinPulseWidth <= 50 ns */
        ADC0_SAR_CSR5_REG &= (uint8)~ADC0_SAR_DLY_INC;
    #else /* Set High non-overlap delay for sampling clock signals (for <500KSPS)*/
        ADC0_SAR_CSR5_REG |= ADC0_SAR_DLY_INC;
    #endif /* ADC0_HIGH_POWER_PULSE == 0u */

    /* Increase comparator latch enable delay by 20%, 
    *  Increase comparator bias current by 30% without impacting delaysDelay 
    *  Default for 1MSPS) 
    */
    #if(ADC0_HIGH_POWER_PULSE == 0u)    /* MinPulseWidth <= 50 ns */
        ADC0_SAR_CSR5_REG |= ADC0_SAR_SEL_CSEL_DFT_CHAR;
    #else /* for <500ksps */
        ADC0_SAR_CSR5_REG &= (uint8)~ADC0_SAR_SEL_CSEL_DFT_CHAR;
    #endif /* ADC0_HIGH_POWER_PULSE == 0u */

    /* Set default power and other configurations for control register 0 in multiple lines */
    ADC0_SAR_CSR0_REG = (uint8)((uint8)ADC0_DEFAULT_POWER << ADC0_SAR_POWER_SHIFT)
    /* SAR_HIZ_CLEAR:   Should not be used for LP */
    #if ((CY_PSOC5LP) || (ADC0_DEFAULT_REFERENCE != ADC0__EXT_REF))
        | ADC0_SAR_HIZ_CLEAR
    #endif /* SAR_HIZ_CLEAR */
    /*Set Convertion mode */
    #if(ADC0_DEFAULT_CONV_MODE != ADC0__FREE_RUNNING)      /* If triggered mode */
        | ADC0_SAR_MX_SOF_UDB           /* source: UDB */
        | ADC0_SAR_SOF_MODE_EDGE        /* Set edge-sensitive sof source */
    #endif /* ADC0_DEFAULT_CONV_MODE */
    ; /* end of multiple line initialization */

    ADC0_SAR_TR0_REG = ADC0_SAR_CAP_TRIM_2;

    /* Enable clock for SAR ADC*/
    ADC0_SAR_CLK_REG |= ADC0_SAR_MX_CLK_EN;

    CyDelayUs(10u); /* The block is ready to use 10 us after the enable signal is set high. */

    #if(ADC0_IRQ_REMOVE == 0u)
        /* Clear a pending interrupt */
        CyIntClearPending(ADC0_INTC_NUMBER);
    #endif   /* End ADC0_IRQ_REMOVE */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: ADC0_Stop
********************************************************************************
*
* Summary:
*  Stops ADC conversions and puts the ADC into its lowest power mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC0_Stop(void)
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Stop all conversions */
    ADC0_SAR_CSR0_REG &= (uint8)~ADC0_SAR_SOF_START_CONV;
    /* Disable the SAR ADC block in Active Power Mode */
    ADC0_PWRMGR_SAR_REG &= (uint8)~ADC0_ACT_PWR_SAR_EN;
    /* Disable the SAR ADC in Standby Power Mode */
    ADC0_STBY_PWRMGR_SAR_REG &= (uint8)~ADC0_STBY_PWR_SAR_EN;

    /* This is only valid if there is an internal clock */
    #if(ADC0_DEFAULT_INTERNAL_CLK)
        ADC0_PWRMGR_CLK_REG &= (uint8)~ADC0_ACT_PWR_CLK_EN;
        ADC0_STBY_PWRMGR_CLK_REG &= (uint8)~ADC0_STBY_PWR_CLK_EN;
    #endif /* End ADC0_DEFAULT_INTERNAL_CLK */

    CyExitCriticalSection(enableInterrupts);

}


/*******************************************************************************
* Function Name: ADC0_SetPower
********************************************************************************
*
* Summary:
*  Sets the operational power of the ADC. You should use the higher power
*  settings with faster clock speeds.
*
* Parameters:
*  power:  Power setting for ADC
*  0 ->    Normal
*  1 ->    Medium power
*  2 ->    1.25 power
*  3 ->    Minimum power.
*
* Return:
*  None.
*
*******************************************************************************/
void ADC0_SetPower(uint8 power)
{
    uint8 tmpReg;

    /* mask off invalid power settings */
    power &= ADC0_SAR_API_POWER_MASK;

    /* Set Power parameter  */
    tmpReg = ADC0_SAR_CSR0_REG & (uint8)~ADC0_SAR_POWER_MASK;
    tmpReg |= (uint8)(power << ADC0_SAR_POWER_SHIFT);
    ADC0_SAR_CSR0_REG = tmpReg;
}


/*******************************************************************************
* Function Name: ADC0_SetResolution
********************************************************************************
*
* Summary:
*  Sets the Relution of the SAR.
*
* Parameters:
*  resolution:
*  12 ->    RES12
*  10 ->    RES10
*  8  ->    RES8
*
* Return:
*  None.
*
* Side Effects:
*  The ADC resolution cannot be changed during a conversion cycle. The
*  recommended best practice is to stop conversions with
*  ADC_StopConvert(), change the resolution, then restart the
*  conversions with ADC_StartConvert().
*  If you decide not to stop conversions before calling this API, you
*  should use ADC_IsEndConversion() to wait until conversion is complete
*  before changing the resolution.
*  If you call ADC_SetResolution() during a conversion, the resolution will
*  not be changed until the current conversion is complete. Data will not be
*  available in the new resolution for another 6 + "New Resolution(in bits)"
*  clock cycles.
*  You may need add a delay of this number of clock cycles after
*  ADC_SetResolution() is called before data is valid again.
*  Affects ADC_CountsTo_Volts(), ADC_CountsTo_mVolts(), and
*  ADC_CountsTo_uVolts() by calculating the correct conversion between ADC
*  counts and the applied input voltage. Calculation depends on resolution,
*  input range, and voltage reference.
*
*******************************************************************************/
void ADC0_SetResolution(uint8 resolution)
{
    uint8 tmpReg;

    /* Set SAR ADC resolution and sample width: 18 conversion cycles at 12bits + 1 gap */
    switch (resolution)
    {
        case (uint8)ADC0__BITS_12:
            tmpReg = ADC0_SAR_RESOLUTION_12BIT | ADC0_SAR_SAMPLE_WIDTH;
            break;
        case (uint8)ADC0__BITS_10:
            tmpReg = ADC0_SAR_RESOLUTION_10BIT | ADC0_SAR_SAMPLE_WIDTH;
            break;
        case (uint8)ADC0__BITS_8:
            tmpReg = ADC0_SAR_RESOLUTION_8BIT | ADC0_SAR_SAMPLE_WIDTH;
            break;
        default:
            tmpReg = ADC0_SAR_RESOLUTION_12BIT | ADC0_SAR_SAMPLE_WIDTH;
            /* Halt CPU in debug mode if resolution is out of valid range */
            CYASSERT(0u != 0u);
            break;
    }
    ADC0_SAR_CSR2_REG = tmpReg;

     /* Calculate gain for convert counts to volts */
    ADC0_CalcGain(resolution);
}


#if(ADC0_DEFAULT_CONV_MODE != ADC0__HARDWARE_TRIGGER)


    /*******************************************************************************
    * Function Name: ADC0_StartConvert
    ********************************************************************************
    *
    * Summary:
    *  Forces the ADC to initiate a conversion. In free-running mode, the ADC runs
    *  continuously. In software trigger mode, the function also acts as a software
    *  version of the SOC and every conversion must be triggered by
    *  ADC0_StartConvert(). This function is not available when the
    *  Hardware Trigger sample mode is selected.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Forces the ADC to initiate a conversion. In Free Running mode, the ADC will
    *  run continuously. In a software trigger mode the function also acts as a
    *  software version of the SOC. Here every conversion has to be triggered by
    *  the routine. This writes into the SOC bit in SAR_CTRL reg.
    *
    * Side Effects:
    *  In a software trigger mode the function switches source for SOF from the
    *  external pin to the internal SOF generation. Application should not call
    *  StartConvert if external source used for SOF.
    *
    *******************************************************************************/
    void ADC0_StartConvert(void)
    {
        #if(ADC0_DEFAULT_CONV_MODE != ADC0__FREE_RUNNING)  /* If software triggered mode */
            ADC0_SAR_CSR0_REG &= (uint8)~ADC0_SAR_MX_SOF_UDB;   /* source: SOF bit */
        #endif /* End ADC0_DEFAULT_CONV_MODE */

        /* Start the conversion */
        ADC0_SAR_CSR0_REG |= ADC0_SAR_SOF_START_CONV;
    }


    /*******************************************************************************
    * Function Name: ADC0_StopConvert
    ********************************************************************************
    *
    * Summary:
    *  Forces the ADC to stop conversions. If a conversion is currently executing,
    *  that conversion will complete, but no further conversions will occur. This
    *  function is not available when the Hardware Trigger sample mode is selected.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Stops ADC conversion in Free Running mode.
    *
    * Side Effects:
    *  In Software Trigger sample mode, this function sets a software version of the
    *  SOC to low level and switches the SOC source to hardware SOC input.
    *
    *******************************************************************************/
    void ADC0_StopConvert(void)
    {
        /* Stop all conversions */
        ADC0_SAR_CSR0_REG &= (uint8)~ADC0_SAR_SOF_START_CONV;

        #if(ADC0_DEFAULT_CONV_MODE != ADC0__FREE_RUNNING)  /* If software triggered mode */
            /* Return source to UDB for hardware SOF signal */
            ADC0_SAR_CSR0_REG |= ADC0_SAR_MX_SOF_UDB;    /* source: UDB */
        #endif /* End ADC0_DEFAULT_CONV_MODE */

    }

#endif /* End ADC0_DEFAULT_CONV_MODE != ADC0__HARDWARE_TRIGGER */


/*******************************************************************************
* Function Name: ADC0_IsEndConversion
********************************************************************************
*
* Summary:
*  Immediately returns the status of the conversion or does not return
*  (blocking) until the conversion completes, depending on the retMode
*  parameter.
*
* Parameters:
*  retMode:  Check conversion return mode.
*   ADC0_RETURN_STATUS: Immediately returns the status. If the
*     value returned is zero, the conversion is not complete, and this function
*     should be retried until a nonzero result is returned.
*   ADC0_WAIT_FOR_RESULT: Does not return a result until the ADC
*     conversion is complete.
*
* Return:
*  (uint8)  0 =>  The ADC is still calculating the last result.
*           1 =>  The last conversion is complete.
*
* Side Effects:
*  This function reads the end of conversion status, which is cleared on read.
*
*******************************************************************************/
uint8 ADC0_IsEndConversion(uint8 retMode)
{
    uint8 status;

    do
    {
        status = ADC0_SAR_CSR1_REG & ADC0_SAR_EOF_1;
    } while ((status != ADC0_SAR_EOF_1) && (retMode == ADC0_WAIT_FOR_RESULT));
    /* If convertion complete, wait until EOF bit released */
    if(status == ADC0_SAR_EOF_1)
    {
        /* wait one ADC clock to let the EOC status bit release */
        CyDelayUs(1u);
        /* Do the unconditional read operation of the CSR1 register to make sure the EOC bit has been cleared */
        CY_GET_REG8(ADC0_SAR_CSR1_PTR);
    }

    return(status);
}


/*******************************************************************************
* Function Name: ADC0_GetResult8
********************************************************************************
*
* Summary:
*  Returns the result of an 8-bit conversion. If the resolution is set greater
*  than 8 bits, the function returns the LSB of the result.
*  ADC0_IsEndConversion() should be called to verify that the data
*   sample is ready.
*
* Parameters:
*  None.
*
* Return:
*  The LSB of the last ADC conversion.
*
* Global Variables:
*  ADC0_shift - used to convert the ADC counts to the 2s
*  compliment form.
*
* Side Effects:
*  Converts the ADC counts to the 2s complement form.
*
*******************************************************************************/
int8 ADC0_GetResult8( void )
{
    return( (int8)ADC0_SAR_WRK0_REG - (int8)ADC0_shift);
}


/*******************************************************************************
* Function Name: ADC0_GetResult16
********************************************************************************
*
* Summary:
*  Returns a 16-bit result for a conversion with a result that has a resolution
*  of 8 to 12 bits.
*  ADC0_IsEndConversion() should be called to verify that the data
*   sample is ready
*
* Parameters:
*  None.
*
* Return:
*  The 16-bit result of the last ADC conversion
*
* Global Variables:
*  ADC0_shift - used to convert the ADC counts to the 2s
*  compliment form.
*
* Side Effects:
*  Converts the ADC counts to the 2s complement form.
*
*******************************************************************************/
int16 ADC0_GetResult16( void )
{
    uint16 res;

    res = CY_GET_REG16(ADC0_SAR_WRK_PTR);

    return( (int16)res - ADC0_shift );
}


/*******************************************************************************
* Function Name: ADC0_SetOffset
********************************************************************************
*
* Summary:
*  Sets the ADC offset, which is used by ADC0_CountsTo_Volts(),
*  ADC0_CountsTo_mVolts(), and ADC0_CountsTo_uVolts()
*  to subtract the offset from the given reading before calculating the voltage
*  conversion.
*
* Parameters:
*  int16: This value is measured when the inputs are shorted or connected to
   the same input voltage.
*
* Return:
*  None.
*
* Global Variables:
*  The ADC0_offset variable modified. This variable is used for
*  offset calibration purpose.
*  Affects the ADC0_CountsTo_Volts,
*  ADC0_CountsTo_mVolts, ADC0_CountsTo_uVolts functions
*  by subtracting the given offset.
*
*******************************************************************************/
void ADC0_SetOffset(int16 offset)
{
    ADC0_offset = offset;
}


/*******************************************************************************
* Function Name: ADC0_CalcGain
********************************************************************************
*
* Summary:
*  This function calculates the ADC gain in counts per 10 volt.
*
* Parameters:
*  uint8: resolution
*
* Return:
*  None.
*
* Global Variables:
*  ADC0_shift variable initialized. This variable is used to
*  convert the ADC counts to the 2s compliment form.
*  ADC0_countsPer10Volt variable initialized. This variable is used
*  for gain calibration purpose.
*
*******************************************************************************/
static void ADC0_CalcGain( uint8 resolution )
{
    int32 counts;
    #if(!((ADC0_DEFAULT_RANGE == ADC0__VSS_TO_VREF) || \
         (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDDA) || \
         (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDAC)) )
        uint16 diff_zero;
    #endif /* End ADC0_DEFAULT_RANGE */

    switch (resolution)
    {
        case (uint8)ADC0__BITS_12:
            counts = (int32)ADC0_SAR_WRK_MAX_12BIT;
            #if(!((ADC0_DEFAULT_RANGE == ADC0__VSS_TO_VREF) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDDA) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDAC)) )
                diff_zero = ADC0_SAR_DIFF_SHIFT;
            #endif /* End ADC0_DEFAULT_RANGE */
            break;
        case (uint8)ADC0__BITS_10:
            counts = (int32)ADC0_SAR_WRK_MAX_10BIT;
            #if(!((ADC0_DEFAULT_RANGE == ADC0__VSS_TO_VREF) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDDA) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDAC)) )
                diff_zero = ADC0_SAR_DIFF_SHIFT >> 2u;
            #endif /* End ADC0_DEFAULT_RANGE */
            break;
        case (uint8)ADC0__BITS_8:
            counts = (int32)ADC0_SAR_WRK_MAX_8BIT;
            #if(!((ADC0_DEFAULT_RANGE == ADC0__VSS_TO_VREF) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDDA) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDAC)) )
                diff_zero = ADC0_SAR_DIFF_SHIFT >> 4u;
            #endif /* End ADC0_DEFAULT_RANGE */
            break;
        default: /* Halt CPU in debug mode if resolution is out of valid range */
            counts = 0;
            #if(!((ADC0_DEFAULT_RANGE == ADC0__VSS_TO_VREF) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDDA) || \
                 (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDAC)) )
                diff_zero = 0u;
            #endif /* End ADC0_DEFAULT_RANGE */
            CYASSERT(0u != 0u);
            break;
    }
    ADC0_countsPerVolt = 0; /* Clear obsolete variable */
    /* Calculate gain in counts per 10 volts with rounding */
    ADC0_countsPer10Volt = (((counts * ADC0_10MV_COUNTS) +
                        ADC0_DEFAULT_REF_VOLTAGE_MV) / (ADC0_DEFAULT_REF_VOLTAGE_MV * 2));

    #if( (ADC0_DEFAULT_RANGE == ADC0__VSS_TO_VREF) || \
         (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDDA) || \
         (ADC0_DEFAULT_RANGE == ADC0__VSSA_TO_VDAC) )
        ADC0_shift = 0;
    #else
        ADC0_shift = diff_zero;
    #endif /* End ADC0_DEFAULT_RANGE */
}


/*******************************************************************************
* Function Name: ADC0_SetGain
********************************************************************************
*
* Summary:
*  Sets the ADC gain in counts per volt for the voltage conversion functions
*  that follow. This value is set by default by the reference and input range
*  settings. It should only be used to further calibrate the ADC with a known
*  input or if the ADC is using an external reference.
*
* Parameters:
*  int16 adcGain counts per volt
*
* Return:
*  None.
*
* Global Variables:
*  ADC0_countsPer10Volt variable modified. This variable is used
*  for gain calibration purpose.
*
*******************************************************************************/
void ADC0_SetGain(int16 adcGain)
{
    ADC0_countsPer10Volt = (int32)adcGain * 10;
}


/*******************************************************************************
* Function Name: ADC0_SetScaledGain
********************************************************************************
*
* Summary:
*  Sets the ADC gain in counts per 10 volt for the voltage conversion functions
*  that follow. This value is set by default by the reference and input range
*  settings. It should only be used to further calibrate the ADC with a known
*  input or if the ADC is using an external reference.
*
* Parameters:
*  int32 adcGain  counts per 10 volt
*
* Return:
*  None.
*
* Global Variables:
*  ADC0_countsPer10Volt variable modified. This variable is used
*  for gain calibration purpose.
*
*******************************************************************************/
void ADC0_SetScaledGain(int32 adcGain)
{
    ADC0_countsPer10Volt = adcGain;
}


/*******************************************************************************
* Function Name: ADC0_CountsTo_mVolts
********************************************************************************
*
* Summary:
*  Converts the ADC output to millivolts as a 16-bit integer.
*
* Parameters:
*  int16 adcCounts:  Result from the ADC conversion
*
* Return:
*  int16 Result in mVolts
*
* Global Variables:
*  ADC0_offset variable used.
*  ADC0_countsPer10Volt variable used.
*
*******************************************************************************/
int16 ADC0_CountsTo_mVolts(int16 adcCounts)
{
    int16 mVolts;
    int32 countsPer10Volt;

    if(ADC0_countsPerVolt != 0)
    {   /* Support obsolete method */
        countsPer10Volt = (int32)ADC0_countsPerVolt * 10;
    }
    else
    {
        countsPer10Volt = ADC0_countsPer10Volt;
    }

    /* Subtract ADC offset */
    adcCounts -= ADC0_offset;
    /* Convert to millivolts with rounding */
    mVolts = (int16)( (( (int32)adcCounts * ADC0_10MV_COUNTS ) + ( (adcCounts > 0) ?
                       (countsPer10Volt / 2) : (-(countsPer10Volt / 2)) )) / countsPer10Volt);

    return( mVolts );
}


/*******************************************************************************
* Function Name: ADC0_CountsTo_uVolts
********************************************************************************
*
* Summary:
*  Converts the ADC output to microvolts as a 32-bit integer.
*
* Parameters:
*  int16 adcCounts: Result from the ADC conversion
*
* Return:
*  int32 Result in micro Volts
*
* Global Variables:
*  ADC0_offset variable used.
*  ADC0_countsPer10Volt used to convert ADC counts to uVolts.
*
*******************************************************************************/
int32 ADC0_CountsTo_uVolts(int16 adcCounts)
{

    int64 uVolts;
    int32 countsPer10Volt;

    if(ADC0_countsPerVolt != 0)
    {   /* Support obsolete method */
        countsPer10Volt = (int32)ADC0_countsPerVolt * 10;
    }
    else
    {
        countsPer10Volt = ADC0_countsPer10Volt;
    }

    /* Subtract ADC offset */
    adcCounts -= ADC0_offset;
    /* To convert adcCounts to microVolts it is required to be multiplied
    *  on 10 million and later divide on gain in counts per 10V.
    */
    uVolts = (( (int64)adcCounts * ADC0_10UV_COUNTS ) / countsPer10Volt);

    return((int32) uVolts );
}


/*******************************************************************************
* Function Name: ADC0_CountsTo_Volts
********************************************************************************
*
* Summary:
*  Converts the ADC output to volts as a floating-point number.
*
* Parameters:
*  int16 adcCounts: Result from the ADC conversion
*
* Return:
*  float Result in Volts
*
* Global Variables:
*  ADC0_offset variable used.
*  ADC0_countsPer10Volt used to convert ADC counts to Volts.
*
*******************************************************************************/
float32 ADC0_CountsTo_Volts(int16 adcCounts)
{
    float32 volts;
    int32 countsPer10Volt;

    if(ADC0_countsPerVolt != 0)
    {   /* Support obsolete method */
        countsPer10Volt = (int32)ADC0_countsPerVolt * 10;
    }
    else
    {
        countsPer10Volt = ADC0_countsPer10Volt;
    }

    /* Subtract ADC offset */
    adcCounts -= ADC0_offset;

    volts = ((float32)adcCounts * ADC0_10V_COUNTS) / (float32)countsPer10Volt;

    return( volts );
}


/* [] END OF FILE */
