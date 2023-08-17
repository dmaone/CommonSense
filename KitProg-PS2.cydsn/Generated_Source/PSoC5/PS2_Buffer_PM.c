/*******************************************************************************
* File Name: PS2_Buffer_PM.c
* Version 2.30
*
* Description:
*  This file provides the API source code for sleep mode support for Shift
*  Register component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "PS2_Buffer.h"

static PS2_Buffer_BACKUP_STRUCT PS2_Buffer_backup =
{
    PS2_Buffer_DISABLED,

    ((uint16) PS2_Buffer_DEFAULT_A0),
    ((uint16) PS2_Buffer_DEFAULT_A1),

    #if(CY_UDB_V0)
        ((uint16) PS2_Buffer_INT_SRC),
    #endif /* (CY_UDB_V0) */
};


/*******************************************************************************
* Function Name: PS2_Buffer_SaveConfig
********************************************************************************
*
* Summary:
*  Saves Shift Register configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PS2_Buffer_SaveConfig(void) 
{
    /* Store working registers A0 and A1 */
    PS2_Buffer_backup.saveSrA0Reg   = CY_GET_REG16(PS2_Buffer_SHIFT_REG_LSB_PTR);
    PS2_Buffer_backup.saveSrA1Reg   = CY_GET_REG16(PS2_Buffer_SHIFT_REG_VALUE_LSB_PTR);

    #if(CY_UDB_V0)
        PS2_Buffer_backup.saveSrIntMask = PS2_Buffer_SR_STATUS_MASK;
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: PS2_Buffer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores Shift Register configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PS2_Buffer_RestoreConfig(void) 
{
    /* Restore working registers A0 and A1 */
    CY_SET_REG16(PS2_Buffer_SHIFT_REG_LSB_PTR, PS2_Buffer_backup.saveSrA0Reg);
    CY_SET_REG16(PS2_Buffer_SHIFT_REG_VALUE_LSB_PTR, PS2_Buffer_backup.saveSrA1Reg);

    #if(CY_UDB_V0)
        PS2_Buffer_SR_STATUS_MASK = ((uint8) PS2_Buffer_backup.saveSrIntMask);
    #endif /* (CY_UDB_V0) */
}


/*******************************************************************************
* Function Name: PS2_Buffer_Sleep
********************************************************************************
*
* Summary:
*  Prepare the component to enter a Sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PS2_Buffer_Sleep(void) 
{
    PS2_Buffer_backup.enableState = ((uint8) PS2_Buffer_IS_ENABLED);

    PS2_Buffer_Stop();
    PS2_Buffer_SaveConfig();
}


/*******************************************************************************
* Function Name: PS2_Buffer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void PS2_Buffer_Wakeup(void) 
{
    PS2_Buffer_RestoreConfig();

    if(0u != PS2_Buffer_backup.enableState)
    {
        PS2_Buffer_Enable();
    }
}


/* [] END OF FILE */
