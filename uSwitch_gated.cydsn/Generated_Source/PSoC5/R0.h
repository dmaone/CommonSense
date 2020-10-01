/*******************************************************************************
* File Name: R0.h  
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

#if !defined(CY_CONTROL_REG_R0_H) /* CY_CONTROL_REG_R0_H */
#define CY_CONTROL_REG_R0_H

#include "cyfitter.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))
    #include "cytypes.h"
#else
    #include "syslib/cy_syslib.h"
#endif

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 controlState;

} R0_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    R0_Write(uint8 control) ;
uint8   R0_Read(void) ;

void R0_SaveConfig(void) ;
void R0_RestoreConfig(void) ;
void R0_Sleep(void) ; 
void R0_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define R0_Control        (* (reg8 *) R0_Sync_ctrl_reg__CONTROL_REG )
#define R0_Control_PTR    (  (reg8 *) R0_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_R0_H */


/* [] END OF FILE */
