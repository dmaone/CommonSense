/*******************************************************************************
* File Name: ILO_Trim.h
* Version 2.0
*
* Description:
*  This file provides constants and parameter values and API definition for the
*  ILO_Trim Component
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_ILO_Trim_ILO_Trim_H) /* ILO_Trim Header File */
#define CY_ILO_Trim_ILO_Trim_H

#include <ILO_Trim_CorrectionISR.h>
#if(CY_PSOC4 && 1)
    #include <ILO_Trim_TCPWM.h>
#endif /* End of PSoC 4 code*/

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#ifndef CY_PSOC5A
    #error Component ILO_Trim_v2_0 requires cy_boot v3.0 or later
#endif /* CY_PSOC5A */


/*******************************************************************************
* Conditional Compilation Parameters
********************************************************************************/
#define ILO_Trim_REF_FREQUENCY_KHZ          ((uint32)79500.000u)
#define ILO_Trim_ILO_FREQUENCY              (100u)
#define ILO_Trim_RATIO                      (795uu)

/* Fixed Function or UDB Timer */
#define ILO_Trim_USING_FIXED_FUNCTION     (1u)
/* External reference Clock */
#define ILO_Trim_REF_FREQ               ((uint32)(1000u * (uint32)79500.000))
/* The target ILO frequency */
#define ILO_Trim_ILO_EDGE_FREQ          ((uint32)(1000u * (uint32)100))


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 timerEnableState;
    #if(!ILO_Trim_USING_FIXED_FUNCTION)
        uint8 timerCapture;
		/* Declaration for backing up enable state of the Timer */
        uint8 timerControlRegister;
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */
}ILO_Trim_backupStruct;


/***************************************
*        Function Prototypes
***************************************/
#if (!CY_PSOC4)
    CY_ISR_PROTO(ILO_Trim_CorrectionIteration);
#endif

void  ILO_Trim_Start(void)                      ;
void  ILO_Trim_Stop(void)                       ;
uint8 ILO_Trim_CheckStatus(void)                ;
int16 ILO_Trim_CheckError(void)                 ;
void ILO_Trim_BeginTrimming(void)               ;
void ILO_Trim_StopTrimming(void)                ;
void ILO_Trim_RestoreTrim(void)                 ;
uint8 ILO_Trim_GetTrim(void)                    ;
void ILO_Trim_SetTrim(uint8 trimVal)            ;
uint16 ILO_Trim_Compensate(uint16 clocks)       ;
void ILO_Trim_Sleep(void)                       ;
void ILO_Trim_Wakeup(void)                      ;
void ILO_Trim_SaveConfig(void)                  ;
void ILO_Trim_RestoreConfig(void)               ;


/*******************************************************************************
* API Constants
********************************************************************************/
#define ILO_Trim_CORRECTION_STEP_SIZE       (1u)

#if (!ILO_Trim_USING_FIXED_FUNCTION || CY_PSOC4)
    /* Ideal edges */
	#define ILO_Trim_EDGES_IDEAL ((uint32)(ILO_Trim_REF_FREQ / ILO_Trim_ILO_EDGE_FREQ))
#else
    /* Ideal edges */
	#define ILO_Trim_EDGES_IDEAL ((uint32)(ILO_Trim_REF_FREQ / (2u * ILO_Trim_ILO_EDGE_FREQ)))
#endif /* End of Device and Implementation specific defines */

#define ILO_Trim_EDGES_THOUSAND    ((uint32)(ILO_Trim_EDGES_IDEAL * 1000u))
#define ILO_Trim_EDGES_ROUND       ((uint32)(ILO_Trim_EDGES_IDEAL >> 1u))
#define ILO_Trim_EDGES_PPT         ((uint32)(ILO_Trim_EDGES_THOUSAND + ILO_Trim_EDGES_ROUND))

#define ILO_Trim_EDGES_HUNDRED     ((uint32)(ILO_Trim_EDGES_IDEAL * 100u))
#define ILO_Trim_EDGES_PPH         ((uint32)(ILO_Trim_EDGES_HUNDRED + ILO_Trim_EDGES_ROUND))

#if (!CY_PSOC4)
	/*  ILO_TR0[3:0] are reserved for 1kHz fine trim, 
		ILO_TR0[7:4] are reserved for 100kHz fine trim,
		ILO_TR1[1:0] are reserved for coarse trim  */
	#if (100 == 1u)
		#define ILO_Trim_ILO_TR0_MASK       (0x0Fu)
		#define ILO_Trim_FINE_TRIM_CLEAR    ((uint8)(~(uint8)ILO_Trim_ILO_TR0_MASK))
	#else
		#define ILO_Trim_ILO_TR0_MASK       (0xF0u)
		#define ILO_Trim_FINE_TRIM_CLEAR    ((uint8)(~(uint8)ILO_Trim_ILO_TR0_MASK))
	#endif /* End of ILO Frequency dependant code*/

	#define ILO_Trim_ILO_TR1_MASK           (0x03u)
	#define ILO_Trim_COARSE_TRIM_CLEAR      (0xF8u)
