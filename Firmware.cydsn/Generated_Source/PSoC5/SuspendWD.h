/*******************************************************************************
* File Name: SuspendWD.h
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

#if !defined(CY_COUNT7_SuspendWD_H)
#define CY_COUNT7_SuspendWD_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
* Function Prototypes
***************************************/
void  SuspendWD_Init(void) ;
void  SuspendWD_Enable(void) ;
void  SuspendWD_Start(void) ;
void  SuspendWD_Stop(void) ;
void  SuspendWD_WriteCounter(uint8 count) ;
uint8 SuspendWD_ReadCounter(void) ;
void  SuspendWD_WritePeriod(uint8 period) ;
uint8 SuspendWD_ReadPeriod(void) ;
void  SuspendWD_SaveConfig(void) ;
void  SuspendWD_RestoreConfig(void) ;
void  SuspendWD_Sleep(void) ;
void  SuspendWD_Wakeup(void) ;


/***************************************
*     Data Struct Definitions
***************************************/
/* Structure to save registers before go to sleep */
typedef struct
{
    uint8 enableState;
    uint8 count;
} SuspendWD_BACKUP_STRUCT;


/***************************************
*           Global Variables
***************************************/
extern SuspendWD_BACKUP_STRUCT SuspendWD_backup;
extern uint8 SuspendWD_initVar;


/***************************************
* Initial Parameter
***************************************/
#define SuspendWD_INITIAL_PERIOD               (3u)


/***************************************
* Registers
***************************************/
#define SuspendWD_COUNT_REG                    (*(reg8 *) SuspendWD_Counter7__COUNT_REG)
#define SuspendWD_COUNT_PTR                    ( (reg8 *) SuspendWD_Counter7__COUNT_REG)
#define SuspendWD_PERIOD_REG                   (*(reg8 *) SuspendWD_Counter7__PERIOD_REG)
#define SuspendWD_PERIOD_PTR                   ( (reg8 *) SuspendWD_Counter7__PERIOD_REG)
#define SuspendWD_AUX_CONTROL_REG              (*(reg8 *) SuspendWD_Counter7__CONTROL_AUX_CTL_REG)
#define SuspendWD_AUX_CONTROL_PTR              ( (reg8 *) SuspendWD_Counter7__CONTROL_AUX_CTL_REG)


/***************************************
* Register Constants
***************************************/
#define SuspendWD_COUNTER_START                (0x20u)

/* This constant is used to mask the TC bit (bit#7) in the Count register */
#define SuspendWD_COUNT_7BIT_MASK              (0x7Fu)


#endif /* CY_COUNT7_SuspendWD_H */


/* [] END OF FILE */
