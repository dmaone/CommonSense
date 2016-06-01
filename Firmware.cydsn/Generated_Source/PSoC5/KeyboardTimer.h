/*******************************************************************************
* File Name: KeyboardTimer.h
* Version 2.70
*
*  Description:
*     Contains the function prototypes and constants available to the timer
*     user module.
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_Timer_v2_60_KeyboardTimer_H)
#define CY_Timer_v2_60_KeyboardTimer_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 KeyboardTimer_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Timer_v2_70 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define KeyboardTimer_Resolution                 32u
#define KeyboardTimer_UsingFixedFunction         0u
#define KeyboardTimer_UsingHWCaptureCounter      0u
#define KeyboardTimer_SoftwareCaptureMode        0u
#define KeyboardTimer_SoftwareTriggerMode        0u
#define KeyboardTimer_UsingHWEnable              1u
#define KeyboardTimer_EnableTriggerMode          0u
#define KeyboardTimer_InterruptOnCaptureCount    0u
#define KeyboardTimer_RunModeUsed                0u
#define KeyboardTimer_ControlRegRemoved          1u

#if defined(KeyboardTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG)
    #define KeyboardTimer_UDB_CONTROL_REG_REMOVED            (0u)
#elif  (KeyboardTimer_UsingFixedFunction)
    #define KeyboardTimer_UDB_CONTROL_REG_REMOVED            (0u)
#else 
    #define KeyboardTimer_UDB_CONTROL_REG_REMOVED            (1u)
#endif /* End KeyboardTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG */


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 TimerEnableState;
    #if(!KeyboardTimer_UsingFixedFunction)

        uint32 TimerUdb;
        uint8 InterruptMaskValue;
        #if (KeyboardTimer_UsingHWCaptureCounter)
            uint8 TimerCaptureCounter;
        #endif /* variable declarations for backing up non retention registers in CY_UDB_V1 */

        #if (!KeyboardTimer_UDB_CONTROL_REG_REMOVED)
            uint8 TimerControlRegister;
        #endif /* variable declaration for backing up enable state of the Timer */
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */

}KeyboardTimer_backupStruct;


/***************************************
*       Function Prototypes
***************************************/

void    KeyboardTimer_Start(void) ;
void    KeyboardTimer_Stop(void) ;

void    KeyboardTimer_SetInterruptMode(uint8 interruptMode) ;
uint8   KeyboardTimer_ReadStatusRegister(void) ;
/* Deprecated function. Do not use this in future. Retained for backward compatibility */
#define KeyboardTimer_GetInterruptSource() KeyboardTimer_ReadStatusRegister()

#if(!KeyboardTimer_UDB_CONTROL_REG_REMOVED)
    uint8   KeyboardTimer_ReadControlRegister(void) ;
    void    KeyboardTimer_WriteControlRegister(uint8 control) ;
#endif /* (!KeyboardTimer_UDB_CONTROL_REG_REMOVED) */

uint32  KeyboardTimer_ReadPeriod(void) ;
void    KeyboardTimer_WritePeriod(uint32 period) ;
uint32  KeyboardTimer_ReadCounter(void) ;
void    KeyboardTimer_WriteCounter(uint32 counter) ;
uint32  KeyboardTimer_ReadCapture(void) ;
void    KeyboardTimer_SoftwareCapture(void) ;

#if(!KeyboardTimer_UsingFixedFunction) /* UDB Prototypes */
    #if (KeyboardTimer_SoftwareCaptureMode)
        void    KeyboardTimer_SetCaptureMode(uint8 captureMode) ;
    #endif /* (!KeyboardTimer_UsingFixedFunction) */

    #if (KeyboardTimer_SoftwareTriggerMode)
        void    KeyboardTimer_SetTriggerMode(uint8 triggerMode) ;
    #endif /* (KeyboardTimer_SoftwareTriggerMode) */

    #if (KeyboardTimer_EnableTriggerMode)
        void    KeyboardTimer_EnableTrigger(void) ;
        void    KeyboardTimer_DisableTrigger(void) ;
    #endif /* (KeyboardTimer_EnableTriggerMode) */


    #if(KeyboardTimer_InterruptOnCaptureCount)
        void    KeyboardTimer_SetInterruptCount(uint8 interruptCount) ;
    #endif /* (KeyboardTimer_InterruptOnCaptureCount) */

    #if (KeyboardTimer_UsingHWCaptureCounter)
        void    KeyboardTimer_SetCaptureCount(uint8 captureCount) ;
        uint8   KeyboardTimer_ReadCaptureCount(void) ;
    #endif /* (KeyboardTimer_UsingHWCaptureCounter) */

    void KeyboardTimer_ClearFIFO(void) ;
