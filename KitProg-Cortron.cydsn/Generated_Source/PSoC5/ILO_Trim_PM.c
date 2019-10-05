/*******************************************************************************
* File Name: ILO_Trim_PM.c
* Version 2.0
*
* Description:
*  This file provides the power management source code to API for the
*  ILO Trim component.
*
* Note:
*  None
*
*******************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "ILO_Trim.h"
static ILO_Trim_backupStruct ILO_Trim_backup;


/*******************************************************************************
* Function Name: ILO_Trim_SaveConfig
********************************************************************************
*
* Summary:
*  Save the current user configuration
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ILO_Trim_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void ILO_Trim_SaveConfig(void) 
{
    #if (!ILO_Trim_USING_FIXED_FUNCTION)
		/* Backup non retention registers, interrupt mask and capture counter */
        ILO_Trim_backup.timerCapture = CY_GET_REG8(ILO_Trim_CAPTURE_PTR);
		/* Backup the enable state of the Timer component */
        ILO_Trim_backup.timerControlRegister = (uint8)ILO_Trim_CONTROL_REG;
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: ILO_Trim_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ILO_Trim_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void ILO_Trim_RestoreConfig(void) 
{   
    #if (!ILO_Trim_USING_FIXED_FUNCTION)
        /* Restore the UDB non-rentention registers for PSoC5A */
        CY_SET_REG8(ILO_Trim_CAPTURE_PTR, ILO_Trim_backup.timerCapture);
		/* Restore the enable state of the Timer component */
        ILO_Trim_CONTROL_REG |= ILO_Trim_backup.timerControlRegister;
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: ILO_Trim_Sleep
********************************************************************************
*
* Summary:
*  Stop and Save the user configuration
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ILO_Trim_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void ILO_Trim_Sleep(void) 
{
	uint8 enState;
	
	/* Back up enable state from the Timer control register */
	#if (!ILO_Trim_USING_FIXED_FUNCTION)
		enState = (uint8)(ILO_Trim_CONTROL_REG & ILO_Trim_CTRL_ENABLE);
	#else
		#if (CY_PSOC4)
			enState = (uint8)(ILO_Trim_TCPWM_BLOCK_CONTROL_REG & ILO_Trim_TCPWM_MASK);
		#else
			enState = (uint8)(ILO_Trim_CONTROL_REG & ILO_Trim_CTRL_ENABLE);
		#endif /* End of PSoC 3 and PSOC 5LP code*/
	#endif /* End of Fixed Function code */
    /* Save Counter's enable state */
    if(enState == ILO_Trim_CTRL_ENABLE)
    {
        /* Timer is enabled */
        ILO_Trim_backup.timerEnableState = 1u;
    }
    else
    {
        /* Timer is disabled */
        ILO_Trim_backup.timerEnableState = 0u;
    }
    ILO_Trim_Stop();
    ILO_Trim_SaveConfig();
}


/*******************************************************************************
* Function Name: ILO_Trim_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  ILO_Trim_backup.TimerEnableState:  Is used to restore the enable 
*  state of block on wakeup from sleep mode.
*
*******************************************************************************/
void ILO_Trim_Wakeup(void) 
{
    ILO_Trim_RestoreConfig();
	
    if(ILO_Trim_backup.timerEnableState == 1u)
    {     /* Enable Timer's operation */
        #if (!ILO_Trim_USING_FIXED_FUNCTION)
			/* Enable UDB Timer */
			ILO_Trim_CONTROL_REG |= ILO_Trim_CTRL_ENABLE;
		#else
			#if (CY_PSOC4)
				ILO_Trim_TCPWM_Enable();
			#else
				/* Enable FF Timer */
				/* Clear all bits but the enable bit (if it's already set) for Timer operation */
			    ILO_Trim_CONTROL_REG &= ILO_Trim_CTRL_ENABLE;
				
				/* CONTROL3 register exists only in PSoC3 OR PSoC5LP */
			    ILO_Trim_CONTROL3_REG &= (uint8)(~ILO_Trim_CTRL_MODE_MASK);
				
				ILO_Trim_GLOBAL_ENABLE |= ILO_Trim_BLOCK_EN_MASK;
		        ILO_Trim_GLOBAL_STBY_ENABLE |= ILO_Trim_BLOCK_STBY_EN_MASK;

				ILO_Trim_CONTROL_REG |= ILO_Trim_CTRL_ENABLE;
			#endif /* End of PSoC 3 and PSOC 5LP code*/
		#endif /* End of Fixed Function code */
    } /* Do nothing if Timer was disabled before */
}


/* [] END OF FILE */
