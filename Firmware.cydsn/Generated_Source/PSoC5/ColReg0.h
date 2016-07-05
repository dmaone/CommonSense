/*******************************************************************************
* File Name: ColReg0.h  
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

#if !defined(CY_CONTROL_REG_ColReg0_H) /* CY_CONTROL_REG_ColReg0_H */
#define CY_CONTROL_REG_ColReg0_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} ColReg0_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    ColReg0_Write(uint8 control) ;
uint8   ColReg0_Read(void) ;

void ColReg0_SaveConfig(void) ;
void ColReg0_RestoreConfig(void) ;
void ColReg0_Sleep(void) ; 
void ColReg0_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define ColReg0_Control        (* (reg8 *) ColReg0_Sync_ctrl_reg__CONTROL_REG )
#define ColReg0_Control_PTR    (  (reg8 *) ColReg0_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_ColReg0_H */


/* [] END OF FILE */
