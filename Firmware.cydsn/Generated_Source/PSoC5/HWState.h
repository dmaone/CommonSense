/*******************************************************************************
* File Name: HWState.h  
* Version 1.90
*
* Description:
*  This file containts Status Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_STATUS_REG_HWState_H) /* CY_STATUS_REG_HWState_H */
#define CY_STATUS_REG_HWState_H

#include "cytypes.h"
#include "CyLib.h"

    
/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 statusState;

} HWState_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

uint8 HWState_Read(void) ;
void HWState_InterruptEnable(void) ;
void HWState_InterruptDisable(void) ;
void HWState_WriteMask(uint8 mask) ;
uint8 HWState_ReadMask(void) ;


/***************************************
*           API Constants
***************************************/

#define HWState_STATUS_INTR_ENBL    0x10u


/***************************************
*         Parameter Constants
***************************************/

/* Status Register Inputs */
#define HWState_INPUTS              1


/***************************************
*             Registers
***************************************/

/* Status Register */
#define HWState_Status             (* (reg8 *) HWState_sts_intr_sts_reg__STATUS_REG )
#define HWState_Status_PTR         (  (reg8 *) HWState_sts_intr_sts_reg__STATUS_REG )
#define HWState_Status_Mask        (* (reg8 *) HWState_sts_intr_sts_reg__MASK_REG )
#define HWState_Status_Aux_Ctrl    (* (reg8 *) HWState_sts_intr_sts_reg__STATUS_AUX_CTL_REG )

#endif /* End CY_STATUS_REG_HWState_H */


/* [] END OF FILE */