#endif /* UDB Prototypes */

/* Sleep Retention APIs */
void KeyboardTimer_Init(void)          ;
void KeyboardTimer_Enable(void)        ;
void KeyboardTimer_SaveConfig(void)    ;
void KeyboardTimer_RestoreConfig(void) ;
void KeyboardTimer_Sleep(void)         ;
void KeyboardTimer_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Timer__CaptureModes, Used in Capture Mode */
#define KeyboardTimer__B_TIMER__CM_NONE 0
#define KeyboardTimer__B_TIMER__CM_RISINGEDGE 1
#define KeyboardTimer__B_TIMER__CM_FALLINGEDGE 2
#define KeyboardTimer__B_TIMER__CM_EITHEREDGE 3
#define KeyboardTimer__B_TIMER__CM_SOFTWARE 4



/* Enumerated Type B_Timer__TriggerModes, Used in Trigger Mode */
#define KeyboardTimer__B_TIMER__TM_NONE 0x00u
#define KeyboardTimer__B_TIMER__TM_RISINGEDGE 0x04u
#define KeyboardTimer__B_TIMER__TM_FALLINGEDGE 0x08u
#define KeyboardTimer__B_TIMER__TM_EITHEREDGE 0x0Cu
#define KeyboardTimer__B_TIMER__TM_SOFTWARE 0x10u


/***************************************
*    Initialial Parameter Constants
***************************************/

#define KeyboardTimer_INIT_PERIOD             4294967295u
#define KeyboardTimer_INIT_CAPTURE_MODE       ((uint8)((uint8)0u << KeyboardTimer_CTRL_CAP_MODE_SHIFT))
#define KeyboardTimer_INIT_TRIGGER_MODE       ((uint8)((uint8)0u << KeyboardTimer_CTRL_TRIG_MODE_SHIFT))
#if (KeyboardTimer_UsingFixedFunction)
    #define KeyboardTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << KeyboardTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                  ((uint8)((uint8)0 << KeyboardTimer_STATUS_CAPTURE_INT_MASK_SHIFT)))
#else
    #define KeyboardTimer_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << KeyboardTimer_STATUS_TC_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << KeyboardTimer_STATUS_CAPTURE_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << KeyboardTimer_STATUS_FIFOFULL_INT_MASK_SHIFT)))
#endif /* (KeyboardTimer_UsingFixedFunction) */
#define KeyboardTimer_INIT_CAPTURE_COUNT      (2u)
#define KeyboardTimer_INIT_INT_CAPTURE_COUNT  ((uint8)((uint8)(1u - 1u) << KeyboardTimer_CTRL_INTCNT_SHIFT))


/***************************************
*           Registers
***************************************/

