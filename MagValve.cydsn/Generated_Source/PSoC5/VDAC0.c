/*******************************************************************************
* File Name: VDAC0.c  
* Version 1.90
*
* Description:
*  This file provides the source code to the API for the 8-bit Voltage DAC 
*  (VDAC8) User Module.
*
* Note:
*  Any unusual or non-standard behavior should be noted here. Other-
*  wise, this section should remain blank.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "VDAC0.h"

#if (CY_PSOC5A)
#include <CyLib.h>
#endif /* CY_PSOC5A */

uint8 VDAC0_initVar = 0u;

#if (CY_PSOC5A)
    static uint8 VDAC0_restoreVal = 0u;
#endif /* CY_PSOC5A */

#if (CY_PSOC5A)
    static VDAC0_backupStruct VDAC0_backup;
#endif /* CY_PSOC5A */


/*******************************************************************************
* Function Name: VDAC0_Init
********************************************************************************
* Summary:
*  Initialize to the schematic state.
* 
* Parameters:
*  void:
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void VDAC0_Init(void) 
{
    VDAC0_CR0 = (VDAC0_MODE_V );

    /* Set default data source */
    #if(VDAC0_DEFAULT_DATA_SRC != 0 )
        VDAC0_CR1 = (VDAC0_DEFAULT_CNTL | VDAC0_DACBUS_ENABLE) ;
    #else
        VDAC0_CR1 = (VDAC0_DEFAULT_CNTL | VDAC0_DACBUS_DISABLE) ;
    #endif /* (VDAC0_DEFAULT_DATA_SRC != 0 ) */

    /* Set default strobe mode */
    #if(VDAC0_DEFAULT_STRB != 0)
        VDAC0_Strobe |= VDAC0_STRB_EN ;
    #endif/* (VDAC0_DEFAULT_STRB != 0) */

    /* Set default range */
    VDAC0_SetRange(VDAC0_DEFAULT_RANGE); 

    /* Set default speed */
    VDAC0_SetSpeed(VDAC0_DEFAULT_SPEED);
}


/*******************************************************************************
* Function Name: VDAC0_Enable
********************************************************************************
* Summary:
*  Enable the VDAC8
* 
* Parameters:
*  void
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void VDAC0_Enable(void) 
{
    VDAC0_PWRMGR |= VDAC0_ACT_PWR_EN;
    VDAC0_STBY_PWRMGR |= VDAC0_STBY_PWR_EN;

    /*This is to restore the value of register CR0 ,
    which is modified  in Stop API , this prevents misbehaviour of VDAC */
    #if (CY_PSOC5A)
        if(VDAC0_restoreVal == 1u) 
        {
             VDAC0_CR0 = VDAC0_backup.data_value;
             VDAC0_restoreVal = 0u;
        }
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: VDAC0_Start
********************************************************************************
*
* Summary:
*  The start function initializes the voltage DAC with the default values, 
*  and sets the power to the given level.  A power level of 0, is the same as
*  executing the stop function.
*
* Parameters:
*  Power: Sets power level between off (0) and (3) high power
*
* Return:
*  void 
*
* Global variables:
*  VDAC0_initVar: Is modified when this function is called for the 
*  first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void VDAC0_Start(void)  
{
    /* Hardware initiazation only needs to occure the first time */
    if(VDAC0_initVar == 0u)
    { 
        VDAC0_Init();
        VDAC0_initVar = 1u;
    }

    /* Enable power to DAC */
    VDAC0_Enable();

    /* Set default value */
    VDAC0_SetValue(VDAC0_DEFAULT_DATA); 
}


/*******************************************************************************
* Function Name: VDAC0_Stop
********************************************************************************
*
* Summary:
*  Powers down DAC to lowest power state.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void VDAC0_Stop(void) 
{
    /* Disble power to DAC */
    VDAC0_PWRMGR &= (uint8)(~VDAC0_ACT_PWR_EN);
    VDAC0_STBY_PWRMGR &= (uint8)(~VDAC0_STBY_PWR_EN);

    /* This is a work around for PSoC5A  ,
    this sets VDAC to current mode with output off */
    #if (CY_PSOC5A)
        VDAC0_backup.data_value = VDAC0_CR0;
        VDAC0_CR0 = VDAC0_CUR_MODE_OUT_OFF;
        VDAC0_restoreVal = 1u;
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: VDAC0_SetSpeed
********************************************************************************
*
* Summary:
*  Set DAC speed
*
* Parameters:
*  power: Sets speed value
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void VDAC0_SetSpeed(uint8 speed) 
{
    /* Clear power mask then write in new value */
    VDAC0_CR0 &= (uint8)(~VDAC0_HS_MASK);
    VDAC0_CR0 |=  (speed & VDAC0_HS_MASK);
}


/*******************************************************************************
* Function Name: VDAC0_SetRange
********************************************************************************
*
* Summary:
*  Set one of three current ranges.
*
* Parameters:
*  Range: Sets one of Three valid ranges.
*
* Return:
*  void 
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void VDAC0_SetRange(uint8 range) 
{
    VDAC0_CR0 &= (uint8)(~VDAC0_RANGE_MASK);      /* Clear existing mode */
    VDAC0_CR0 |= (range & VDAC0_RANGE_MASK);      /*  Set Range  */
    VDAC0_DacTrim();
}


/*******************************************************************************
* Function Name: VDAC0_SetValue
********************************************************************************
*
* Summary:
*  Set 8-bit DAC value
*
* Parameters:  
*  value:  Sets DAC value between 0 and 255.
*
* Return: 
*  void 
*
* Theory: 
*
* Side Effects:
*
*******************************************************************************/
void VDAC0_SetValue(uint8 value) 
{
    #if (CY_PSOC5A)
        uint8 VDAC0_intrStatus = CyEnterCriticalSection();
    #endif /* CY_PSOC5A */

    VDAC0_Data = value;                /*  Set Value  */

    /* PSOC5A requires a double write */
    /* Exit Critical Section */
    #if (CY_PSOC5A)
        VDAC0_Data = value;
        CyExitCriticalSection(VDAC0_intrStatus);
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: VDAC0_DacTrim
********************************************************************************
*
* Summary:
*  Set the trim value for the given range.
*
* Parameters:
*  range:  1V or 4V range.  See constants.
*
* Return:
*  void
*
* Theory: 
*
* Side Effects:
*
*******************************************************************************/
void VDAC0_DacTrim(void) 
{
    uint8 mode;

    mode = (uint8)((VDAC0_CR0 & VDAC0_RANGE_MASK) >> 2) + VDAC0_TRIM_M7_1V_RNG_OFFSET;
    VDAC0_TR = CY_GET_XTND_REG8((uint8 *)(VDAC0_DAC_TRIM_BASE + mode));
}


/* [] END OF FILE */
