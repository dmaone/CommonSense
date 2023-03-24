/*******************************************************************************
* File Name: SysTimer.h
* Version 2.80
*
*  Description:
*     Contains the function prototypes and constants available to the timer
*     user module.
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_TIMER_SysTimer_H)
#define CY_TIMER_SysTimer_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 SysTimer_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Timer_v2_80 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define SysTimer_Resolution                 8u
#define SysTimer_UsingFixedFunction         0u
#define SysTimer_UsingHWCaptureCounter      0u
#define SysTimer_SoftwareCaptureMode        0u
#define SysTimer_SoftwareTriggerMode        0u
#define SysTimer_UsingHWEnable              0u
#define SysTimer_EnableTriggerMode          0u
#define SysTimer_InterruptOnCaptureCount    0u
#define SysTimer_RunModeUsed                0u
#define SysTimer_ControlRegRemoved          0u

#if defined(SysTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG)
    #define SysTimer_UDB_CONTROL_REG_REMOVED            (0u)
#elif  (SysTimer_UsingFixedFunction)
    #define SysTimer_UDB_CONTROL_REG_REMOVED            (0u)
#else 
    #define SysTimer_UDB_CONTROL_REG_REMOVED            (1u)
#endif /* End SysTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG */


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 TimerEnableState;
    #if(!SysTimer_UsingFixedFunction)

        uint8 TimerUdb;
        uint8 InterruptMaskValue;
        #if (SysTimer_UsingHWCaptureCounter)
            uint8 TimerCaptureCounter;
        #endif /* variable declarations for backing up non retention registers in CY_UDB_V1 */

        #if (!SysTimer_UDB_CONTROL_REG_REMOVED)
            uint8 TimerControlRegister;
        #endif /* variable declaration for backing up enable state of the Timer */
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */

}SysTimer_backupStruct;


/***************************************
*       Function Prototypes
***************************************/

void    SysTimer_Start(void) ;
void    SysTimer_Stop(void) ;

void    SysTimer_SetInterruptMode(uint8 interruptMode) ;
uint8   SysTimer_ReadStatusRegister(void) ;
/* Deprecated function. Do not use this in future. Retained for backward compatibility */
#define SysTimer_GetInterruptSource() SysTimer_ReadStatusRegister()

#if(!SysTimer_UDB_CONTROL_REG_REMOVED)
    uint8   SysTimer_ReadControlRegister(void) ;
    void    SysTimer_WriteControlRegister(uint8 control) ;
#endif /* (!SysTimer_UDB_CONTROL_REG_REMOVED) */

uint8  SysTimer_ReadPeriod(void) ;
void    SysTimer_WritePeriod(uint8 period) ;
uint8  SysTimer_ReadCounter(void) ;
void    SysTimer_WriteCounter(uint8 counter) ;
uint8  SysTimer_ReadCapture(void) ;
void    SysTimer_SoftwareCapture(void) ;

#if(!SysTimer_UsingFixedFunction) /* UDB Prototypes */
    #if (SysTimer_SoftwareCaptureMode)
        void    SysTimer_SetCaptureMode(uint8 captureMode) ;
    #endif /* (!SysTimer_UsingFixedFunction) */

    #if (SysTimer_SoftwareTriggerMode)
        void    SysTimer_SetTriggerMode(uint8 triggerMode) ;
    #endif /* (SysTimer_SoftwareTriggerMode) */

    #if (SysTimer_EnableTriggerMode)
        void    SysTimer_EnableTrigger(void) ;
        void    SysTimer_DisableTrigger(void) ;
    #endif /* (SysTimer_EnableTriggerMode) */


    #if(SysTimer_InterruptOnCaptureCount)
        void    SysTimer_SetInterruptCount(uint8 interruptCount) ;
    #endif /* (SysTimer_InterruptOnCaptureCount) */

    #if (SysTimer_UsingHWCaptureCounter)
        void    SysTimer_SetCaptureCount(uint8 captureCount) ;
        uint8   SysTimer_ReadCaptureCount(void) ;
    #endif /* (SysTimer_UsingHWCaptureCounter) */

    void SysTimer_ClearFIFO(void) ;
#endif /* UDB Prototypes */

/* Sleep Retention APIs */
void SysTimer_Init(void)          ;
void SysTimer_Enable(void)        ;
void SysTimer_SaveConfig(void)    ;
void SysTimer_RestoreConfig(void) ;
void SysTimer_Sleep(void)         ;
void SysTimer_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Timer__CaptureModes, Used in Capture Mode */
#define SysTimer__B_TIMER__CM_NONE 0
#define SysTimer__B_TIMER__CM_RISINGEDGE 1
#define SysTimer__B_TIMER__CM_FALLINGEDGE 2
#define SysTimer__B_TIMER__CM_EITHEREDGE 3
#define SysTimer__B_TIMER__CM_SOFTWARE 4