#if (KeyboardTimer_UsingFixedFunction) /* Implementation Specific Registers and Register Constants */


    /***************************************
    *    Fixed Function Registers
    ***************************************/

    #define KeyboardTimer_STATUS         (*(reg8 *) KeyboardTimer_TimerHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define KeyboardTimer_STATUS_MASK    (*(reg8 *) KeyboardTimer_TimerHW__SR0 )
    #define KeyboardTimer_CONTROL        (*(reg8 *) KeyboardTimer_TimerHW__CFG0)
    #define KeyboardTimer_CONTROL2       (*(reg8 *) KeyboardTimer_TimerHW__CFG1)
    #define KeyboardTimer_CONTROL2_PTR   ( (reg8 *) KeyboardTimer_TimerHW__CFG1)
    #define KeyboardTimer_RT1            (*(reg8 *) KeyboardTimer_TimerHW__RT1)
    #define KeyboardTimer_RT1_PTR        ( (reg8 *) KeyboardTimer_TimerHW__RT1)

    #if (CY_PSOC3 || CY_PSOC5LP)
        #define KeyboardTimer_CONTROL3       (*(reg8 *) KeyboardTimer_TimerHW__CFG2)
        #define KeyboardTimer_CONTROL3_PTR   ( (reg8 *) KeyboardTimer_TimerHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define KeyboardTimer_GLOBAL_ENABLE  (*(reg8 *) KeyboardTimer_TimerHW__PM_ACT_CFG)
    #define KeyboardTimer_GLOBAL_STBY_ENABLE  (*(reg8 *) KeyboardTimer_TimerHW__PM_STBY_CFG)

    #define KeyboardTimer_CAPTURE_LSB         (* (reg16 *) KeyboardTimer_TimerHW__CAP0 )
    #define KeyboardTimer_CAPTURE_LSB_PTR       ((reg16 *) KeyboardTimer_TimerHW__CAP0 )
    #define KeyboardTimer_PERIOD_LSB          (* (reg16 *) KeyboardTimer_TimerHW__PER0 )
    #define KeyboardTimer_PERIOD_LSB_PTR        ((reg16 *) KeyboardTimer_TimerHW__PER0 )
    #define KeyboardTimer_COUNTER_LSB         (* (reg16 *) KeyboardTimer_TimerHW__CNT_CMP0 )
    #define KeyboardTimer_COUNTER_LSB_PTR       ((reg16 *) KeyboardTimer_TimerHW__CNT_CMP0 )


    /***************************************
    *    Register Constants
    ***************************************/

    /* Fixed Function Block Chosen */
    #define KeyboardTimer_BLOCK_EN_MASK                     KeyboardTimer_TimerHW__PM_ACT_MSK
    #define KeyboardTimer_BLOCK_STBY_EN_MASK                KeyboardTimer_TimerHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* Interrupt Count - Not valid for Fixed Function Block */
    #define KeyboardTimer_CTRL_INTCNT_SHIFT                  0x00u
    /* Trigger Polarity - Not valid for Fixed Function Block */
    #define KeyboardTimer_CTRL_TRIG_MODE_SHIFT               0x00u
    /* Trigger Enable - Not valid for Fixed Function Block */
    #define KeyboardTimer_CTRL_TRIG_EN_SHIFT                 0x00u
    /* Capture Polarity - Not valid for Fixed Function Block */
    #define KeyboardTimer_CTRL_CAP_MODE_SHIFT                0x00u
    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define KeyboardTimer_CTRL_ENABLE_SHIFT                  0x00u

    /* Control Register Bit Masks */
    #define KeyboardTimer_CTRL_ENABLE                        ((uint8)((uint8)0x01u << KeyboardTimer_CTRL_ENABLE_SHIFT))

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define KeyboardTimer_CTRL2_IRQ_SEL_SHIFT                 0x00u
    #define KeyboardTimer_CTRL2_IRQ_SEL                      ((uint8)((uint8)0x01u << KeyboardTimer_CTRL2_IRQ_SEL_SHIFT))

    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        /* As defined by Verilog Implementation */
        #define KeyboardTimer_CTRL_MODE_SHIFT                 0x01u
        #define KeyboardTimer_CTRL_MODE_MASK                 ((uint8)((uint8)0x07u << KeyboardTimer_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Control3 Register Bit Locations */
        #define KeyboardTimer_CTRL_RCOD_SHIFT        0x02u
        #define KeyboardTimer_CTRL_ENBL_SHIFT        0x00u
        #define KeyboardTimer_CTRL_MODE_SHIFT        0x00u

        /* Control3 Register Bit Masks */
        #define KeyboardTimer_CTRL_RCOD_MASK  ((uint8)((uint8)0x03u << KeyboardTimer_CTRL_RCOD_SHIFT)) /* ROD and COD bit masks */
        #define KeyboardTimer_CTRL_ENBL_MASK  ((uint8)((uint8)0x80u << KeyboardTimer_CTRL_ENBL_SHIFT)) /* HW_EN bit mask */
        #define KeyboardTimer_CTRL_MODE_MASK  ((uint8)((uint8)0x03u << KeyboardTimer_CTRL_MODE_SHIFT)) /* Run mode bit mask */

        #define KeyboardTimer_CTRL_RCOD       ((uint8)((uint8)0x03u << KeyboardTimer_CTRL_RCOD_SHIFT))
        #define KeyboardTimer_CTRL_ENBL       ((uint8)((uint8)0x80u << KeyboardTimer_CTRL_ENBL_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define KeyboardTimer_RT1_SHIFT                       0x04u
    /* Sync TC and CMP bit masks */
    #define KeyboardTimer_RT1_MASK                        ((uint8)((uint8)0x03u << KeyboardTimer_RT1_SHIFT))
    #define KeyboardTimer_SYNC                            ((uint8)((uint8)0x03u << KeyboardTimer_RT1_SHIFT))
    #define KeyboardTimer_SYNCDSI_SHIFT                   0x00u
    /* Sync all DSI inputs with Mask  */
    #define KeyboardTimer_SYNCDSI_MASK                    ((uint8)((uint8)0x0Fu << KeyboardTimer_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define KeyboardTimer_SYNCDSI_EN                      ((uint8)((uint8)0x0Fu << KeyboardTimer_SYNCDSI_SHIFT))

    #define KeyboardTimer_CTRL_MODE_PULSEWIDTH            ((uint8)((uint8)0x01u << KeyboardTimer_CTRL_MODE_SHIFT))
    #define KeyboardTimer_CTRL_MODE_PERIOD                ((uint8)((uint8)0x02u << KeyboardTimer_CTRL_MODE_SHIFT))
    #define KeyboardTimer_CTRL_MODE_CONTINUOUS            ((uint8)((uint8)0x00u << KeyboardTimer_CTRL_MODE_SHIFT))

    /* Status Register Bit Locations */
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define KeyboardTimer_STATUS_TC_SHIFT                 0x07u
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define KeyboardTimer_STATUS_CAPTURE_SHIFT            0x06u
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define KeyboardTimer_STATUS_TC_INT_MASK_SHIFT        (KeyboardTimer_STATUS_TC_SHIFT - 0x04u)
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define KeyboardTimer_STATUS_CAPTURE_INT_MASK_SHIFT   (KeyboardTimer_STATUS_CAPTURE_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define KeyboardTimer_STATUS_TC                       ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_TC_SHIFT))
    #define KeyboardTimer_STATUS_CAPTURE                  ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on TC */
    #define KeyboardTimer_STATUS_TC_INT_MASK              ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_TC_INT_MASK_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on Capture */
    #define KeyboardTimer_STATUS_CAPTURE_INT_MASK         ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_CAPTURE_INT_MASK_SHIFT))

