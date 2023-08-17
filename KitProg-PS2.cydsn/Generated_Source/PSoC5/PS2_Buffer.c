/*******************************************************************************
* File Name: PS2_Buffer.c
* Version 2.30
*
* Description:
*  This file provides the API source code for the Shift Register component.
*
* Note: none
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "PS2_Buffer.h"

uint8 PS2_Buffer_initVar = 0u;


/*******************************************************************************
* Function Name: PS2_Buffer_Start
********************************************************************************
*
* Summary:
*  Starts the Shift Register.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  PS2_Buffer_initVar - used to check initial configuration, modified on
*  first function call.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PS2_Buffer_Start(void) 
{
    if(0u == PS2_Buffer_initVar)
    {
        PS2_Buffer_Init();
        PS2_Buffer_initVar = 1u; /* Component initialized */
    }

    PS2_Buffer_Enable();
}


/*******************************************************************************
* Function Name: PS2_Buffer_Enable
********************************************************************************
*
* Summary:
*  Enables the Shift Register.
*
* Parameters:
*  void.
*
* Return:
*  void.
*
*******************************************************************************/
void PS2_Buffer_Enable(void) 
{
    /* Changing address in Datapath Control Store
       from NOP to component state machine commands space */
    PS2_Buffer_SR_CONTROL |= PS2_Buffer_CLK_EN;

    PS2_Buffer_EnableInt();
}


/*******************************************************************************
* Function Name: PS2_Buffer_Init
********************************************************************************
*
* Summary:
*  Initializes Tx and/or Rx interrupt sources with initial values.
*
* Parameters:
*  void.
*
* Return:
*  void.
*
*******************************************************************************/
void PS2_Buffer_Init(void) 
{
    PS2_Buffer_SetIntMode(PS2_Buffer_INT_SRC);
}


/*******************************************************************************
* Function Name: PS2_Buffer_Stop
********************************************************************************
*
* Summary:
*  Disables the Shift Register
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PS2_Buffer_Stop(void) 
{
    /*changing Datapath Control Store address to NOP space*/
    PS2_Buffer_SR_CONTROL &= ((uint8) ~PS2_Buffer_CLK_EN);
    PS2_Buffer_DisableInt();
}


/*******************************************************************************
* Function Name: PS2_Buffer_EnableInt
********************************************************************************
*
* Summary:
*  Enables the Shift Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PS2_Buffer_EnableInt(void) 
{
    uint8 interruptState;

    interruptState = CyEnterCriticalSection();
    PS2_Buffer_SR_AUX_CONTROL |= PS2_Buffer_INTERRUPTS_ENABLE;
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: PS2_Buffer_DisableInt
********************************************************************************
*
* Summary:
*  Disables the Shift Register interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PS2_Buffer_DisableInt(void) 
{
    uint8 interruptState;

    interruptState = CyEnterCriticalSection();
    PS2_Buffer_SR_AUX_CONTROL &= ((uint8) ~PS2_Buffer_INTERRUPTS_ENABLE);
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: PS2_Buffer_GetFIFOStatus
********************************************************************************
*
* Summary:
*  Returns current status of input or output FIFO.
*
* Parameters:
*  fifoId.
*
* Return:
*  FIFO status.
*
*******************************************************************************/
uint8 PS2_Buffer_GetFIFOStatus(uint8 fifoId) 
{
    uint8 result;

    result = PS2_Buffer_RET_FIFO_NOT_DEFINED;

    #if(0u != PS2_Buffer_USE_INPUT_FIFO)
        if(PS2_Buffer_IN_FIFO == fifoId)
        {
            switch(PS2_Buffer_GET_IN_FIFO_STS)
            {
                case PS2_Buffer_IN_FIFO_FULL :
                    result = PS2_Buffer_RET_FIFO_FULL;
                    break;

                case PS2_Buffer_IN_FIFO_EMPTY :
                    result = PS2_Buffer_RET_FIFO_EMPTY;
                    break;

                case PS2_Buffer_IN_FIFO_PARTIAL:
                    result = PS2_Buffer_RET_FIFO_PARTIAL;
                    break;
                    
                default:
                    /* Initial result value, while 
                       IN_FIFO_EMPTY case is false 
                     */
                    result = PS2_Buffer_RET_FIFO_EMPTY;
                    break;
            }   
        }
    #endif /* (0u != PS2_Buffer_USE_INPUT_FIFO) */

    if(PS2_Buffer_OUT_FIFO == fifoId)
    {
        switch(PS2_Buffer_GET_OUT_FIFO_STS)
        {
            case PS2_Buffer_OUT_FIFO_FULL :
                result = PS2_Buffer_RET_FIFO_FULL;
                break;

            case PS2_Buffer_OUT_FIFO_EMPTY :
                result = PS2_Buffer_RET_FIFO_EMPTY;
                break;

            case PS2_Buffer_OUT_FIFO_PARTIAL :
                result = PS2_Buffer_RET_FIFO_PARTIAL;
                break;

            default:
                /* Initial result value, while 
                   OUT_FIFO_FULL case is false 
                 */
                result = PS2_Buffer_RET_FIFO_FULL;
                break;
        }
    }

    return(result);
}


