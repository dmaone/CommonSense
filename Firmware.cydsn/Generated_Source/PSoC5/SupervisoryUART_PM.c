/*******************************************************************************
* File Name: SupervisoryUART_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SupervisoryUART.h"


/***************************************
* Local data allocation
***************************************/

static SupervisoryUART_BACKUP_STRUCT  SupervisoryUART_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: SupervisoryUART_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the SupervisoryUART_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SupervisoryUART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SupervisoryUART_SaveConfig(void)
{
    #if(SupervisoryUART_CONTROL_REG_REMOVED == 0u)
        SupervisoryUART_backup.cr = SupervisoryUART_CONTROL_REG;
    #endif /* End SupervisoryUART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: SupervisoryUART_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SupervisoryUART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling SupervisoryUART_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void SupervisoryUART_RestoreConfig(void)
{
    #if(SupervisoryUART_CONTROL_REG_REMOVED == 0u)
        SupervisoryUART_CONTROL_REG = SupervisoryUART_backup.cr;
    #endif /* End SupervisoryUART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: SupervisoryUART_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The SupervisoryUART_Sleep() API saves the current component state. Then it
*  calls the SupervisoryUART_Stop() function and calls 
*  SupervisoryUART_SaveConfig() to save the hardware configuration.
*  Call the SupervisoryUART_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SupervisoryUART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SupervisoryUART_Sleep(void)
{
    #if(SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED)
        if((SupervisoryUART_RXSTATUS_ACTL_REG  & SupervisoryUART_INT_ENABLE) != 0u)
        {
            SupervisoryUART_backup.enableState = 1u;
        }
        else
        {
            SupervisoryUART_backup.enableState = 0u;
        }
    #else
        if((SupervisoryUART_TXSTATUS_ACTL_REG  & SupervisoryUART_INT_ENABLE) !=0u)
        {
            SupervisoryUART_backup.enableState = 1u;
        }
        else
        {
            SupervisoryUART_backup.enableState = 0u;
        }
    #endif /* End SupervisoryUART_RX_ENABLED || SupervisoryUART_HD_ENABLED*/

    SupervisoryUART_Stop();
    SupervisoryUART_SaveConfig();
}


/*******************************************************************************
* Function Name: SupervisoryUART_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  SupervisoryUART_Sleep() was called. The SupervisoryUART_Wakeup() function
*  calls the SupervisoryUART_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  SupervisoryUART_Sleep() function was called, the SupervisoryUART_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SupervisoryUART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SupervisoryUART_Wakeup(void)
{
    SupervisoryUART_RestoreConfig();
    #if( (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) )
        SupervisoryUART_ClearRxBuffer();
    #endif /* End (SupervisoryUART_RX_ENABLED) || (SupervisoryUART_HD_ENABLED) */
    #if(SupervisoryUART_TX_ENABLED || SupervisoryUART_HD_ENABLED)
        SupervisoryUART_ClearTxBuffer();
    #endif /* End SupervisoryUART_TX_ENABLED || SupervisoryUART_HD_ENABLED */

    if(SupervisoryUART_backup.enableState != 0u)
    {
        SupervisoryUART_Enable();
    }
}


/* [] END OF FILE */
