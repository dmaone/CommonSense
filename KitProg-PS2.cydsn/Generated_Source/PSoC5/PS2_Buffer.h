/*******************************************************************************
* File Name: PS2_Buffer.h
* Version 2.30
*
* Description:
*  This header file contains definitions associated with the Shift Register
*  component.
*
* Note: none
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/


#if !defined(CY_SHIFTREG_PS2_Buffer_H)
#define CY_SHIFTREG_PS2_Buffer_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define PS2_Buffer_FIFO_SIZE          (4u)
#define PS2_Buffer_USE_INPUT_FIFO     (0u)
#define PS2_Buffer_USE_OUTPUT_FIFO    (0u)
#define PS2_Buffer_SR_SIZE            (16u)


/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;

    uint16 saveSrA0Reg;
    uint16 saveSrA1Reg;

    #if(CY_UDB_V0)
        uint16 saveSrIntMask;
    #endif /* (CY_UDB_V0) */

} PS2_Buffer_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  PS2_Buffer_Start(void)                              ;
void  PS2_Buffer_Stop(void)                               ;
void  PS2_Buffer_Init(void)                               ;
void  PS2_Buffer_Enable(void)                             ;
void  PS2_Buffer_RestoreConfig(void)                      ;
void  PS2_Buffer_SaveConfig(void)                         ;
void  PS2_Buffer_Sleep(void)                              ;
void  PS2_Buffer_Wakeup(void)                             ;
void  PS2_Buffer_EnableInt(void)                          ;
void  PS2_Buffer_DisableInt(void)                         ;
void  PS2_Buffer_SetIntMode(uint8 interruptSource)        ;
uint8 PS2_Buffer_GetIntStatus(void)                       ;
void  PS2_Buffer_WriteRegValue(uint16 shiftData) \
                                                                ;
uint16 PS2_Buffer_ReadRegValue(void) ;
uint8    PS2_Buffer_GetFIFOStatus(uint8 fifoId)           ;

#if(0u != PS2_Buffer_USE_INPUT_FIFO)
    cystatus PS2_Buffer_WriteData(uint16 shiftData) \
                                                                ;
#endif /* (0u != PS2_Buffer_USE_INPUT_FIFO) */

#if(0u != PS2_Buffer_USE_OUTPUT_FIFO)
    uint16 PS2_Buffer_ReadData(void) ;
#endif /* (0u != PS2_Buffer_USE_OUTPUT_FIFO) */


/**********************************
*   Variable with external linkage
**********************************/

extern uint8 PS2_Buffer_initVar;


/***************************************
*           API Constants
***************************************/

#define PS2_Buffer_LOAD                   (0x01u)
#define PS2_Buffer_STORE                  (0x02u)
#define PS2_Buffer_RESET                  (0x04u)

#define PS2_Buffer_IN_FIFO                (0x01u)
#define PS2_Buffer_OUT_FIFO               (0x02u)

#define PS2_Buffer_RET_FIFO_FULL          (0x00u)

/* This define is obsolete */
#define PS2_Buffer_RET_FIFO_NOT_EMPTY     (0x01u)

#define PS2_Buffer_RET_FIFO_PARTIAL       (0x01u)
#define PS2_Buffer_RET_FIFO_EMPTY         (0x02u)
#define PS2_Buffer_RET_FIFO_NOT_DEFINED   (0xFEu)


/***************************************
*    Enumerated Types and Parameters
***************************************/

#define PS2_Buffer__LEFT 0
#define PS2_Buffer__RIGHT 1



/***************************************
*    Initial Parameter Constants
***************************************/

#define PS2_Buffer_SR_MASK    (0xFFFFu) /* Unsigned is added to parameter */
#define PS2_Buffer_INT_SRC    (0u)
#define PS2_Buffer_DIRECTION  (1u)


/***************************************
*             Registers
***************************************/

/* Control register */
#define PS2_Buffer_SR_CONTROL_REG (* (reg8 *) \
                                           PS2_Buffer_bSR_SyncCtl_CtrlReg__CONTROL_REG)
#define PS2_Buffer_SR_CONTROL_PTR (  (reg8 *) \
                                           PS2_Buffer_bSR_SyncCtl_CtrlReg__CONTROL_REG)