/* Enumerated Type B_Timer__TriggerModes, Used in Trigger Mode */
#define SysTimer__B_TIMER__TM_NONE 0x00u
#define SysTimer__B_TIMER__TM_RISINGEDGE 0x04u
#define SysTimer__B_TIMER__TM_FALLINGEDGE 0x08u
#define SysTimer__B_TIMER__TM_EITHEREDGE 0x0Cu
#define SysTimer__B_TIMER__TM_SOFTWARE 0x10u


/***************************************
*    Initialial Parameter Constants
***************************************/

#define SysTimer_INIT_PERIOD             99u
#define SysTimer_INIT_CAPTURE_MODE       ((uint8)((uint8)0u << SysTimer_CTRL_CAP_MODE_SHIFT))
#define SysTimer_INIT_TRIGGER_MODE       ((uint8)((uint8)0u << SysTimer_CTRL_TRIG_MODE_SHIFT))
#if (SysTimer_UsingFixedFunction)
    #define SysTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << SysTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                  ((uint8)((uint8)0 << SysTimer_STATUS_CAPTURE_INT_MASK_SHIFT)))
#else
    #define SysTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << SysTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << SysTimer_STATUS_CAPTURE_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << SysTimer_STATUS_FIFOFULL_INT_MASK_SHIFT)))
#endif /* (SysTimer_UsingFixedFunction) */
#define SysTimer_INIT_CAPTURE_COUNT      (2u)
#define SysTimer_INIT_INT_CAPTURE_COUNT  ((uint8)((uint8)(1u - 1u) << SysTimer_CTRL_INTCNT_SHIFT))


/***************************************
*           Registers
***************************************/

