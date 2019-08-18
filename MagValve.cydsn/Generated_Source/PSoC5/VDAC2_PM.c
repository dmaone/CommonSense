/*******************************************************************************
* File Name: VDAC2_PM.c  
* Version 1.90
*
* Description:
*  This file provides the power management source code to API for the
*  VDAC8.  
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "VDAC2.h"

static VDAC2_backupStruct VDAC2_backup;


/*******************************************************************************
* Function Name: VDAC2_SaveConfig
********************************************************************************
* Summary:
*  Save the current user configuration
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void VDAC2_SaveConfig(void) 
{
    if (!((VDAC2_CR1 & VDAC2_SRC_MASK) == VDAC2_SRC_UDB))
    {
        VDAC2_backup.data_value = VDAC2_Data;
    }
}


/*******************************************************************************
* Function Name: VDAC2_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
*******************************************************************************/
void VDAC2_RestoreConfig(void) 
{
    if (!((VDAC2_CR1 & VDAC2_SRC_MASK) == VDAC2_SRC_UDB))
    {
        if((VDAC2_Strobe & VDAC2_STRB_MASK) == VDAC2_STRB_EN)
        {
            VDAC2_Strobe &= (uint8)(~VDAC2_STRB_MASK);
            VDAC2_Data = VDAC2_backup.data_value;
            VDAC2_Strobe |= VDAC2_STRB_EN;
        }
        else
        {
            VDAC2_Data = VDAC2_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: VDAC2_Sleep
********************************************************************************
* Summary:
*  Stop and Save the user configuration
*
* Parameters:  
*  void:  
*
* Return: 
*  void
*
* Global variables:
*  VDAC2_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void VDAC2_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(VDAC2_ACT_PWR_EN == (VDAC2_PWRMGR & VDAC2_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        VDAC2_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        VDAC2_backup.enableState = 0u;
    }
    
    VDAC2_Stop();
    VDAC2_SaveConfig();
}


/*******************************************************************************
* Function Name: VDAC2_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  VDAC2_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void VDAC2_Wakeup(void) 
{
    VDAC2_RestoreConfig();
    
    if(VDAC2_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        VDAC2_Enable();

        /* Restore the data register */
        VDAC2_SetValue(VDAC2_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