/* Status register */
#define PS2_Buffer_SR_STATUS_REG      (* (reg8 *) PS2_Buffer_bSR_StsReg__STATUS_REG)
#define PS2_Buffer_SR_STATUS_PTR      (  (reg8 *) PS2_Buffer_bSR_StsReg__STATUS_REG)

/* Interrupt status register */
#define PS2_Buffer_SR_STATUS_MASK_REG (* (reg8 *) PS2_Buffer_bSR_StsReg__MASK_REG)
#define PS2_Buffer_SR_STATUS_MASK_PTR (  (reg8 *) PS2_Buffer_bSR_StsReg__MASK_REG)

/* Aux control register */
#define PS2_Buffer_SR_AUX_CONTROL_REG (* (reg8 *) PS2_Buffer_bSR_StsReg__STATUS_AUX_CTL_REG)
#define PS2_Buffer_SR_AUX_CONTROL_PTR (  (reg8 *) PS2_Buffer_bSR_StsReg__STATUS_AUX_CTL_REG)

/* A1 register: only used to implement capture function */
#define PS2_Buffer_SHIFT_REG_CAPTURE_PTR    ( (reg8 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__A1_REG )

#if(CY_PSOC3 || CY_PSOC5)
    #define PS2_Buffer_IN_FIFO_VAL_LSB_PTR        ( (reg16 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__F0_REG )

    #define PS2_Buffer_SHIFT_REG_LSB_PTR          ( (reg16 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__A0_REG )

    #define PS2_Buffer_SHIFT_REG_VALUE_LSB_PTR    ( (reg16 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__A1_REG )

    #define PS2_Buffer_OUT_FIFO_VAL_LSB_PTR       ( (reg16 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__F1_REG )

#else
    #if(PS2_Buffer_SR_SIZE <= 8u) /* 8bit - ShiftReg */
        #define PS2_Buffer_IN_FIFO_VAL_LSB_PTR        ( (reg8 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__F0_REG )

        #define PS2_Buffer_SHIFT_REG_LSB_PTR          ( (reg8 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__A0_REG )

        #define PS2_Buffer_SHIFT_REG_VALUE_LSB_PTR    ( (reg8 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__A1_REG )

        #define PS2_Buffer_OUT_FIFO_VAL_LSB_PTR       ( (reg8 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__F1_REG )

    #elif(PS2_Buffer_SR_SIZE <= 16u) /* 16bit - ShiftReg */
        #define PS2_Buffer_IN_FIFO_VAL_LSB_PTR        ( (reg16 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__16BIT_F0_REG )

        #define PS2_Buffer_SHIFT_REG_LSB_PTR          ( (reg16 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__16BIT_A0_REG )

        #define PS2_Buffer_SHIFT_REG_VALUE_LSB_PTR    ( (reg16 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__16BIT_A1_REG )

        #define PS2_Buffer_OUT_FIFO_VAL_LSB_PTR       ( (reg16 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__16BIT_F1_REG )


    #elif(PS2_Buffer_SR_SIZE <= 24u) /* 24bit - ShiftReg */
        #define PS2_Buffer_IN_FIFO_VAL_LSB_PTR        ( (reg32 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__F0_REG )

        #define PS2_Buffer_SHIFT_REG_LSB_PTR          ( (reg32 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__A0_REG )

        #define PS2_Buffer_SHIFT_REG_VALUE_LSB_PTR    ( (reg32 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__A1_REG )

        #define PS2_Buffer_OUT_FIFO_VAL_LSB_PTR       ( (reg32 *) \
                                        PS2_Buffer_bSR_sC16_BShiftRegDp_u0__F1_REG )

    #else /* 32bit - ShiftReg */
        #define PS2_Buffer_IN_FIFO_VAL_LSB_PTR        ( (reg32 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__32BIT_F0_REG )

        #define PS2_Buffer_SHIFT_REG_LSB_PTR          ( (reg32 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__32BIT_A0_REG )

        #define PS2_Buffer_SHIFT_REG_VALUE_LSB_PTR    ( (reg32 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__32BIT_A1_REG )

        #define PS2_Buffer_OUT_FIFO_VAL_LSB_PTR       ( (reg32 *) \
                                  PS2_Buffer_bSR_sC16_BShiftRegDp_u0__32BIT_F1_REG )

    #endif  /* (PS2_Buffer_SR_SIZE <= 8u) */
