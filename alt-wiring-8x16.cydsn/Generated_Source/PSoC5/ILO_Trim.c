/*******************************************************************************
* File Name: ILO_Trim.c
* Version 2.0
*
* Description:
*  This file contains the setup, control and status commands for the 
*  ILO_Trim component. It also contains the ISR for the trimming operation.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "ILO_Trim.h"
#include "cyapicallbacks.h"

static uint16 ILO_Trim_ReadCapture(void) CYREENTRANT;
static uint16 ILO_Trim_ReadEdgeCount(void) CYREENTRANT;

#if (!CY_PSOC4)
	static uint8 trimFactory;
#endif /* End of PSoC 3 and PSoC 5LP code */

#if(!CY_PSOC4)


/*******************************************************************************
* Interrupt Name: ILO_Trim_CorrectionISR
********************************************************************************
*
* Summary: 
*  If enabled, this CorrectionISR is executed for every cycle of the ILO. It 
*  reads the counter capture register to find out how many Reference edges were
*  counted per cycle of the ILO. Then, the CorrectionISR checks whether this 
*  value is within the acceptable range. If yes, it disables itself. If not, 
*  it corrects the ILO frequency by unit trim - either coarse or fine, and exits
*  the routine.
*
* Parameters:
*  None
*
* Side Effects: 
*  1) Turns off CorrectionISR if ILO is trimmed.
*  2) Reads counter capture register directly
*  3) Changes ILO frequency
*
*******************************************************************************/
CY_ISR(ILO_Trim_CorrectionIteration)
{
    uint16 edgeCount, errorNewAbs;
    int16 errorNew;
    uint8 trimNew;
    static uint16 errorOld = ILO_Trim_ERROR_DEFAULT;
    static uint8 trimOld, startDelay = ILO_Trim_INIT_DELAY;
    static uint8 iteration = ILO_Trim_ISR_INVALID;

    #ifdef ILO_Trim_CORRECTION_ITERATION_ENTRY_CALLBACK
        ILO_Trim_CorrectionIteration_EntryCallback();
    #endif /* ILO_Trim_CORRECTION_ITERATION_ENTRY_CALLBACK */
    
    if(iteration == ILO_Trim_ISR_INVALID)
    {
        if(startDelay == ILO_Trim_FINAL_DELAY)
        {
            iteration = ILO_Trim_ISR_VALID;
        }
        else
        {
            /* Give some time once the component has started */
            startDelay += ILO_Trim_DELAY_COUNT;
        }
    }
    else
    {
        iteration = ILO_Trim_ISR_INVALID;
        
        edgeCount = ILO_Trim_ReadEdgeCount();
            
        if(edgeCount != ILO_Trim_COUNT_ZERO)
        {
            errorNew = (int16)((uint16)((ILO_Trim_EDGES_PPT / edgeCount) - ILO_Trim_PP_THOUSAND));            
        }
        else
        {
            errorNew = (int16)ILO_Trim_COUNT_ZERO;
        }
    
        #if (ILO_Trim_ILO_FREQUENCY == ILO_Trim_1kHz)
            /* 1k ILO fine trim - TR0[3:0] reserved for 1kHz ILO */
            trimNew = (uint8)((ILO_Trim_ILO_TR0_MASK & CY_GET_REG8(CYREG_ILO_TR0))
                        | (uint8)((ILO_Trim_ILO_TR1_MASK & CY_GET_REG8(CYREG_ILO_TR1))
                            << ILO_Trim_TRIM_SHIFT));
        #else
            /* 100k ILO fine trim - TR0[7:4] reserved for 100kHz ILO */
            trimNew = (uint8)((uint8)((ILO_Trim_ILO_TR0_MASK & CY_GET_REG8(CYREG_ILO_TR0))
                            >>ILO_Trim_TRIM_SHIFT)
                        | (uint8)((ILO_Trim_ILO_TR1_MASK & CY_GET_REG8(CYREG_ILO_TR1))
                            << ILO_Trim_TRIM_SHIFT));
        #endif /* End of ILO Frequency dependent code. */
    
        if(errorNew < ILO_Trim_ERROR_ZERO)
        {
            errorNewAbs = (uint16)(-errorNew);
        }
        else
        {
             errorNewAbs = (uint16)errorNew;
        }
        
        if(errorNewAbs > errorOld)
        {
            if(errorOld <= ILO_Trim_PP_HUNDRED)
            {
                /* Revert back to the previous Trim */
                trimNew = trimOld;
        
                /* Stop the CorrectionISR but not the Timer if the ILO frequency is stable */
                ILO_Trim_CorrectionISR_Disable();
            }
            else
            {
                /* Start all over again from another coarse trim value */                
                if((trimNew & ILO_Trim_FINE_TRIM_MASK)!=ILO_Trim_TRIM_COARSE_NEXT)
                {
                    trimNew += ILO_Trim_COARSE_TRIM_ADD; 
                }
                else
                {
                    trimNew += ILO_Trim_FINE_TRIM_ADD; 
                }
            }
            /* Reinitialize the errorOld memory at the end of function */
            errorNewAbs = ILO_Trim_ERROR_DEFAULT;
        }
        else
        {
            /* Keep a copy of the old trim setting */
            trimOld = trimNew;
    
            if(edgeCount < (uint16)ILO_Trim_EDGES_IDEAL)
            {
                trimNew -= ILO_Trim_CORRECTION_STEP_SIZE;
            }
            else 
            {
                trimNew += ILO_Trim_CORRECTION_STEP_SIZE;
            }
        }

        #if (ILO_Trim_ILO_FREQUENCY == ILO_Trim_1kHz)
            CY_SET_REG8(CYREG_ILO_TR0, 
                (uint8)(CY_GET_REG8(CYREG_ILO_TR0) & ILO_Trim_FINE_TRIM_CLEAR)
                | (trimNew & ILO_Trim_ILO_TR0_MASK));
            CY_SET_REG8(CYREG_ILO_TR1, 
                (uint8)(CY_GET_REG8(CYREG_ILO_TR1) & ILO_Trim_COARSE_TRIM_CLEAR)
                | ((uint8)((uint8)trimNew >> ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR1_MASK));
        #else
            CY_SET_REG8(CYREG_ILO_TR0, 
                (uint8)(CY_GET_REG8(CYREG_ILO_TR0) & ILO_Trim_FINE_TRIM_CLEAR)
                | ((uint8)((uint8)trimNew << ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR0_MASK));
            CY_SET_REG8(CYREG_ILO_TR1, 
                (uint8)(CY_GET_REG8(CYREG_ILO_TR1) & ILO_Trim_COARSE_TRIM_CLEAR)
                | ((uint8)((uint8)trimNew >> ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR1_MASK));
        #endif /* End of ILO Frequency dependent code */
        
        /* Keep a copy of the old error */
        errorOld = errorNewAbs;
    }
    
    #ifdef ILO_Trim_CORRECTION_ITERATION_EXIT_CALLBACK
        ILO_Trim_CorrectionIteration_ExitCallback();
    #endif /* ILO_Trim_CORRECTION_ITERATION_EXIT_CALLBACK */
}/* End CorrectionISR */


/*******************************************************************************
* Function Name: ILO_Trim_CheckStatus
********************************************************************************
*
* Summary: 
*  Checks whether the ILO frequency is in the best possible error range.
*
* Parameters:
*  None
*
* Return: 
*  uint8 status
*   IS_ACCURATE     ILO is within error range
*   TRIMMING        CorrectionISR is running 
*
* Side Effects: 
*  May not return correct value when called in the first 
*  millisecond of component start.
*
*******************************************************************************/
uint8 ILO_Trim_CheckStatus(void) 
{
	uint16 errorAbs;
    int16 error;
    uint8 isrOn;
	uint8 status = ILO_Trim_TRIMMED;

	isrOn = ILO_Trim_CorrectionISR_GetState();
	
	/* Change status for CorrectionISR */
	if(isrOn!=ILO_Trim_ISR_OFF)
    {
		status = ILO_Trim_TRIMMING;
	}
	
    error = ILO_Trim_CheckError();
    
    if(error < ILO_Trim_ERROR_ZERO)
    {
        errorAbs = (uint16)(-error);
    }
    else
    {
         errorAbs = (uint16)error;
    }

	if(errorAbs <= ILO_Trim_PP_HUNDRED)
	{
		status |= ILO_Trim_IS_ACCURATE;
	}
    
	return(status);
}

#endif /* End of PSoC 3 and PSoC 5LP code */


/*******************************************************************************
* Function Name: ILO_Trim_CheckError
********************************************************************************
*
* Summary: 
*  Calculates ILO error in parts per thousand.
*
* Parameters:
*  None
*
* Return: 
*  int16 error
*   error value in parts per thousand.
*
* Side Effects:
*  May not return correct value when called in the first 
*  millisecond of component start.
*
*******************************************************************************/
int16 ILO_Trim_CheckError(void) 
{
	uint16 edgeCount;
	int16 error;

	edgeCount = ILO_Trim_ReadEdgeCount();
	
    if(edgeCount != ILO_Trim_COUNT_ZERO)
	{
        error = (int16)((uint16)((ILO_Trim_EDGES_PPT / edgeCount) - ILO_Trim_PP_THOUSAND));
    }
    else
    {
        error = (int16)ILO_Trim_COUNT_ZERO;
    }
	
	return(error);
}


/*******************************************************************************
* Function Name: ILO_Trim_Compensate
********************************************************************************
*
* Summary: 
*  Compensates for the ILO clock inaccuracy by converting from a desired 
*  nominal number of clock cycles to the effective number of ILO clock cycles 
*  required based on the current accuracy of the ILO.  
*
* Parameters:
*  targetTime
*
* Return: 
*  uint16 scaleFactor
*
* Side Effects: 
*  None
*
*******************************************************************************/
uint16 ILO_Trim_Compensate(uint16 clocks) 
{
	uint32 timeDesired, ILOActual, clocksActual;
	uint16 edgeCount, clocksOut;
	int16 error;

	/* Calculate the user's target time */
	timeDesired = (uint32)(((uint32)clocks * ILO_Trim_TIME_SCALE) / ILO_Trim_ILO_FREQUENCY);
	
	/* Calculate the ILO error */
	edgeCount = ILO_Trim_ReadEdgeCount();
	if(edgeCount != ILO_Trim_COUNT_ZERO)
	{
        /* ILO error is proportional to the difference of the inverse of edge count and EDGES_IDEAL */
        error = (int16)((uint16)((ILO_Trim_EDGES_PPH / edgeCount) - ILO_Trim_PP_HUNDRED));
    }
    else
    {
        error = (int16)ILO_Trim_COUNT_ZERO;
    }
    
	ILOActual = ((uint32)ILO_Trim_ILO_FREQUENCY * ILO_Trim_ILO_SCALE)
                + ((uint32)ILO_Trim_ILO_FREQUENCY * (uint32)error);
	
	/* Calculate the scaled clock count */
	clocksActual = ILOActual * (timeDesired / ILO_Trim_ILO_SCALE);
	
	if((clocksActual / ILO_Trim_TIME_SCALE) > ILO_Trim_COUNTER_SATURATE)
	{
		clocksOut = ILO_Trim_COUNTER_SATURATE;
	}
	else
	{
		clocksOut = (uint16)(clocksActual / ILO_Trim_TIME_SCALE);
	}
	
	return(clocksOut);
}


/*******************************************************************************
* Function Name: ILO_Trim_Start
********************************************************************************
*
* Summary:
*  Enables the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side effects: 
*  None
*
*******************************************************************************/
void ILO_Trim_Start(void) 
{
    #if (!CY_PSOC4)
	    ILO_Trim_CorrectionISR_StartEx(&ILO_Trim_CorrectionIteration);
    #endif /* Only available for PSoC 3 and PSoC 5LP */
	
    ILO_Trim_CorrectionISR_Disable();	
	
	/* Store away the factory trim value */
	#if (!CY_PSOC4)
        #if (ILO_Trim_ILO_FREQUENCY == ILO_Trim_1kHz)
            /* 1k ILO fine trim - TR0[3:0] reserved for 1kHz ILO */
            trimFactory = (uint8)((ILO_Trim_ILO_TR0_MASK & CY_GET_REG8(CYREG_ILO_TR0))
                        |(uint8)((ILO_Trim_ILO_TR1_MASK & CY_GET_REG8(CYREG_ILO_TR1))
                            << ILO_Trim_TRIM_SHIFT));
        #else
            /* 100k ILO fine trim - TR0[7:4] reserved for 100kHz ILO */
            trimFactory = (uint8)((uint8)((ILO_Trim_ILO_TR0_MASK & CY_GET_REG8(CYREG_ILO_TR0))
                            >>ILO_Trim_TRIM_SHIFT)
                        |(uint8)((ILO_Trim_ILO_TR1_MASK & CY_GET_REG8(CYREG_ILO_TR1))
                            << ILO_Trim_TRIM_SHIFT));
        #endif /* End of ILO Frequency dependent code. */
	#endif /* End of PSoC 3 and PSoC 5LP code */
	
	/* Start Edge_Counter */
	#if (!ILO_Trim_USING_FIXED_FUNCTION)
		/* Enable UDB Timer */
		ILO_Trim_CONTROL_REG |= ILO_Trim_CTRL_ENABLE;
	#else
		#if (CY_PSOC4)
			ILO_Trim_TCPWM_Start();
			ILO_Trim_TCPWM_TriggerCommand(ILO_Trim_TCPWM_MASK, ILO_Trim_TCPWM_CMD_START); 
		#else
			/* Enable FF Timer */
			/* Clear all bits but the enable bit (if it's already set) for Timer operation */
		    ILO_Trim_CONTROL_REG &= ILO_Trim_CTRL_ENABLE;
			
			/* CONTROL3 register exists only in PSoC3 OR PSoC5LP */
		    ILO_Trim_CONTROL3_REG &= ((uint8)(~ILO_Trim_CTRL_MODE_MASK));

	        /*Enable DSI Sync on all inputs of the Timer*/
	        ILO_Trim_RT1_REG &= ((uint8)(~ILO_Trim_SYNCDSI_MASK));
	        ILO_Trim_RT1_REG |= ILO_Trim_SYNCDSI_EN;
		
		    /* Set Initial values from Configuration */
			CY_SET_REG16(ILO_Trim_PERIOD_LSB_PTR, ILO_Trim_INIT_PERIOD);
			CY_SET_REG16(ILO_Trim_COUNTER_LSB_PTR, ILO_Trim_INIT_PERIOD);
			
			ILO_Trim_GLOBAL_ENABLE |= ILO_Trim_BLOCK_EN_MASK;
	        ILO_Trim_GLOBAL_STBY_ENABLE |= ILO_Trim_BLOCK_STBY_EN_MASK;

			ILO_Trim_CONTROL_REG |= ILO_Trim_CTRL_ENABLE;
		#endif /* End of PSoC 3 and PSoC 5LP code */
	#endif /* End of Fixed Function code */
}


/*******************************************************************************
* Function Name: ILO_Trim_Stop
********************************************************************************
*
* Summary:
*  Disables the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side effects:
*  None
*
*******************************************************************************/
void ILO_Trim_Stop(void) 
{
	ILO_Trim_CorrectionISR_Disable();
	ILO_Trim_CorrectionISR_ClearPending();
	
    #if (!ILO_Trim_USING_FIXED_FUNCTION)
		/* Stop the Edge_Counter */
		ILO_Trim_CONTROL_REG &= ((uint8) ~ILO_Trim_CTRL_ENABLE);
	#else
		#if (CY_PSOC4)
			ILO_Trim_TCPWM_Stop();
		#else
			/* Disable Timer */
		    ILO_Trim_CONTROL_REG &= ((uint8)(~ILO_Trim_CTRL_ENABLE));
			
	        ILO_Trim_GLOBAL_ENABLE &= ((uint8)(~ILO_Trim_BLOCK_EN_MASK));
	        ILO_Trim_GLOBAL_STBY_ENABLE &= ((uint8)(~ILO_Trim_BLOCK_STBY_EN_MASK));
		#endif /* End of PSoC 3 and PSoC 5LP code */
    #endif /* End of Fixed Function code */
}

#if(!CY_PSOC4)


/*******************************************************************************
* Function Name: ILO_Trim_BeginTrimming
********************************************************************************
*
* Summary:
*  Sets and starts the CorrectionISR vector. This causes the trimming 
*  process to begin.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side effects: 
*  None
*
*******************************************************************************/
void ILO_Trim_BeginTrimming(void) 
{
    uint8 isrOn;
    
    isrOn = ILO_Trim_CorrectionISR_GetState();
    if(isrOn == ILO_Trim_ISR_OFF)
    {
        ILO_Trim_CorrectionISR_Enable();
    }
}


/*******************************************************************************
* Function Name: ILO_Trim_StopTrimming
********************************************************************************
*
* Summary:
*  Disables the CorrectionISR vector, clears pedning interrupts. 
*  This causes the trimming process to stop.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects: 
*  Turns the isr off
*
*******************************************************************************/
void ILO_Trim_StopTrimming(void) 
{
    uint8 isrOn;
    
    isrOn = ILO_Trim_CorrectionISR_GetState();
    if(isrOn != ILO_Trim_ISR_OFF)
    {
        ILO_Trim_CorrectionISR_Disable();
        ILO_Trim_CorrectionISR_ClearPending();
    }
}


/*******************************************************************************
* Function Name: ILO_Trim_RestoreTrim
********************************************************************************
*
* Summary: 
*  Restores ILO factory trim.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects: 
*  Turns off the isr.
*
*******************************************************************************/
void ILO_Trim_RestoreTrim(void) 
{
	ILO_Trim_CorrectionISR_ClearPending();
	ILO_Trim_CorrectionISR_Disable();
	
    #if (ILO_Trim_ILO_FREQUENCY == ILO_Trim_1kHz)
        CY_SET_REG8(CYREG_ILO_TR0, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR0) & ILO_Trim_FINE_TRIM_CLEAR)
            | (trimFactory & ILO_Trim_ILO_TR0_MASK));
        CY_SET_REG8(CYREG_ILO_TR1, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR1) & ILO_Trim_COARSE_TRIM_CLEAR)
            | ((uint8)((uint8)trimFactory >> ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR1_MASK));
    #else
        CY_SET_REG8(CYREG_ILO_TR0, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR0) & ILO_Trim_FINE_TRIM_CLEAR)
            | ((uint8)((uint8)trimFactory << ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR0_MASK));
        CY_SET_REG8(CYREG_ILO_TR1, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR1) & ILO_Trim_COARSE_TRIM_CLEAR)
            | ((uint8)((uint8)trimFactory >> ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR1_MASK));
    #endif /* End of ILO Frequency dependent code */
}


