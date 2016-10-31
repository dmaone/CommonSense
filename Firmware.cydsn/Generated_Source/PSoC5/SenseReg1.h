/*******************************************************************************
* File Name: SenseReg1.h  
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

#if !defined(CY_CONTROL_REG_SenseReg1_H) /* CY_CONTROL_REG_SenseReg1_H */
#define CY_CONTROL_REG_SenseReg1_H

#include "cytypes.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} SenseReg1_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    SenseReg1_Write(uint8 control) ;
uint8   SenseReg1_Read(void) ;

void SenseReg1_SaveConfig(void) ;
void SenseReg1_RestoreConfig(void) ;
void SenseReg1_Sleep(void) ; 
void SenseReg1_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define SenseReg1_Control        (* (reg8 *) SenseReg1_Sync_ctrl_reg__CONTROL_REG )
#define SenseReg1_Control_PTR    (  (reg8 *) SenseReg1_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_SenseReg1_H */


/* [] END OF FILE */
