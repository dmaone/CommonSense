/*******************************************************************************
* File Name: ColReg1.h  
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

#if !defined(CY_CONTROL_REG_ColReg1_H) /* CY_CONTROL_REG_ColReg1_H */
#define CY_CONTROL_REG_ColReg1_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} ColReg1_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    ColReg1_Write(uint8 control) ;
uint8   ColReg1_Read(void) ;

void ColReg1_SaveConfig(void) ;
void ColReg1_RestoreConfig(void) ;
void ColReg1_Sleep(void) ; 
void ColReg1_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define ColReg1_Control        (* (reg8 *) ColReg1_Sync_ctrl_reg__CONTROL_REG )
#define ColReg1_Control_PTR    (  (reg8 *) ColReg1_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_ColReg1_H */


/* [] END OF FILE */