/*******************************************************************************
* Function Name: ILO_Trim_GetTrim
********************************************************************************
*
* Summary:
*  Returns the current ILO trim value
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
uint8 ILO_Trim_GetTrim(void) 
{
	uint8 trimVal;
	
	#if (ILO_Trim_ILO_FREQUENCY == ILO_Trim_1kHz)
		/* 1k ILO fine trim - TR0[3:0] reserved for 1kHz ILO */
		trimVal = (uint8)((ILO_Trim_ILO_TR0_MASK & CY_GET_REG8(CYREG_ILO_TR0))
                    | (uint8)((uint8)(ILO_Trim_ILO_TR1_MASK & CY_GET_REG8(CYREG_ILO_TR1)) 
                    << ILO_Trim_TRIM_SHIFT));
	#else
		/* 100k ILO fine trim - TR0[7:4] reserved for 100kHz ILO */
		trimVal = (uint8)((uint8)((uint8)(ILO_Trim_ILO_TR0_MASK & CY_GET_REG8(CYREG_ILO_TR0))
                    >> ILO_Trim_TRIM_SHIFT)
                    | (uint8)((uint8)(ILO_Trim_ILO_TR1_MASK & CY_GET_REG8(CYREG_ILO_TR1)) 
                    << ILO_Trim_TRIM_SHIFT));
	#endif /* End of ILO Frequency dependent code */
	
	return trimVal;
}