#endif      /* (CY_PSOC3 || CY_PSOC5) */


/***************************************
*       Register Constants
***************************************/

#define PS2_Buffer_INTERRUPTS_ENABLE      (0x10u)
#define PS2_Buffer_LOAD_INT_EN            (0x01u)
#define PS2_Buffer_STORE_INT_EN           (0x02u)
#define PS2_Buffer_RESET_INT_EN           (0x04u)
#define PS2_Buffer_CLK_EN                 (0x01u)

#define PS2_Buffer_RESET_INT_EN_MASK      (0xFBu)
#define PS2_Buffer_LOAD_INT_EN_MASK       (0xFEu)
#define PS2_Buffer_STORE_INT_EN_MASK      (0xFDu)
#define PS2_Buffer_INTS_EN_MASK           (0x07u)

#define PS2_Buffer_OUT_FIFO_CLR_BIT       (0x02u)

#if(0u != PS2_Buffer_USE_INPUT_FIFO)

    #define PS2_Buffer_IN_FIFO_MASK       (0x18u)

    #define PS2_Buffer_IN_FIFO_FULL       (0x00u)
    #define PS2_Buffer_IN_FIFO_EMPTY      (0x01u)
    #define PS2_Buffer_IN_FIFO_PARTIAL    (0x02u)
    
    /* This define is obsolete */
    #define PS2_Buffer_IN_FIFO_NOT_EMPTY  (0x02u)
    
#endif /* (0u != PS2_Buffer_USE_INPUT_FIFO) */

#define PS2_Buffer_OUT_FIFO_MASK          (0x60u)

#define PS2_Buffer_OUT_FIFO_EMPTY         (0x00u)
#define PS2_Buffer_OUT_FIFO_FULL          (0x01u)
#define PS2_Buffer_OUT_FIFO_PARTIAL       (0x02u)

/* This define is obsolete */
#define PS2_Buffer_OUT_FIFO_NOT_EMPTY     (0x02u)

#define PS2_Buffer_IN_FIFO_SHIFT_MASK     (0x03u)
#define PS2_Buffer_OUT_FIFO_SHIFT_MASK    (0x05u)

#define PS2_Buffer_DISABLED               (0u)
#define PS2_Buffer_DEFAULT_A0             (0u)
#define PS2_Buffer_DEFAULT_A1             (0u)


/***************************************
*       Macros
***************************************/

#define PS2_Buffer_IS_ENABLED         (0u != (PS2_Buffer_SR_CONTROL & PS2_Buffer_CLK_EN))

#define PS2_Buffer_GET_OUT_FIFO_STS   ((PS2_Buffer_SR_STATUS & PS2_Buffer_OUT_FIFO_MASK) >> \
                                              PS2_Buffer_OUT_FIFO_SHIFT_MASK)

#define PS2_Buffer_GET_IN_FIFO_STS    ((PS2_Buffer_SR_STATUS & PS2_Buffer_IN_FIFO_MASK)  >> \
                                              PS2_Buffer_IN_FIFO_SHIFT_MASK)


/***************************************
*       Obsolete
***************************************/

/* Following code are OBSOLETE and must not be used 
 * starting from ShiftReg 2.20 
 */

#define PS2_Buffer_SR_CONTROL     (PS2_Buffer_SR_CONTROL_REG)
#define PS2_Buffer_SR_STATUS      (PS2_Buffer_SR_STATUS_REG)
#define PS2_Buffer_SR_STATUS_MASK (PS2_Buffer_SR_STATUS_MASK_REG)
#define PS2_Buffer_SR_AUX_CONTROL (PS2_Buffer_SR_AUX_CONTROL_REG)

#define PS2_Buffer_IN_FIFO_SHFT_MASK  (PS2_Buffer_IN_FIFO_SHIFT_MASK)
#define PS2_Buffer_OUT_FIFO_SHFT_MASK (PS2_Buffer_OUT_FIFO_SHFIT_MASK)

#define PS2_Buffer_RET_FIFO_BAD_PARAM (0xFFu)

#endif /* (CY_SHIFTREG_PS2_Buffer_H) */


/* [] END OF FILE */
