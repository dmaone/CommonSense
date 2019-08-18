/*******************************************************************************
* File Name: VDAC2.c  
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
#include "VDAC2.h"

#if (CY_PSOC5A)
#include <CyLib.h>
#endif /* CY_PSOC5A */

uint8 VDAC2_initVar = 0u;

#if (CY_PSOC5A)
    static uint8 VDAC2_restoreVal = 0u;
#endif /* CY_PSOC5A */

#if (CY_PSOC5A)
    static VDAC2_backupStruct VDAC2_backup;
#endif /* CY_PSOC5A */


/*******************************************************************************
* Function Name: VDAC2_Init
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
void VDAC2_Init(void) 
{
    VDAC2_CR0 = (VDAC2_MODE_V );

    /* Set default data source */
    #if(VDAC2_DEFAULT_DATA_SRC != 0 )
        VDAC2_CR1 = (VDAC2_DEFAULT_CNTL | VDAC2_DACBUS_ENABLE) ;
    #else
        VDAC2_CR1 = (VDAC2_DEFAULT_CNTL | VDAC2_DACBUS_DISABLE) ;
    #endif /* (VDAC2_DEFAULT_DATA_SRC != 0 ) */

    /* Set default strobe mode */
    #if(VDAC2_DEFAULT_STRB != 0)
        VDAC2_Strobe |= VDAC2_STRB_EN ;
    #endif/* (VDAC2_DEFAULT_STRB != 0) */

    /* Set default range */
    VDAC2_SetRange(VDAC2_DEFAULT_RANGE); 

    /* Set default speed */
    VDAC2_SetSpeed(VDAC2_DEFAULT_SPEED);
}


/*******************************************************************************
* Function Name: VDAC2_Enable
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
void VDAC2_Enable(void) 
{
    VDAC2_PWRMGR |= VDAC2_ACT_PWR_EN;
    VDAC2_STBY_PWRMGR |= VDAC2_STBY_PWR_EN;

    /*This is to restore the value of register CR0 ,
    which is modified  in Stop API , this prevents misbehaviour of VDAC */
    #if (CY_PSOC5A)
        if(VDAC2_restoreVal == 1u) 
        {
             VDAC2_CR0 = VDAC2_backup.data_value;
             VDAC2_restoreVal = 0u;
        }
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: VDAC2_Start
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
*  VDAC2_initVar: Is modified when this function is called for the 
*  first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void VDAC2_Start(void)  
{
    /* Hardware initiazation only needs to occure the first time */
    if(VDAC2_initVar == 0u)
    { 
        VDAC2_Init();
        VDAC2_initVar = 1u;
    }

    /* Enable power to DAC */
    VDAC2_Enable();

    /* Set default value */
    VDAC2_SetValue(VDAC2_DEFAULT_DATA); 
}


/*******************************************************************************
* Function Name: VDAC2_Stop
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
void VDAC2_Stop(void) 
{
    /* Disble power to DAC */
    VDAC2_PWRMGR &= (uint8)(~VDAC2_ACT_PWR_EN);
    VDAC2_STBY_PWRMGR &= (uint8)(~VDAC2_STBY_PWR_EN);

    /* This is a work around for PSoC5A  ,
    this sets VDAC to current mode with output off */
    #if (CY_PSOC5A)
        VDAC2_backup.data_value = VDAC2_CR0;
        VDAC2_CR0 = VDAC2_CUR_MODE_OUT_OFF;
        VDAC2_restoreVal = 1u;
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: VDAC2_SetSpeed
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
void VDAC2_SetSpeed(uint8 speed) 
{
    /* Clear power mask then write in new value */
    VDAC2_CR0 &= (uint8)(~VDAC2_HS_MASK);
    VDAC2_CR0 |=  (speed & VDAC2_HS_MASK);
}


/*******************************************************************************
* Function Name: VDAC2_SetRange
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
void VDAC2_SetRange(uint8 range) 
{
    VDAC2_CR0 &= (uint8)(~VDAC2_RANGE_MASK);      /* Clear existing mode */
    VDAC2_CR0 |= (range & VDAC2_RANGE_MASK);      /*  Set Range  */
    VDAC2_DacTrim();
}


/*******************************************************************************
* Function Name: VDAC2_SetValue
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
void VDAC2_SetValue(uint8 value) 
{
    #if (CY_PSOC5A)
        uint8 VDAC2_intrStatus = CyEnterCriticalSection();
    #endif /* CY_PSOC5A */

    VDAC2_Data = value;                /*  Set Value  */

    /* PSOC5A requires a double write */
    /* Exit Critical Section */
    #if (CY_PSOC5A)
        VDAC2_Data = value;
        CyExitCriticalSection(VDAC2_intrStatus);
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: VDAC2_DacTrim
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
void VDAC2_DacTrim(void) 
{
    uint8 mode;

    mode = (uint8)((VDAC2_CR0 & VDAC2_RANGE_MASK) >> 2) + VDAC2_TRIM_M7_1V_RNG_OFFSET;
    VDAC2_TR = CY_GET_XTND_REG8((uint8 *)(VDAC2_DAC_TRIM_BASE + mode));
}


/* [] END OF FILE */