/*******************************************************************************
* Function Name: ILO_Trim_SetTrim
********************************************************************************
*
* Summary:
*  Sets the ILO trim value
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
void ILO_Trim_SetTrim(uint8 trimVal) 
{
    #if (ILO_Trim_ILO_FREQUENCY == ILO_Trim_1kHz)
        CY_SET_REG8(CYREG_ILO_TR0, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR0) & ILO_Trim_FINE_TRIM_CLEAR)
            | (trimVal & ILO_Trim_ILO_TR0_MASK));
        CY_SET_REG8(CYREG_ILO_TR1, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR1) & ILO_Trim_COARSE_TRIM_CLEAR)
            | ((uint8)((uint8)trimVal >> ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR1_MASK));
    #else
        CY_SET_REG8(CYREG_ILO_TR0, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR0) & ILO_Trim_FINE_TRIM_CLEAR)
            | ((uint8)((uint8)trimVal << ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR0_MASK));
        CY_SET_REG8(CYREG_ILO_TR1, 
            (uint8)(CY_GET_REG8(CYREG_ILO_TR1) & ILO_Trim_COARSE_TRIM_CLEAR)
            | ((uint8)((uint8)trimVal >> ILO_Trim_TRIM_SHIFT) & ILO_Trim_ILO_TR1_MASK));
    #endif /* End of ILO Frequency dependent code */
}

