/*******************************************************************************
* File Name: DirveReg3.h  
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

#if !defined(CY_CONTROL_REG_DirveReg3_H) /* CY_CONTROL_REG_DirveReg3_H */
#define CY_CONTROL_REG_DirveReg3_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} DirveReg3_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    DirveReg3_Write(uint8 control) ;
uint8   DirveReg3_Read(void) ;

void DirveReg3_SaveConfig(void) ;
void DirveReg3_RestoreConfig(void) ;
void DirveReg3_Sleep(void) ; 
void DirveReg3_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define DirveReg3_Control        (* (reg8 *) DirveReg3_Sync_ctrl_reg__CONTROL_REG )
#define DirveReg3_Control_PTR    (  (reg8 *) DirveReg3_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_DirveReg3_H */


/* [] END OF FILE */