#if (SysTimer_UsingFixedFunction) /* Implementation Specific Registers and Register Constants */


    /***************************************
    *    Fixed Function Registers
    ***************************************/

    #define SysTimer_STATUS         (*(reg8 *) SysTimer_TimerHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define SysTimer_STATUS_MASK    (*(reg8 *) SysTimer_TimerHW__SR0 )
    #define SysTimer_CONTROL        (*(reg8 *) SysTimer_TimerHW__CFG0)
    #define SysTimer_CONTROL2       (*(reg8 *) SysTimer_TimerHW__CFG1)
    #define SysTimer_CONTROL2_PTR   ( (reg8 *) SysTimer_TimerHW__CFG1)
    #define SysTimer_RT1            (*(reg8 *) SysTimer_TimerHW__RT1)
    #define SysTimer_RT1_PTR        ( (reg8 *) SysTimer_TimerHW__RT1)

    #if (CY_PSOC3 || CY_PSOC5LP)
        #define SysTimer_CONTROL3       (*(reg8 *) SysTimer_TimerHW__CFG2)
        #define SysTimer_CONTROL3_PTR   ( (reg8 *) SysTimer_TimerHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define SysTimer_GLOBAL_ENABLE  (*(reg8 *) SysTimer_TimerHW__PM_ACT_CFG)
    #define SysTimer_GLOBAL_STBY_ENABLE  (*(reg8 *) SysTimer_TimerHW__PM_STBY_CFG)

    #define SysTimer_CAPTURE_LSB         (* (reg16 *) SysTimer_TimerHW__CAP0 )
    #define SysTimer_CAPTURE_LSB_PTR       ((reg16 *) SysTimer_TimerHW__CAP0 )
    #define SysTimer_PERIOD_LSB          (* (reg16 *) SysTimer_TimerHW__PER0 )
    #define SysTimer_PERIOD_LSB_PTR        ((reg16 *) SysTimer_TimerHW__PER0 )
    #define SysTimer_COUNTER_LSB         (* (reg16 *) SysTimer_TimerHW__CNT_CMP0 )
    #define SysTimer_COUNTER_LSB_PTR       ((reg16 *) SysTimer_TimerHW__CNT_CMP0 )


    /***************************************
    *    Register Constants
    ***************************************/

    /* Fixed Function Block Chosen */
    #define SysTimer_BLOCK_EN_MASK                     SysTimer_TimerHW__PM_ACT_MSK
    #define SysTimer_BLOCK_STBY_EN_MASK                SysTimer_TimerHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* Interrupt Count - Not valid for Fixed Function Block */
    #define SysTimer_CTRL_INTCNT_SHIFT                  0x00u
    /* Trigger Polarity - Not valid for Fixed Function Block */
    #define SysTimer_CTRL_TRIG_MODE_SHIFT               0x00u
    /* Trigger Enable - Not valid for Fixed Function Block */
    #define SysTimer_CTRL_TRIG_EN_SHIFT                 0x00u
    /* Capture Polarity - Not valid for Fixed Function Block */
    #define SysTimer_CTRL_CAP_MODE_SHIFT                0x00u
    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define SysTimer_CTRL_ENABLE_SHIFT                  0x00u

    /* Control Register Bit Masks */
    #define SysTimer_CTRL_ENABLE                        ((uint8)((uint8)0x01u << SysTimer_CTRL_ENABLE_SHIFT))

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define SysTimer_CTRL2_IRQ_SEL_SHIFT                 0x00u
    #define SysTimer_CTRL2_IRQ_SEL                      ((uint8)((uint8)0x01u << SysTimer_CTRL2_IRQ_SEL_SHIFT))

    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        /* As defined by Verilog Implementation */
        #define SysTimer_CTRL_MODE_SHIFT                 0x01u
        #define SysTimer_CTRL_MODE_MASK                 ((uint8)((uint8)0x07u << SysTimer_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Control3 Register Bit Locations */
        #define SysTimer_CTRL_RCOD_SHIFT        0x02u
        #define SysTimer_CTRL_ENBL_SHIFT        0x00u
        #define SysTimer_CTRL_MODE_SHIFT        0x00u

        /* Control3 Register Bit Masks */
        #define SysTimer_CTRL_RCOD_MASK  ((uint8)((uint8)0x03u << SysTimer_CTRL_RCOD_SHIFT)) /* ROD and COD bit masks */
        #define SysTimer_CTRL_ENBL_MASK  ((uint8)((uint8)0x80u << SysTimer_CTRL_ENBL_SHIFT)) /* HW_EN bit mask */
        #define SysTimer_CTRL_MODE_MASK  ((uint8)((uint8)0x03u << SysTimer_CTRL_MODE_SHIFT)) /* Run mode bit mask */

        #define SysTimer_CTRL_RCOD       ((uint8)((uint8)0x03u << SysTimer_CTRL_RCOD_SHIFT))
        #define SysTimer_CTRL_ENBL       ((uint8)((uint8)0x80u << SysTimer_CTRL_ENBL_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define SysTimer_RT1_SHIFT                       0x04u
    /* Sync TC and CMP bit masks */
    #define SysTimer_RT1_MASK                        ((uint8)((uint8)0x03u << SysTimer_RT1_SHIFT))
    #define SysTimer_SYNC                            ((uint8)((uint8)0x03u << SysTimer_RT1_SHIFT))
    #define SysTimer_SYNCDSI_SHIFT                   0x00u
    /* Sync all DSI inputs with Mask  */
    #define SysTimer_SYNCDSI_MASK                    ((uint8)((uint8)0x0Fu << SysTimer_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define SysTimer_SYNCDSI_EN                      ((uint8)((uint8)0x0Fu << SysTimer_SYNCDSI_SHIFT))

    #define SysTimer_CTRL_MODE_PULSEWIDTH            ((uint8)((uint8)0x01u << SysTimer_CTRL_MODE_SHIFT))
    #define SysTimer_CTRL_MODE_PERIOD                ((uint8)((uint8)0x02u << SysTimer_CTRL_MODE_SHIFT))
    #define SysTimer_CTRL_MODE_CONTINUOUS            ((uint8)((uint8)0x00u << SysTimer_CTRL_MODE_SHIFT))

    /* Status Register Bit Locations */
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define SysTimer_STATUS_TC_SHIFT                 0x07u
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define SysTimer_STATUS_CAPTURE_SHIFT            0x06u
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define SysTimer_STATUS_TC_INT_MASK_SHIFT        (SysTimer_STATUS_TC_SHIFT - 0x04u)
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define SysTimer_STATUS_CAPTURE_INT_MASK_SHIFT   (SysTimer_STATUS_CAPTURE_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define SysTimer_STATUS_TC                       ((uint8)((uint8)0x01u << SysTimer_STATUS_TC_SHIFT))
    #define SysTimer_STATUS_CAPTURE                  ((uint8)((uint8)0x01u << SysTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on TC */
    #define SysTimer_STATUS_TC_INT_MASK              ((uint8)((uint8)0x01u << SysTimer_STATUS_TC_INT_MASK_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on Capture */
    #define SysTimer_STATUS_CAPTURE_INT_MASK         ((uint8)((uint8)0x01u << SysTimer_STATUS_CAPTURE_INT_MASK_SHIFT))

#else   /* UDB Registers and Register Constants */


    /***************************************
    *           UDB Registers
    ***************************************/

    #define SysTimer_STATUS              (* (reg8 *) SysTimer_TimerUDB_rstSts_stsreg__STATUS_REG )
    #define SysTimer_STATUS_MASK         (* (reg8 *) SysTimer_TimerUDB_rstSts_stsreg__MASK_REG)
    #define SysTimer_STATUS_AUX_CTRL     (* (reg8 *) SysTimer_TimerUDB_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define SysTimer_CONTROL             (* (reg8 *) SysTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    
    #if(SysTimer_Resolution <= 8u) /* 8-bit Timer */
        #define SysTimer_CAPTURE_LSB         (* (reg8 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
        #define SysTimer_CAPTURE_LSB_PTR       ((reg8 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
        #define SysTimer_PERIOD_LSB          (* (reg8 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
        #define SysTimer_PERIOD_LSB_PTR        ((reg8 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
        #define SysTimer_COUNTER_LSB         (* (reg8 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
        #define SysTimer_COUNTER_LSB_PTR       ((reg8 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
    #elif(SysTimer_Resolution <= 16u) /* 8-bit Timer */
        #if(CY_PSOC3) /* 8-bit addres space */
            #define SysTimer_CAPTURE_LSB         (* (reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
            #define SysTimer_CAPTURE_LSB_PTR       ((reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
            #define SysTimer_PERIOD_LSB          (* (reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
            #define SysTimer_PERIOD_LSB_PTR        ((reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
            #define SysTimer_COUNTER_LSB         (* (reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
            #define SysTimer_COUNTER_LSB_PTR       ((reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define SysTimer_CAPTURE_LSB         (* (reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__16BIT_F0_REG )
            #define SysTimer_CAPTURE_LSB_PTR       ((reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__16BIT_F0_REG )
            #define SysTimer_PERIOD_LSB          (* (reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__16BIT_D0_REG )
            #define SysTimer_PERIOD_LSB_PTR        ((reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__16BIT_D0_REG )
            #define SysTimer_COUNTER_LSB         (* (reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__16BIT_A0_REG )
            #define SysTimer_COUNTER_LSB_PTR       ((reg16 *) SysTimer_TimerUDB_sT8_timerdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(SysTimer_Resolution <= 24u)/* 24-bit Timer */
        #define SysTimer_CAPTURE_LSB         (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
        #define SysTimer_CAPTURE_LSB_PTR       ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
        #define SysTimer_PERIOD_LSB          (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
        #define SysTimer_PERIOD_LSB_PTR        ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
        #define SysTimer_COUNTER_LSB         (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
        #define SysTimer_COUNTER_LSB_PTR       ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
    #else /* 32-bit Timer */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define SysTimer_CAPTURE_LSB         (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
            #define SysTimer_CAPTURE_LSB_PTR       ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__F0_REG )
            #define SysTimer_PERIOD_LSB          (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
            #define SysTimer_PERIOD_LSB_PTR        ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__D0_REG )
            #define SysTimer_COUNTER_LSB         (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
            #define SysTimer_COUNTER_LSB_PTR       ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define SysTimer_CAPTURE_LSB         (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__32BIT_F0_REG )
            #define SysTimer_CAPTURE_LSB_PTR       ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__32BIT_F0_REG )
            #define SysTimer_PERIOD_LSB          (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__32BIT_D0_REG )
            #define SysTimer_PERIOD_LSB_PTR        ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__32BIT_D0_REG )
            #define SysTimer_COUNTER_LSB         (* (reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__32BIT_A0_REG )
            #define SysTimer_COUNTER_LSB_PTR       ((reg32 *) SysTimer_TimerUDB_sT8_timerdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */ 
    #endif

    #define SysTimer_COUNTER_LSB_PTR_8BIT       ((reg8 *) SysTimer_TimerUDB_sT8_timerdp_u0__A0_REG )
    
    #if (SysTimer_UsingHWCaptureCounter)
        #define SysTimer_CAP_COUNT              (*(reg8 *) SysTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define SysTimer_CAP_COUNT_PTR          ( (reg8 *) SysTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define SysTimer_CAPTURE_COUNT_CTRL     (*(reg8 *) SysTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
        #define SysTimer_CAPTURE_COUNT_CTRL_PTR ( (reg8 *) SysTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
    #endif /* (SysTimer_UsingHWCaptureCounter) */


    /***************************************
    *       Register Constants
    ***************************************/

    /* Control Register Bit Locations */
    #define SysTimer_CTRL_INTCNT_SHIFT              0x00u       /* As defined by Verilog Implementation */
    #define SysTimer_CTRL_TRIG_MODE_SHIFT           0x02u       /* As defined by Verilog Implementation */
    #define SysTimer_CTRL_TRIG_EN_SHIFT             0x04u       /* As defined by Verilog Implementation */
    #define SysTimer_CTRL_CAP_MODE_SHIFT            0x05u       /* As defined by Verilog Implementation */
    #define SysTimer_CTRL_ENABLE_SHIFT              0x07u       /* As defined by Verilog Implementation */

    /* Control Register Bit Masks */
    #define SysTimer_CTRL_INTCNT_MASK               ((uint8)((uint8)0x03u << SysTimer_CTRL_INTCNT_SHIFT))
    #define SysTimer_CTRL_TRIG_MODE_MASK            ((uint8)((uint8)0x03u << SysTimer_CTRL_TRIG_MODE_SHIFT))
    #define SysTimer_CTRL_TRIG_EN                   ((uint8)((uint8)0x01u << SysTimer_CTRL_TRIG_EN_SHIFT))
    #define SysTimer_CTRL_CAP_MODE_MASK             ((uint8)((uint8)0x03u << SysTimer_CTRL_CAP_MODE_SHIFT))
    #define SysTimer_CTRL_ENABLE                    ((uint8)((uint8)0x01u << SysTimer_CTRL_ENABLE_SHIFT))

    /* Bit Counter (7-bit) Control Register Bit Definitions */
    /* As defined by the Register map for the AUX Control Register */
    #define SysTimer_CNTR_ENABLE                    0x20u

    /* Status Register Bit Locations */
    #define SysTimer_STATUS_TC_SHIFT                0x00u  /* As defined by Verilog Implementation */
    #define SysTimer_STATUS_CAPTURE_SHIFT           0x01u  /* As defined by Verilog Implementation */
    #define SysTimer_STATUS_TC_INT_MASK_SHIFT       SysTimer_STATUS_TC_SHIFT
    #define SysTimer_STATUS_CAPTURE_INT_MASK_SHIFT  SysTimer_STATUS_CAPTURE_SHIFT
    #define SysTimer_STATUS_FIFOFULL_SHIFT          0x02u  /* As defined by Verilog Implementation */
    #define SysTimer_STATUS_FIFONEMP_SHIFT          0x03u  /* As defined by Verilog Implementation */
    #define SysTimer_STATUS_FIFOFULL_INT_MASK_SHIFT SysTimer_STATUS_FIFOFULL_SHIFT

    /* Status Register Bit Masks */
    /* Sticky TC Event Bit-Mask */
    #define SysTimer_STATUS_TC                      ((uint8)((uint8)0x01u << SysTimer_STATUS_TC_SHIFT))
    /* Sticky Capture Event Bit-Mask */
    #define SysTimer_STATUS_CAPTURE                 ((uint8)((uint8)0x01u << SysTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define SysTimer_STATUS_TC_INT_MASK             ((uint8)((uint8)0x01u << SysTimer_STATUS_TC_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define SysTimer_STATUS_CAPTURE_INT_MASK        ((uint8)((uint8)0x01u << SysTimer_STATUS_CAPTURE_SHIFT))
    /* NOT-Sticky FIFO Full Bit-Mask */
    #define SysTimer_STATUS_FIFOFULL                ((uint8)((uint8)0x01u << SysTimer_STATUS_FIFOFULL_SHIFT))
    /* NOT-Sticky FIFO Not Empty Bit-Mask */
    #define SysTimer_STATUS_FIFONEMP                ((uint8)((uint8)0x01u << SysTimer_STATUS_FIFONEMP_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define SysTimer_STATUS_FIFOFULL_INT_MASK       ((uint8)((uint8)0x01u << SysTimer_STATUS_FIFOFULL_SHIFT))

    #define SysTimer_STATUS_ACTL_INT_EN             0x10u   /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define SysTimer_AUX_CTRL_FIFO0_CLR             0x01u   /* As defined by Register map */
    #define SysTimer_AUX_CTRL_FIFO1_CLR             0x02u   /* As defined by Register map */
    #define SysTimer_AUX_CTRL_FIFO0_LVL             0x04u   /* As defined by Register map */
    #define SysTimer_AUX_CTRL_FIFO1_LVL             0x08u   /* As defined by Register map */
    #define SysTimer_STATUS_ACTL_INT_EN_MASK        0x10u   /* As defined for the ACTL Register */

#endif /* Implementation Specific Registers and Register Constants */

#endif  /* CY_TIMER_SysTimer_H */


/* [] END OF FILE */
