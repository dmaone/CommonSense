/*******************************************************************************
* File Name: DriveReg0.h  
* Version 1.80
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CONTROL_REG_DriveReg0_H) /* CY_CONTROL_REG_DriveReg0_H */
#define CY_CONTROL_REG_DriveReg0_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} DriveReg0_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    DriveReg0_Write(uint8 control) ;
uint8   DriveReg0_Read(void) ;

void DriveReg0_SaveConfig(void) ;
void DriveReg0_RestoreConfig(void) ;
void DriveReg0_Sleep(void) ; 
void DriveReg0_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define DriveReg0_Control        (* (reg8 *) DriveReg0_Sync_ctrl_reg__CONTROL_REG )
#define DriveReg0_Control_PTR    (  (reg8 *) DriveReg0_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_DriveReg0_H */


/* [] END OF FILE */
