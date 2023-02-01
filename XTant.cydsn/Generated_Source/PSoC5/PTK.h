/*******************************************************************************
* File Name: PTK.h
* Version 1.0
*
* Description:
*  This header file contains registers and constants associated with the
*  Count7 component.
*
* Note:
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_COUNT7_PTK_H)
#define CY_COUNT7_PTK_H

#include "cyfitter.h"

#if ((CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC3) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC4) || \
     (CYDEV_CHIP_FAMILY_USED == CYDEV_CHIP_FAMILY_PSOC5))
    #include "cytypes.h"
    #include "CyLib.h"
#else
    #include "syslib/cy_syslib.h"
#endif


/***************************************
* Function Prototypes
***************************************/
void  PTK_Init(void) ;
void  PTK_Enable(void) ;
void  PTK_Start(void) ;
void  PTK_Stop(void) ;
void  PTK_WriteCounter(uint8 count) ;
uint8 PTK_ReadCounter(void) ;
void  PTK_WritePeriod(uint8 period) ;
uint8 PTK_ReadPeriod(void) ;
void  PTK_SaveConfig(void) ;
void  PTK_RestoreConfig(void) ;
void  PTK_Sleep(void) ;
void  PTK_Wakeup(void) ;


/***************************************
*     Data Struct Definitions
***************************************/
/* Structure to save registers before go to sleep */
typedef struct
{
    uint8 enableState;
    uint8 count;
} PTK_BACKUP_STRUCT;


/***************************************
*           Global Variables
***************************************/
extern PTK_BACKUP_STRUCT PTK_backup;
extern uint8 PTK_initVar;


/***************************************
* Initial Parameter
***************************************/
#define PTK_INITIAL_PERIOD               (51u)


/***************************************
* Registers
***************************************/
#define PTK_COUNT_REG                    (*(reg8 *) PTK_Counter7__COUNT_REG)
#define PTK_COUNT_PTR                    ( (reg8 *) PTK_Counter7__COUNT_REG)
#define PTK_PERIOD_REG                   (*(reg8 *) PTK_Counter7__PERIOD_REG)
#define PTK_PERIOD_PTR                   ( (reg8 *) PTK_Counter7__PERIOD_REG)
#define PTK_AUX_CONTROL_REG              (*(reg8 *) PTK_Counter7__CONTROL_AUX_CTL_REG)
#define PTK_AUX_CONTROL_PTR              ( (reg8 *) PTK_Counter7__CONTROL_AUX_CTL_REG)


/***************************************
* Register Constants
***************************************/
#define PTK_COUNTER_START                (0x20u)

/* This constant is used to mask the TC bit (bit#7) in the Count register */
#define PTK_COUNT_7BIT_MASK              (0x7Fu)


#endif /* CY_COUNT7_PTK_H */


/* [] END OF FILE */
