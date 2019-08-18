/*******************************************************************************
* File Name: VDAC1_PM.c  
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

#include "VDAC1.h"

static VDAC1_backupStruct VDAC1_backup;


/*******************************************************************************
* Function Name: VDAC1_SaveConfig
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
void VDAC1_SaveConfig(void) 
{
    if (!((VDAC1_CR1 & VDAC1_SRC_MASK) == VDAC1_SRC_UDB))
    {
        VDAC1_backup.data_value = VDAC1_Data;
    }
}


/*******************************************************************************
* Function Name: VDAC1_RestoreConfig
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
void VDAC1_RestoreConfig(void) 
{
    if (!((VDAC1_CR1 & VDAC1_SRC_MASK) == VDAC1_SRC_UDB))
    {
        if((VDAC1_Strobe & VDAC1_STRB_MASK) == VDAC1_STRB_EN)
        {
            VDAC1_Strobe &= (uint8)(~VDAC1_STRB_MASK);
            VDAC1_Data = VDAC1_backup.data_value;
            VDAC1_Strobe |= VDAC1_STRB_EN;
        }
        else
        {
            VDAC1_Data = VDAC1_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: VDAC1_Sleep
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
*  VDAC1_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void VDAC1_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(VDAC1_ACT_PWR_EN == (VDAC1_PWRMGR & VDAC1_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        VDAC1_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        VDAC1_backup.enableState = 0u;
    }
    
    VDAC1_Stop();
    VDAC1_SaveConfig();
}


/*******************************************************************************
* Function Name: VDAC1_Wakeup
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
*  VDAC1_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void VDAC1_Wakeup(void) 
{
    VDAC1_RestoreConfig();
    
    if(VDAC1_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        VDAC1_Enable();

        /* Restore the data register */
        VDAC1_SetValue(VDAC1_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
