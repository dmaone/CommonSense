/*******************************************************************************
* File Name: ColReg2.h  
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

#if !defined(CY_CONTROL_REG_ColReg2_H) /* CY_CONTROL_REG_ColReg2_H */
#define CY_CONTROL_REG_ColReg2_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} ColReg2_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    ColReg2_Write(uint8 control) ;
uint8   ColReg2_Read(void) ;

void ColReg2_SaveConfig(void) ;
void ColReg2_RestoreConfig(void) ;
void ColReg2_Sleep(void) ; 
void ColReg2_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define ColReg2_Control        (* (reg8 *) ColReg2_Sync_ctrl_reg__CONTROL_REG )
#define ColReg2_Control_PTR    (  (reg8 *) ColReg2_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_ColReg2_H */


/* [] END OF FILE */
