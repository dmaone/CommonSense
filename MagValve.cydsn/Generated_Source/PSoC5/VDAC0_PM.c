/*******************************************************************************
* File Name: VDAC0_PM.c  
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

#include "VDAC0.h"

static VDAC0_backupStruct VDAC0_backup;


/*******************************************************************************
* Function Name: VDAC0_SaveConfig
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
void VDAC0_SaveConfig(void) 
{
    if (!((VDAC0_CR1 & VDAC0_SRC_MASK) == VDAC0_SRC_UDB))
    {
        VDAC0_backup.data_value = VDAC0_Data;
    }
}


/*******************************************************************************
* Function Name: VDAC0_RestoreConfig
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
void VDAC0_RestoreConfig(void) 
{
    if (!((VDAC0_CR1 & VDAC0_SRC_MASK) == VDAC0_SRC_UDB))
    {
        if((VDAC0_Strobe & VDAC0_STRB_MASK) == VDAC0_STRB_EN)
        {
            VDAC0_Strobe &= (uint8)(~VDAC0_STRB_MASK);
            VDAC0_Data = VDAC0_backup.data_value;
            VDAC0_Strobe |= VDAC0_STRB_EN;
        }
        else
        {
            VDAC0_Data = VDAC0_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: VDAC0_Sleep
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
*  VDAC0_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void VDAC0_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(VDAC0_ACT_PWR_EN == (VDAC0_PWRMGR & VDAC0_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        VDAC0_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        VDAC0_backup.enableState = 0u;
    }
    
    VDAC0_Stop();
    VDAC0_SaveConfig();
}


/*******************************************************************************
* Function Name: VDAC0_Wakeup
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
*  VDAC0_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void VDAC0_Wakeup(void) 
{
    VDAC0_RestoreConfig();
    
    if(VDAC0_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        VDAC0_Enable();

        /* Restore the data register */
        VDAC0_SetValue(VDAC0_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