/*******************************************************************************
* Function Name: PS2_Buffer_SetIntMode
********************************************************************************
*
* Summary:
*  Sets the Interrupt Source for the Shift Register interrupt. Multiple
*  sources may be ORed together
*
* Parameters:
*  interruptSource: Byte containing the constant for the selected interrupt
*  source/s.
*
* Return:
*  None.
*
*******************************************************************************/
void PS2_Buffer_SetIntMode(uint8 interruptSource) 
{
    PS2_Buffer_SR_STATUS_MASK &= ((uint8) ~PS2_Buffer_INTS_EN_MASK);          /* Clear existing int */
    PS2_Buffer_SR_STATUS_MASK |= (interruptSource & PS2_Buffer_INTS_EN_MASK); /* Set int */
}


/*******************************************************************************
* Function Name: PS2_Buffer_GetIntStatus
********************************************************************************
*
* Summary:
*  Gets the Shift Register Interrupt status.
*
* Parameters:
*  None.
*
* Return:
*  Byte containing the constant for the selected interrupt source/s.
*
*******************************************************************************/
uint8 PS2_Buffer_GetIntStatus(void) 
{
    return(PS2_Buffer_SR_STATUS & PS2_Buffer_INTS_EN_MASK);
}


/*******************************************************************************
* Function Name: PS2_Buffer_WriteRegValue
********************************************************************************
*
* Summary:
*  Send state directly to shift register
*
* Parameters:
*  shiftData: containing shift register state.
*
* Return:
*  None.
*
*******************************************************************************/
void PS2_Buffer_WriteRegValue(uint16 shiftData)
                                                                     
{
    CY_SET_REG16(PS2_Buffer_SHIFT_REG_LSB_PTR, shiftData);
}


#if(0u != PS2_Buffer_USE_INPUT_FIFO)
    /*******************************************************************************
    * Function Name: PS2_Buffer_WriteData
    ********************************************************************************
    *
    * Summary:
    *  Send state to FIFO for later transfer to shift register based on the Load
    *  input
    *
    * Parameters:
    *  shiftData: containing shift register state.
    *
    * Return:
    *  Indicates: successful execution of function
    *  when FIFO is empty; and error when FIFO is full.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    cystatus PS2_Buffer_WriteData(uint16 shiftData)
                                                                         
    {
        cystatus result;

        result = CYRET_INVALID_STATE;

        /* Writes data into the input FIFO if it is not FULL */
        if(PS2_Buffer_RET_FIFO_FULL != (PS2_Buffer_GetFIFOStatus(PS2_Buffer_IN_FIFO)))
        {
            CY_SET_REG16(PS2_Buffer_IN_FIFO_VAL_LSB_PTR, shiftData);
            result = CYRET_SUCCESS;
        }

        return(result);
    }
#endif /* (0u != PS2_Buffer_USE_INPUT_FIFO) */


#if(0u != PS2_Buffer_USE_OUTPUT_FIFO)
    /*******************************************************************************
    * Function Name: PS2_Buffer_ReadData
    ********************************************************************************
    *
    * Summary:
    *  Returns state in FIFO due to Store input.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Shift Register state
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 PS2_Buffer_ReadData(void) 
    {
        return(CY_GET_REG16(PS2_Buffer_OUT_FIFO_VAL_LSB_PTR));
    }
#endif /* (0u != PS2_Buffer_USE_OUTPUT_FIFO) */


/*******************************************************************************
* Function Name: PS2_Buffer_ReadRegValue
********************************************************************************
*
* Summary:
*  Directly returns current state in shift register, not data in FIFO due
*  to Store input.
*
* Parameters:
*  None.
*
* Return:
*  Shift Register state. Clears output FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint16 PS2_Buffer_ReadRegValue(void) 
{
    uint16 result;

    /* Clear FIFO before software capture */
    while(PS2_Buffer_RET_FIFO_EMPTY != PS2_Buffer_GetFIFOStatus(PS2_Buffer_OUT_FIFO))
    {
        (void) CY_GET_REG16(PS2_Buffer_OUT_FIFO_VAL_LSB_PTR);
    }

    /* Read of 8 bits from A1 causes capture to output FIFO */
    (void) CY_GET_REG8(PS2_Buffer_SHIFT_REG_CAPTURE_PTR);

    /* Read output FIFO */
    result  = CY_GET_REG16(PS2_Buffer_OUT_FIFO_VAL_LSB_PTR);
    
    #if(0u != (PS2_Buffer_SR_SIZE % 8u))
        result &= ((uint16) PS2_Buffer_SR_MASK);
    #endif /* (0u != (PS2_Buffer_SR_SIZE % 8u)) */
    
    return(result);
}


/* [] END OF FILE */