#endif /* End of PSoC 3 and PSOC 5LP code*/

#define ILO_Trim_FINE_TRIM_MASK         (0x0Fu)
#define ILO_Trim_COARSE_TRIM_ADD        (0x10u)
#define ILO_Trim_FINE_TRIM_ADD          (0x07u)

/* Default error values at startup and ISR control */
#define ILO_Trim_ERROR_DEFAULT          ((uint16)0x7FFFu)
#define ILO_Trim_INIT_DELAY             (0x00u)
#define ILO_Trim_FINAL_DELAY            (0x04u)
#define ILO_Trim_DELAY_COUNT            (0x01u)
#define ILO_Trim_ISR_INVALID            (0x01u)
#define ILO_Trim_ISR_VALID              (0x00u)
#define ILO_Trim_ISR_OFF                (0x00u)

/* Constants for error calculation and compensation */
#define ILO_Trim_PP_THOUSAND            ((uint32)0x000003E8u)
#define ILO_Trim_PP_HUNDRED             ((uint32)0x00000064u)
#define ILO_Trim_HALF                   ((uint32)0x00000002u)
#define ILO_Trim_TIME_SCALE             ((uint32)0x00002710u)
#define ILO_Trim_ILO_SCALE              ((uint32)0x00000064u)
#define ILO_Trim_COUNTER_SATURATE       ((uint16)0xFFFFu)

/* Misc Constants */
#define ILO_Trim_COUNT_ZERO             (0x00u)
#define ILO_Trim_ERROR_ZERO             (0x00)
#define ILO_Trim_TRIM_INVALID           (0x00u)
#define ILO_Trim_TRIM_COARSE_NEXT       (0x00u)

/* Accuracy status */
#define ILO_Trim_ACCURATE_SHIFT         (0x00u)
#define ILO_Trim_IS_ACCURATE            ((uint8)((uint8)0x01u << ILO_Trim_ACCURATE_SHIFT))

/* Trimming status */
#define ILO_Trim_TRIMMED                (0x00u)
#define ILO_Trim_TRIMMING_SHIFT         (0x01u)
#define ILO_Trim_TRIMMING               ((uint8)((uint8)0x01u << ILO_Trim_TRIMMING_SHIFT))

/* ILO frequency definitions */
#define ILO_Trim_1kHz                   (1u)
#define ILO_Trim_32kHz                  (32u)
#define ILO_Trim_100kHz                 (100u)

/* Resolution of the timer */
#if (!ILO_Trim_USING_FIXED_FUNCTION)
	#define ILO_Trim_RESOLUTION         ((uint16)0x00FFu)
#else
	#define ILO_Trim_RESOLUTION         ((uint16)0xFFFFu)
#endif /* End of Fixed Function code */

/* Used for shifting the contents of the ILO Trim registers */
#define ILO_Trim_TRIM_SHIFT             (4u)

/* Timer needs an offset when converting to real number */
#if(CY_PSOC4)
    #define ILO_Trim_TIMER_OFFSET       (2u)
#else
    #if (!ILO_Trim_USING_FIXED_FUNCTION)
        #define ILO_Trim_TIMER_OFFSET   (2u)
    #else
        #define ILO_Trim_TIMER_OFFSET   (3u)
    #endif
#endif /* End of PSoC 4 code*/


/*******************************************************************************
*   UDB Timer defines
********************************************************************************/
#if (!ILO_Trim_USING_FIXED_FUNCTION)


    /***************************************
    *             UDB Registers
    ***************************************/
    /* Control register */
    #define ILO_Trim_CONTROL_REG            (*(reg8 *) ILO_Trim_bILO_Timer_CtlReg__CONTROL_REG)
    #define ILO_Trim_CONTROL_PTR            ( (reg8 *) ILO_Trim_bILO_Timer_CtlReg__CONTROL_REG)

    /* Counter register */
    #define ILO_Trim_COUNTER_PTR            ((reg8 *) ILO_Trim_bILO_Timer_CounterDP_u0__A0_REG)
    /* Capture register */
    #define ILO_Trim_CAPTURE_PTR            ((reg8 *) ILO_Trim_bILO_Timer_CounterDP_u0__A1_REG)


    /***************************************
    *    Register Constants
    ***************************************/
    #define ILO_Trim_CTRL_ENABLE_SHIFT      (0x00u)
     /* Control Register Bit Mask */
    #define ILO_Trim_CTRL_ENABLE            ((uint8)((uint8)0x01u << ILO_Trim_CTRL_ENABLE_SHIFT))