#endif /* End of PSoC 3 and PSoC 5LP code */
/* End of non-PSoC 4 code */


/******************************************************************************
* Function Name: ILO_Trim_ReadCapture
*******************************************************************************
* Summary:
*  Retrieves the value held in the capture register.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static uint16 ILO_Trim_ReadCapture(void) CYREENTRANT
{
	uint16 captured;
    /* Get the captured counter value */
	#if (!ILO_Trim_USING_FIXED_FUNCTION)
        captured = CY_GET_REG8(ILO_Trim_CAPTURE_PTR);
	#else
		#if (CY_PSOC4)
			captured = (uint16)ILO_Trim_TCPWM_ReadCapture();
		#else
			captured = CY_GET_REG16(ILO_Trim_CAPTURE_LSB_PTR);
		#endif /* End of PSoC 3 and PSoC 5LP code */
	#endif /* End of Fixed Function code */
	
	return captured;
}


/******************************************************************************
* Function Name: ILO_Trim_ReadEdgeCount
*******************************************************************************
* Summary:
*  Calculates the edge count
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static uint16 ILO_Trim_ReadEdgeCount(void) CYREENTRANT
{
	uint16 edgecount, captureOld, captureNew;
	
	captureOld = ILO_Trim_ReadCapture();
	captureNew = ILO_Trim_ReadCapture();
	
	/* Make sure that the read value is correct (perhaps read on transition) */
	while (captureOld != captureNew)
	{	
		captureNew = ILO_Trim_ReadCapture();
		captureOld = ILO_Trim_ReadCapture();
	}
	
	/* Make the captured value into a real count number. Then add the offset term.    */
	/* The offsets are different for PSoC 4 */
    #if(CY_PSOC4)
        edgecount = (ILO_Trim_RESOLUTION - captureNew) - ILO_Trim_TIMER_OFFSET;
    #else
	    edgecount = (ILO_Trim_RESOLUTION - captureNew) + ILO_Trim_TIMER_OFFSET;
	#endif /* End of PSoC 3 and PSoC 5LP code */
    
	return edgecount;
}

/* [] END OF FILE */
