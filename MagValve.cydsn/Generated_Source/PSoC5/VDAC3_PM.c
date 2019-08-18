/*******************************************************************************
* File Name: VDAC3_PM.c  
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

#include "VDAC3.h"

static VDAC3_backupStruct VDAC3_backup;


/*******************************************************************************
* Function Name: VDAC3_SaveConfig
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
void VDAC3_SaveConfig(void) 
{
    if (!((VDAC3_CR1 & VDAC3_SRC_MASK) == VDAC3_SRC_UDB))
    {
        VDAC3_backup.data_value = VDAC3_Data;
    }
}


/*******************************************************************************
* Function Name: VDAC3_RestoreConfig
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
void VDAC3_RestoreConfig(void) 
{
    if (!((VDAC3_CR1 & VDAC3_SRC_MASK) == VDAC3_SRC_UDB))
    {
        if((VDAC3_Strobe & VDAC3_STRB_MASK) == VDAC3_STRB_EN)
        {
            VDAC3_Strobe &= (uint8)(~VDAC3_STRB_MASK);
            VDAC3_Data = VDAC3_backup.data_value;
            VDAC3_Strobe |= VDAC3_STRB_EN;
        }
        else
        {
            VDAC3_Data = VDAC3_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: VDAC3_Sleep
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
*  VDAC3_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void VDAC3_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(VDAC3_ACT_PWR_EN == (VDAC3_PWRMGR & VDAC3_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        VDAC3_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        VDAC3_backup.enableState = 0u;
    }
    
    VDAC3_Stop();
    VDAC3_SaveConfig();
}


/*******************************************************************************
* Function Name: VDAC3_Wakeup
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
*  VDAC3_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void VDAC3_Wakeup(void) 
{
    VDAC3_RestoreConfig();
    
    if(VDAC3_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        VDAC3_Enable();

        /* Restore the data register */
        VDAC3_SetValue(VDAC3_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