/*******************************************************************************
*   Fixed Function Timer defines
********************************************************************************/
#else


    /***************************************
    *    Fixed Function constants
    ***************************************/
    #define ILO_Trim_INIT_PERIOD           ((uint16)0xFFFFu)


    /***************************************
    *    Fixed Function Registers
    ***************************************/
    #define ILO_Trim_STATUS_REG         (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define ILO_Trim_STATUS_MASK        (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__SR0 )
    #define ILO_Trim_CONTROL_REG        (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__CFG0)
    #define ILO_Trim_CONTROL2_REG       (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__CFG1)
    #define ILO_Trim_CONTROL2_PTR       ( (reg8 *) ILO_Trim_bILO_Trim_FF_Timer__CFG1)

    #define ILO_Trim_CONTROL3_REG       (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__CFG2)
    #define ILO_Trim_CONTROL3_PTR       ( (reg8 *) ILO_Trim_bILO_Trim_FF_Timer__CFG2)

    #define ILO_Trim_GLOBAL_ENABLE      (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__PM_ACT_CFG)
    #define ILO_Trim_GLOBAL_STBY_ENABLE (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__PM_STBY_CFG)

    #define ILO_Trim_CAPTURE_LSB        (* (reg16 *) ILO_Trim_bILO_Trim_FF_Timer__CAP0 )
    #define ILO_Trim_CAPTURE_LSB_PTR    ((reg16 *) ILO_Trim_bILO_Trim_FF_Timer__CAP0 )
    #define ILO_Trim_PERIOD_LSB         (* (reg16 *) ILO_Trim_bILO_Trim_FF_Timer__PER0 )
    #define ILO_Trim_PERIOD_LSB_PTR     ((reg16 *) ILO_Trim_bILO_Trim_FF_Timer__PER0 )
    #define ILO_Trim_COUNTER_LSB        (* (reg16 *) ILO_Trim_bILO_Trim_FF_Timer__CNT_CMP0 )
    #define ILO_Trim_COUNTER_LSB_PTR    ((reg16 *) ILO_Trim_bILO_Trim_FF_Timer__CNT_CMP0 )

    #define ILO_Trim_RT1_REG            (*(reg8 *) ILO_Trim_bILO_Trim_FF_Timer__RT1)
    #define ILO_Trim_RT1_PTR            ( (reg8 *) ILO_Trim_bILO_Trim_FF_Timer__RT1)


    /***************************************
    *    Register Constants
    ***************************************/
    #define ILO_Trim_BLOCK_EN_MASK          ILO_Trim_bILO_Trim_FF_Timer__PM_ACT_MSK
    #define ILO_Trim_BLOCK_STBY_EN_MASK     ILO_Trim_bILO_Trim_FF_Timer__PM_STBY_MSK

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define ILO_Trim_RT1_SHIFT              (0x04u)
    /* Sync TC and CMP bit masks */
    #define ILO_Trim_RT1_MASK               ((uint8)((uint8)0x03u << ILO_Trim_RT1_SHIFT))
    #define ILO_Trim_SYNC                   ((uint8)((uint8)0x03u << ILO_Trim_RT1_SHIFT))
    #define ILO_Trim_SYNCDSI_SHIFT          (0x00u)
    /* Sync all DSI inputs with Mask  */
    #define ILO_Trim_SYNCDSI_MASK           ((uint8)((uint8)0x0Fu << ILO_Trim_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define ILO_Trim_SYNCDSI_EN             ((uint8)((uint8)0x0Fu << ILO_Trim_SYNCDSI_SHIFT))

    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define ILO_Trim_CTRL_ENABLE_SHIFT      (0x00u)
     /* Control Register Bit Mask */
    #define ILO_Trim_CTRL_ENABLE            ((uint8)((uint8)0x01u << ILO_Trim_CTRL_ENABLE_SHIFT))

    /* Control3 Register Bit Locations */
    #define ILO_Trim_CTRL_MODE_SHIFT        (0x00u)
    /* Control3 Register Bit Mask */
    #define ILO_Trim_CTRL_MODE_MASK         ((uint8)((uint8)0x03u << ILO_Trim_CTRL_MODE_SHIFT))

#endif /* End of Fixed Function code */

#endif /* End of ILO_Trim Header File */


/* [] END OF FILE */
