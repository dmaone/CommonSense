/*******************************************************************************
* File Name: PS2_CLK_En.h  
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

#if !defined(CY_CONTROL_REG_PS2_CLK_En_H) /* CY_CONTROL_REG_PS2_CLK_En_H */
#define CY_CONTROL_REG_PS2_CLK_En_H

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

} PS2_CLK_En_BACKUP_STRUCT;


/***************************************
*         Function Prototypes 
***************************************/

void    PS2_CLK_En_Write(uint8 control) ;
uint8   PS2_CLK_En_Read(void) ;

void PS2_CLK_En_SaveConfig(void) ;
void PS2_CLK_En_RestoreConfig(void) ;
void PS2_CLK_En_Sleep(void) ; 
void PS2_CLK_En_Wakeup(void) ;


/***************************************
*            Registers        
***************************************/

/* Control Register */
#define PS2_CLK_En_Control        (* (reg8 *) PS2_CLK_En_Sync_ctrl_reg__CONTROL_REG )
#define PS2_CLK_En_Control_PTR    (  (reg8 *) PS2_CLK_En_Sync_ctrl_reg__CONTROL_REG )

#endif /* End CY_CONTROL_REG_PS2_CLK_En_H */


/* [] END OF FILE */
