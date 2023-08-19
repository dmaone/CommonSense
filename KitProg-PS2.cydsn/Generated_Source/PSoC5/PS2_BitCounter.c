/*******************************************************************************
* File Name: PS2_BitCounter.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "PS2_BitCounter.h"

uint8 PS2_BitCounter_initVar = 0u;


/*******************************************************************************
* Function Name: PS2_BitCounter_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void PS2_BitCounter_Init(void) 
{
        #if (!PS2_BitCounter_UsingFixedFunction && !PS2_BitCounter_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!PS2_BitCounter_UsingFixedFunction && !PS2_BitCounter_ControlRegRemoved) */
        
        #if(!PS2_BitCounter_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 PS2_BitCounter_interruptState;
        #endif /* (!PS2_BitCounter_UsingFixedFunction) */
        
        #if (PS2_BitCounter_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            PS2_BitCounter_CONTROL &= PS2_BitCounter_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                PS2_BitCounter_CONTROL2 &= ((uint8)(~PS2_BitCounter_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                PS2_BitCounter_CONTROL3 &= ((uint8)(~PS2_BitCounter_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (PS2_BitCounter_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                PS2_BitCounter_CONTROL |= PS2_BitCounter_ONESHOT;
            #endif /* (PS2_BitCounter_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            PS2_BitCounter_CONTROL2 |= PS2_BitCounter_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            PS2_BitCounter_RT1 &= ((uint8)(~PS2_BitCounter_RT1_MASK));
            PS2_BitCounter_RT1 |= PS2_BitCounter_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            PS2_BitCounter_RT1 &= ((uint8)(~PS2_BitCounter_SYNCDSI_MASK));
            PS2_BitCounter_RT1 |= PS2_BitCounter_SYNCDSI_EN;

        #else
            #if(!PS2_BitCounter_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = PS2_BitCounter_CONTROL & ((uint8)(~PS2_BitCounter_CTRL_CMPMODE_MASK));
            PS2_BitCounter_CONTROL = ctrl | PS2_BitCounter_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = PS2_BitCounter_CONTROL & ((uint8)(~PS2_BitCounter_CTRL_CAPMODE_MASK));
            
            #if( 0 != PS2_BitCounter_CAPTURE_MODE_CONF)
                PS2_BitCounter_CONTROL = ctrl | PS2_BitCounter_DEFAULT_CAPTURE_MODE;
            #else
                PS2_BitCounter_CONTROL = ctrl;
            #endif /* 0 != PS2_BitCounter_CAPTURE_MODE */ 
            
            #endif /* (!PS2_BitCounter_ControlRegRemoved) */
        #endif /* (PS2_BitCounter_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!PS2_BitCounter_UsingFixedFunction)
            PS2_BitCounter_ClearFIFO();
        #endif /* (!PS2_BitCounter_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        PS2_BitCounter_WritePeriod(PS2_BitCounter_INIT_PERIOD_VALUE);
        #if (!(PS2_BitCounter_UsingFixedFunction && (CY_PSOC5A)))
            PS2_BitCounter_WriteCounter(PS2_BitCounter_INIT_COUNTER_VALUE);
        #endif /* (!(PS2_BitCounter_UsingFixedFunction && (CY_PSOC5A))) */
        PS2_BitCounter_SetInterruptMode(PS2_BitCounter_INIT_INTERRUPTS_MASK);
        
        #if (!PS2_BitCounter_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)PS2_BitCounter_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            PS2_BitCounter_WriteCompare(PS2_BitCounter_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            PS2_BitCounter_interruptState = CyEnterCriticalSection();
            
            PS2_BitCounter_STATUS_AUX_CTRL |= PS2_BitCounter_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(PS2_BitCounter_interruptState);
            
        #endif /* (!PS2_BitCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PS2_BitCounter_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void PS2_BitCounter_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (PS2_BitCounter_UsingFixedFunction)
        PS2_BitCounter_GLOBAL_ENABLE |= PS2_BitCounter_BLOCK_EN_MASK;
        PS2_BitCounter_GLOBAL_STBY_ENABLE |= PS2_BitCounter_BLOCK_STBY_EN_MASK;
    #endif /* (PS2_BitCounter_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!PS2_BitCounter_ControlRegRemoved || PS2_BitCounter_UsingFixedFunction)
        PS2_BitCounter_CONTROL |= PS2_BitCounter_CTRL_ENABLE;                
    #endif /* (!PS2_BitCounter_ControlRegRemoved || PS2_BitCounter_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: PS2_BitCounter_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  PS2_BitCounter_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void PS2_BitCounter_Start(void) 
{
    if(PS2_BitCounter_initVar == 0u)
    {
        PS2_BitCounter_Init();
        
        PS2_BitCounter_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    PS2_BitCounter_Enable();        
}


/*******************************************************************************
* Function Name: PS2_BitCounter_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void PS2_BitCounter_Stop(void) 
{
    /* Disable Counter */
    #if(!PS2_BitCounter_ControlRegRemoved || PS2_BitCounter_UsingFixedFunction)
        PS2_BitCounter_CONTROL &= ((uint8)(~PS2_BitCounter_CTRL_ENABLE));        
    #endif /* (!PS2_BitCounter_ControlRegRemoved || PS2_BitCounter_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (PS2_BitCounter_UsingFixedFunction)
        PS2_BitCounter_GLOBAL_ENABLE &= ((uint8)(~PS2_BitCounter_BLOCK_EN_MASK));
        PS2_BitCounter_GLOBAL_STBY_ENABLE &= ((uint8)(~PS2_BitCounter_BLOCK_STBY_EN_MASK));
    #endif /* (PS2_BitCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PS2_BitCounter_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void PS2_BitCounter_SetInterruptMode(uint8 interruptsMask) 
{
    PS2_BitCounter_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: PS2_BitCounter_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   PS2_BitCounter_ReadStatusRegister(void) 
{
    return PS2_BitCounter_STATUS;
}


#if(!PS2_BitCounter_ControlRegRemoved)
/*******************************************************************************
* Function Name: PS2_BitCounter_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   PS2_BitCounter_ReadControlRegister(void) 
{
    return PS2_BitCounter_CONTROL;
}


/*******************************************************************************
* Function Name: PS2_BitCounter_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    PS2_BitCounter_WriteControlRegister(uint8 control) 
{
    PS2_BitCounter_CONTROL = control;
}

#endif  /* (!PS2_BitCounter_ControlRegRemoved) */


#if (!(PS2_BitCounter_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: PS2_BitCounter_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void PS2_BitCounter_WriteCounter(uint8 counter) \
                                   
{
    #if(PS2_BitCounter_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (PS2_BitCounter_GLOBAL_ENABLE & PS2_BitCounter_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        PS2_BitCounter_GLOBAL_ENABLE |= PS2_BitCounter_BLOCK_EN_MASK;
        CY_SET_REG16(PS2_BitCounter_COUNTER_LSB_PTR, (uint16)counter);
        PS2_BitCounter_GLOBAL_ENABLE &= ((uint8)(~PS2_BitCounter_BLOCK_EN_MASK));
    #else
        CY_SET_REG8(PS2_BitCounter_COUNTER_LSB_PTR, counter);
    #endif /* (PS2_BitCounter_UsingFixedFunction) */
}
#endif /* (!(PS2_BitCounter_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: PS2_BitCounter_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint8) The present value of the counter.
*
*******************************************************************************/
uint8 PS2_BitCounter_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(PS2_BitCounter_UsingFixedFunction)
		(void)CY_GET_REG16(PS2_BitCounter_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(PS2_BitCounter_COUNTER_LSB_PTR_8BIT);
	#endif/* (PS2_BitCounter_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(PS2_BitCounter_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(PS2_BitCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG8(PS2_BitCounter_STATICCOUNT_LSB_PTR));
    #endif /* (PS2_BitCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PS2_BitCounter_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) Present Capture value.
*
*******************************************************************************/
uint8 PS2_BitCounter_ReadCapture(void) 
{
    #if(PS2_BitCounter_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(PS2_BitCounter_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG8(PS2_BitCounter_STATICCOUNT_LSB_PTR));
    #endif /* (PS2_BitCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PS2_BitCounter_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint8) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void PS2_BitCounter_WritePeriod(uint8 period) 
{
    #if(PS2_BitCounter_UsingFixedFunction)
        CY_SET_REG16(PS2_BitCounter_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG8(PS2_BitCounter_PERIOD_LSB_PTR, period);
    #endif /* (PS2_BitCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PS2_BitCounter_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint8) Present period value.
*
*******************************************************************************/
uint8 PS2_BitCounter_ReadPeriod(void) 
{
    #if(PS2_BitCounter_UsingFixedFunction)
        return ((uint8)CY_GET_REG16(PS2_BitCounter_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG8(PS2_BitCounter_PERIOD_LSB_PTR));
    #endif /* (PS2_BitCounter_UsingFixedFunction) */
}


#if (!PS2_BitCounter_UsingFixedFunction)
/*******************************************************************************
* Function Name: PS2_BitCounter_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void PS2_BitCounter_WriteCompare(uint8 compare) \
                                   
{
    #if(PS2_BitCounter_UsingFixedFunction)
        CY_SET_REG16(PS2_BitCounter_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG8(PS2_BitCounter_COMPARE_LSB_PTR, compare);
    #endif /* (PS2_BitCounter_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: PS2_BitCounter_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint8) Present compare value.
*
*******************************************************************************/
uint8 PS2_BitCounter_ReadCompare(void) 
{
    return (CY_GET_REG8(PS2_BitCounter_COMPARE_LSB_PTR));
}


#if (PS2_BitCounter_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: PS2_BitCounter_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void PS2_BitCounter_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    PS2_BitCounter_CONTROL &= ((uint8)(~PS2_BitCounter_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    PS2_BitCounter_CONTROL |= compareMode;
}
#endif  /* (PS2_BitCounter_COMPARE_MODE_SOFTWARE) */


#if (PS2_BitCounter_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: PS2_BitCounter_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void PS2_BitCounter_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    PS2_BitCounter_CONTROL &= ((uint8)(~PS2_BitCounter_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    PS2_BitCounter_CONTROL |= ((uint8)((uint8)captureMode << PS2_BitCounter_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (PS2_BitCounter_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: PS2_BitCounter_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void PS2_BitCounter_ClearFIFO(void) 
{

    while(0u != (PS2_BitCounter_ReadStatusRegister() & PS2_BitCounter_STATUS_FIFONEMP))
    {
        (void)PS2_BitCounter_ReadCapture();
    }

}
#endif  /* (!PS2_BitCounter_UsingFixedFunction) */


/* [] END OF FILE */