#else   /* UDB Registers and Register Constants */


    /***************************************
    *           UDB Registers
    ***************************************/

    #define KeyboardTimer_STATUS              (* (reg8 *) KeyboardTimer_TimerUDB_rstSts_stsreg__STATUS_REG )
    #define KeyboardTimer_STATUS_MASK         (* (reg8 *) KeyboardTimer_TimerUDB_rstSts_stsreg__MASK_REG)
    #define KeyboardTimer_STATUS_AUX_CTRL     (* (reg8 *) KeyboardTimer_TimerUDB_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define KeyboardTimer_CONTROL             (* (reg8 *) KeyboardTimer_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    
    #if(KeyboardTimer_Resolution <= 8u) /* 8-bit Timer */
        #define KeyboardTimer_CAPTURE_LSB         (* (reg8 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
        #define KeyboardTimer_CAPTURE_LSB_PTR       ((reg8 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
        #define KeyboardTimer_PERIOD_LSB          (* (reg8 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
        #define KeyboardTimer_PERIOD_LSB_PTR        ((reg8 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
        #define KeyboardTimer_COUNTER_LSB         (* (reg8 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
        #define KeyboardTimer_COUNTER_LSB_PTR       ((reg8 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
    #elif(KeyboardTimer_Resolution <= 16u) /* 8-bit Timer */
        #if(CY_PSOC3) /* 8-bit addres space */
            #define KeyboardTimer_CAPTURE_LSB         (* (reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
            #define KeyboardTimer_CAPTURE_LSB_PTR       ((reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
            #define KeyboardTimer_PERIOD_LSB          (* (reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
            #define KeyboardTimer_PERIOD_LSB_PTR        ((reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
            #define KeyboardTimer_COUNTER_LSB         (* (reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
            #define KeyboardTimer_COUNTER_LSB_PTR       ((reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define KeyboardTimer_CAPTURE_LSB         (* (reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__16BIT_F0_REG )
            #define KeyboardTimer_CAPTURE_LSB_PTR       ((reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__16BIT_F0_REG )
            #define KeyboardTimer_PERIOD_LSB          (* (reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__16BIT_D0_REG )
            #define KeyboardTimer_PERIOD_LSB_PTR        ((reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__16BIT_D0_REG )
            #define KeyboardTimer_COUNTER_LSB         (* (reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__16BIT_A0_REG )
            #define KeyboardTimer_COUNTER_LSB_PTR       ((reg16 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(KeyboardTimer_Resolution <= 24u)/* 24-bit Timer */
        #define KeyboardTimer_CAPTURE_LSB         (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
        #define KeyboardTimer_CAPTURE_LSB_PTR       ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
        #define KeyboardTimer_PERIOD_LSB          (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
        #define KeyboardTimer_PERIOD_LSB_PTR        ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
        #define KeyboardTimer_COUNTER_LSB         (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
        #define KeyboardTimer_COUNTER_LSB_PTR       ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
    #else /* 32-bit Timer */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define KeyboardTimer_CAPTURE_LSB         (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
            #define KeyboardTimer_CAPTURE_LSB_PTR       ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__F0_REG )
            #define KeyboardTimer_PERIOD_LSB          (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
            #define KeyboardTimer_PERIOD_LSB_PTR        ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__D0_REG )
            #define KeyboardTimer_COUNTER_LSB         (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
            #define KeyboardTimer_COUNTER_LSB_PTR       ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define KeyboardTimer_CAPTURE_LSB         (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__32BIT_F0_REG )
            #define KeyboardTimer_CAPTURE_LSB_PTR       ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__32BIT_F0_REG )
            #define KeyboardTimer_PERIOD_LSB          (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__32BIT_D0_REG )
            #define KeyboardTimer_PERIOD_LSB_PTR        ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__32BIT_D0_REG )
            #define KeyboardTimer_COUNTER_LSB         (* (reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__32BIT_A0_REG )
            #define KeyboardTimer_COUNTER_LSB_PTR       ((reg32 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */ 
    #endif

    #define KeyboardTimer_COUNTER_LSB_PTR_8BIT       ((reg8 *) KeyboardTimer_TimerUDB_sT32_timerdp_u0__A0_REG )
    
    #if (KeyboardTimer_UsingHWCaptureCounter)
        #define KeyboardTimer_CAP_COUNT              (*(reg8 *) KeyboardTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define KeyboardTimer_CAP_COUNT_PTR          ( (reg8 *) KeyboardTimer_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define KeyboardTimer_CAPTURE_COUNT_CTRL     (*(reg8 *) KeyboardTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
        #define KeyboardTimer_CAPTURE_COUNT_CTRL_PTR ( (reg8 *) KeyboardTimer_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
    #endif /* (KeyboardTimer_UsingHWCaptureCounter) */


    /***************************************
    *       Register Constants
    ***************************************/

    /* Control Register Bit Locations */
    #define KeyboardTimer_CTRL_INTCNT_SHIFT              0x00u       /* As defined by Verilog Implementation */
    #define KeyboardTimer_CTRL_TRIG_MODE_SHIFT           0x02u       /* As defined by Verilog Implementation */
    #define KeyboardTimer_CTRL_TRIG_EN_SHIFT             0x04u       /* As defined by Verilog Implementation */
    #define KeyboardTimer_CTRL_CAP_MODE_SHIFT            0x05u       /* As defined by Verilog Implementation */
    #define KeyboardTimer_CTRL_ENABLE_SHIFT              0x07u       /* As defined by Verilog Implementation */

    /* Control Register Bit Masks */
    #define KeyboardTimer_CTRL_INTCNT_MASK               ((uint8)((uint8)0x03u << KeyboardTimer_CTRL_INTCNT_SHIFT))
    #define KeyboardTimer_CTRL_TRIG_MODE_MASK            ((uint8)((uint8)0x03u << KeyboardTimer_CTRL_TRIG_MODE_SHIFT))
    #define KeyboardTimer_CTRL_TRIG_EN                   ((uint8)((uint8)0x01u << KeyboardTimer_CTRL_TRIG_EN_SHIFT))
    #define KeyboardTimer_CTRL_CAP_MODE_MASK             ((uint8)((uint8)0x03u << KeyboardTimer_CTRL_CAP_MODE_SHIFT))
    #define KeyboardTimer_CTRL_ENABLE                    ((uint8)((uint8)0x01u << KeyboardTimer_CTRL_ENABLE_SHIFT))

    /* Bit Counter (7-bit) Control Register Bit Definitions */
    /* As defined by the Register map for the AUX Control Register */
    #define KeyboardTimer_CNTR_ENABLE                    0x20u

    /* Status Register Bit Locations */
    #define KeyboardTimer_STATUS_TC_SHIFT                0x00u  /* As defined by Verilog Implementation */
    #define KeyboardTimer_STATUS_CAPTURE_SHIFT           0x01u  /* As defined by Verilog Implementation */
    #define KeyboardTimer_STATUS_TC_INT_MASK_SHIFT       KeyboardTimer_STATUS_TC_SHIFT
    #define KeyboardTimer_STATUS_CAPTURE_INT_MASK_SHIFT  KeyboardTimer_STATUS_CAPTURE_SHIFT
    #define KeyboardTimer_STATUS_FIFOFULL_SHIFT          0x02u  /* As defined by Verilog Implementation */
    #define KeyboardTimer_STATUS_FIFONEMP_SHIFT          0x03u  /* As defined by Verilog Implementation */
    #define KeyboardTimer_STATUS_FIFOFULL_INT_MASK_SHIFT KeyboardTimer_STATUS_FIFOFULL_SHIFT

    /* Status Register Bit Masks */
    /* Sticky TC Event Bit-Mask */
    #define KeyboardTimer_STATUS_TC                      ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_TC_SHIFT))
    /* Sticky Capture Event Bit-Mask */
    #define KeyboardTimer_STATUS_CAPTURE                 ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define KeyboardTimer_STATUS_TC_INT_MASK             ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_TC_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define KeyboardTimer_STATUS_CAPTURE_INT_MASK        ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_CAPTURE_SHIFT))
    /* NOT-Sticky FIFO Full Bit-Mask */
    #define KeyboardTimer_STATUS_FIFOFULL                ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_FIFOFULL_SHIFT))
    /* NOT-Sticky FIFO Not Empty Bit-Mask */
    #define KeyboardTimer_STATUS_FIFONEMP                ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_FIFONEMP_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define KeyboardTimer_STATUS_FIFOFULL_INT_MASK       ((uint8)((uint8)0x01u << KeyboardTimer_STATUS_FIFOFULL_SHIFT))

    #define KeyboardTimer_STATUS_ACTL_INT_EN             0x10u   /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define KeyboardTimer_AUX_CTRL_FIFO0_CLR             0x01u   /* As defined by Register map */
    #define KeyboardTimer_AUX_CTRL_FIFO1_CLR             0x02u   /* As defined by Register map */
    #define KeyboardTimer_AUX_CTRL_FIFO0_LVL             0x04u   /* As defined by Register map */
    #define KeyboardTimer_AUX_CTRL_FIFO1_LVL             0x08u   /* As defined by Register map */
    #define KeyboardTimer_STATUS_ACTL_INT_EN_MASK        0x10u   /* As defined for the ACTL Register */

#endif /* Implementation Specific Registers and Register Constants */

#endif  /* CY_Timer_v2_30_KeyboardTimer_H */


/* [] END OF FILE */
