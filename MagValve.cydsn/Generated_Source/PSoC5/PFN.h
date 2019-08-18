/*******************************************************************************
* File Name: PFN.h
* Version 3.30
*
* Description:
*  Contains the prototypes and constants for the functions available to the
*  PWM user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_PWM_PFN_H)
#define CY_PWM_PFN_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 PFN_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define PFN_Resolution                     (8u)
#define PFN_UsingFixedFunction             (0u)
#define PFN_DeadBandMode                   (0u)
#define PFN_KillModeMinTime                (0u)
#define PFN_KillMode                       (0u)
#define PFN_PWMMode                        (0u)
#define PFN_PWMModeIsCenterAligned         (0u)
#define PFN_DeadBandUsed                   (0u)
#define PFN_DeadBand2_4                    (0u)

#if !defined(PFN_PWMUDB_genblk8_stsreg__REMOVED)
    #define PFN_UseStatus                  (0u)
#else
    #define PFN_UseStatus                  (0u)
#endif /* !defined(PFN_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(PFN_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define PFN_UseControl                 (0u)
#else
    #define PFN_UseControl                 (0u)
#endif /* !defined(PFN_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define PFN_UseOneCompareMode              (1u)
#define PFN_MinimumKillTime                (1u)
#define PFN_EnableMode                     (1u)

#define PFN_CompareMode1SW                 (0u)
#define PFN_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define PFN__B_PWM__DISABLED 0
#define PFN__B_PWM__ASYNCHRONOUS 1
#define PFN__B_PWM__SINGLECYCLE 2
#define PFN__B_PWM__LATCHED 3
#define PFN__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define PFN__B_PWM__DBMDISABLED 0
#define PFN__B_PWM__DBM_2_4_CLOCKS 1
#define PFN__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define PFN__B_PWM__ONE_OUTPUT 0
#define PFN__B_PWM__TWO_OUTPUTS 1
#define PFN__B_PWM__DUAL_EDGE 2
#define PFN__B_PWM__CENTER_ALIGN 3
#define PFN__B_PWM__DITHER 5
#define PFN__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define PFN__B_PWM__LESS_THAN 1
#define PFN__B_PWM__LESS_THAN_OR_EQUAL 2
#define PFN__B_PWM__GREATER_THAN 3
#define PFN__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define PFN__B_PWM__EQUAL 0
#define PFN__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!PFN_UsingFixedFunction)
        uint8 PWMUdb;               /* PWM Current Counter value  */
        #if(!PFN_PWMModeIsCenterAligned)
            uint8 PWMPeriod;
        #endif /* (!PFN_PWMModeIsCenterAligned) */
        #if (PFN_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (PFN_UseStatus) */

        /* Backup for Deadband parameters */
        #if(PFN_DeadBandMode == PFN__B_PWM__DBM_256_CLOCKS || \
            PFN_DeadBandMode == PFN__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(PFN_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (PFN_KillModeMinTime) */

        /* Backup control register */
        #if(PFN_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (PFN_UseControl) */

    #endif /* (!PFN_UsingFixedFunction) */

}PFN_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    PFN_Start(void) ;
void    PFN_Stop(void) ;

#if (PFN_UseStatus || PFN_UsingFixedFunction)
    void  PFN_SetInterruptMode(uint8 interruptMode) ;
    uint8 PFN_ReadStatusRegister(void) ;
#endif /* (PFN_UseStatus || PFN_UsingFixedFunction) */

#define PFN_GetInterruptSource() PFN_ReadStatusRegister()

#if (PFN_UseControl)
    uint8 PFN_ReadControlRegister(void) ;
    void  PFN_WriteControlRegister(uint8 control)
          ;
#endif /* (PFN_UseControl) */

#if (PFN_UseOneCompareMode)
   #if (PFN_CompareMode1SW)
       void    PFN_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (PFN_CompareMode1SW) */
#else
    #if (PFN_CompareMode1SW)
        void    PFN_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (PFN_CompareMode1SW) */
    #if (PFN_CompareMode2SW)
        void    PFN_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (PFN_CompareMode2SW) */
#endif /* (PFN_UseOneCompareMode) */

#if (!PFN_UsingFixedFunction)
    uint8   PFN_ReadCounter(void) ;
    uint8 PFN_ReadCapture(void) ;

    #if (PFN_UseStatus)
            void PFN_ClearFIFO(void) ;
    #endif /* (PFN_UseStatus) */

    void    PFN_WriteCounter(uint8 counter)
            ;
#endif /* (!PFN_UsingFixedFunction) */

void    PFN_WritePeriod(uint8 period)
        ;
uint8 PFN_ReadPeriod(void) ;

#if (PFN_UseOneCompareMode)
    void    PFN_WriteCompare(uint8 compare)
            ;
    uint8 PFN_ReadCompare(void) ;
#else
    void    PFN_WriteCompare1(uint8 compare)
            ;
    uint8 PFN_ReadCompare1(void) ;
    void    PFN_WriteCompare2(uint8 compare)
            ;
    uint8 PFN_ReadCompare2(void) ;
#endif /* (PFN_UseOneCompareMode) */


#if (PFN_DeadBandUsed)
    void    PFN_WriteDeadTime(uint8 deadtime) ;
    uint8   PFN_ReadDeadTime(void) ;
#endif /* (PFN_DeadBandUsed) */

#if ( PFN_KillModeMinTime)
    void PFN_WriteKillTime(uint8 killtime) ;
    uint8 PFN_ReadKillTime(void) ;
#endif /* ( PFN_KillModeMinTime) */

void PFN_Init(void) ;
void PFN_Enable(void) ;
void PFN_Sleep(void) ;
void PFN_Wakeup(void) ;
void PFN_SaveConfig(void) ;
void PFN_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define PFN_INIT_PERIOD_VALUE          (30u)
#define PFN_INIT_COMPARE_VALUE1        (1u)
#define PFN_INIT_COMPARE_VALUE2        (128u)
#define PFN_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    PFN_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    PFN_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    PFN_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    PFN_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define PFN_DEFAULT_COMPARE2_MODE      (uint8)((uint8)2u <<  PFN_CTRL_CMPMODE2_SHIFT)
#define PFN_DEFAULT_COMPARE1_MODE      (uint8)((uint8)1u <<  PFN_CTRL_CMPMODE1_SHIFT)
#define PFN_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (PFN_UsingFixedFunction)
   #define PFN_PERIOD_LSB              (*(reg16 *) PFN_PWMHW__PER0)
   #define PFN_PERIOD_LSB_PTR          ( (reg16 *) PFN_PWMHW__PER0)
   #define PFN_COMPARE1_LSB            (*(reg16 *) PFN_PWMHW__CNT_CMP0)
   #define PFN_COMPARE1_LSB_PTR        ( (reg16 *) PFN_PWMHW__CNT_CMP0)
   #define PFN_COMPARE2_LSB            (0x00u)
   #define PFN_COMPARE2_LSB_PTR        (0x00u)
   #define PFN_COUNTER_LSB             (*(reg16 *) PFN_PWMHW__CNT_CMP0)
   #define PFN_COUNTER_LSB_PTR         ( (reg16 *) PFN_PWMHW__CNT_CMP0)
   #define PFN_CAPTURE_LSB             (*(reg16 *) PFN_PWMHW__CAP0)
   #define PFN_CAPTURE_LSB_PTR         ( (reg16 *) PFN_PWMHW__CAP0)
   #define PFN_RT1                     (*(reg8 *)  PFN_PWMHW__RT1)
   #define PFN_RT1_PTR                 ( (reg8 *)  PFN_PWMHW__RT1)

#else
   #if (PFN_Resolution == 8u) /* 8bit - PWM */

       #if(PFN_PWMModeIsCenterAligned)
           #define PFN_PERIOD_LSB      (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
           #define PFN_PERIOD_LSB_PTR  ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #else
           #define PFN_PERIOD_LSB      (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__F0_REG)
           #define PFN_PERIOD_LSB_PTR  ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__F0_REG)
       #endif /* (PFN_PWMModeIsCenterAligned) */

       #define PFN_COMPARE1_LSB        (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define PFN_COMPARE1_LSB_PTR    ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define PFN_COMPARE2_LSB        (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define PFN_COMPARE2_LSB_PTR    ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define PFN_COUNTERCAP_LSB      (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define PFN_COUNTERCAP_LSB_PTR  ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define PFN_COUNTER_LSB         (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define PFN_COUNTER_LSB_PTR     ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define PFN_CAPTURE_LSB         (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__F1_REG)
       #define PFN_CAPTURE_LSB_PTR     ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(PFN_PWMModeIsCenterAligned)
               #define PFN_PERIOD_LSB      (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
               #define PFN_PERIOD_LSB_PTR  ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #else
               #define PFN_PERIOD_LSB      (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__F0_REG)
               #define PFN_PERIOD_LSB_PTR  ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__F0_REG)
            #endif /* (PFN_PWMModeIsCenterAligned) */

            #define PFN_COMPARE1_LSB       (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define PFN_COMPARE1_LSB_PTR   ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define PFN_COMPARE2_LSB       (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define PFN_COMPARE2_LSB_PTR   ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define PFN_COUNTERCAP_LSB     (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define PFN_COUNTERCAP_LSB_PTR ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define PFN_COUNTER_LSB        (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define PFN_COUNTER_LSB_PTR    ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define PFN_CAPTURE_LSB        (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__F1_REG)
            #define PFN_CAPTURE_LSB_PTR    ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__F1_REG)
        #else
            #if(PFN_PWMModeIsCenterAligned)
               #define PFN_PERIOD_LSB      (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
               #define PFN_PERIOD_LSB_PTR  ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #else
               #define PFN_PERIOD_LSB      (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
               #define PFN_PERIOD_LSB_PTR  ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
            #endif /* (PFN_PWMModeIsCenterAligned) */

            #define PFN_COMPARE1_LSB       (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define PFN_COMPARE1_LSB_PTR   ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define PFN_COMPARE2_LSB       (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define PFN_COMPARE2_LSB_PTR   ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define PFN_COUNTERCAP_LSB     (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define PFN_COUNTERCAP_LSB_PTR ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define PFN_COUNTER_LSB        (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define PFN_COUNTER_LSB_PTR    ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define PFN_CAPTURE_LSB        (*(reg16 *) PFN_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
            #define PFN_CAPTURE_LSB_PTR    ((reg16 *)  PFN_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define PFN_AUX_CONTROLDP1          (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)
       #define PFN_AUX_CONTROLDP1_PTR      ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (PFN_Resolution == 8) */

   #define PFN_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__A1_REG)
   #define PFN_AUX_CONTROLDP0          (*(reg8 *)  PFN_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)
   #define PFN_AUX_CONTROLDP0_PTR      ((reg8 *)   PFN_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (PFN_UsingFixedFunction) */

#if(PFN_KillModeMinTime )
    #define PFN_KILLMODEMINTIME        (*(reg8 *)  PFN_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define PFN_KILLMODEMINTIME_PTR    ((reg8 *)   PFN_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (PFN_KillModeMinTime ) */

#if(PFN_DeadBandMode == PFN__B_PWM__DBM_256_CLOCKS)
    #define PFN_DEADBAND_COUNT         (*(reg8 *)  PFN_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define PFN_DEADBAND_COUNT_PTR     ((reg8 *)   PFN_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define PFN_DEADBAND_LSB_PTR       ((reg8 *)   PFN_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define PFN_DEADBAND_LSB           (*(reg8 *)  PFN_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(PFN_DeadBandMode == PFN__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (PFN_UsingFixedFunction)
        #define PFN_DEADBAND_COUNT         (*(reg8 *)  PFN_PWMHW__CFG0)
        #define PFN_DEADBAND_COUNT_PTR     ((reg8 *)   PFN_PWMHW__CFG0)
        #define PFN_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << PFN_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define PFN_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define PFN_DEADBAND_COUNT         (*(reg8 *)  PFN_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define PFN_DEADBAND_COUNT_PTR     ((reg8 *)   PFN_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define PFN_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << PFN_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define PFN_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (PFN_UsingFixedFunction) */
#endif /* (PFN_DeadBandMode == PFN__B_PWM__DBM_256_CLOCKS) */



#if (PFN_UsingFixedFunction)
    #define PFN_STATUS                 (*(reg8 *) PFN_PWMHW__SR0)
    #define PFN_STATUS_PTR             ((reg8 *) PFN_PWMHW__SR0)
    #define PFN_STATUS_MASK            (*(reg8 *) PFN_PWMHW__SR0)
    #define PFN_STATUS_MASK_PTR        ((reg8 *) PFN_PWMHW__SR0)
    #define PFN_CONTROL                (*(reg8 *) PFN_PWMHW__CFG0)
    #define PFN_CONTROL_PTR            ((reg8 *) PFN_PWMHW__CFG0)
    #define PFN_CONTROL2               (*(reg8 *) PFN_PWMHW__CFG1)
    #define PFN_CONTROL3               (*(reg8 *) PFN_PWMHW__CFG2)
    #define PFN_GLOBAL_ENABLE          (*(reg8 *) PFN_PWMHW__PM_ACT_CFG)
    #define PFN_GLOBAL_ENABLE_PTR      ( (reg8 *) PFN_PWMHW__PM_ACT_CFG)
    #define PFN_GLOBAL_STBY_ENABLE     (*(reg8 *) PFN_PWMHW__PM_STBY_CFG)
    #define PFN_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) PFN_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define PFN_BLOCK_EN_MASK          (PFN_PWMHW__PM_ACT_MSK)
    #define PFN_BLOCK_STBY_EN_MASK     (PFN_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define PFN_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define PFN_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define PFN_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define PFN_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define PFN_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define PFN_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define PFN_CTRL_ENABLE            (uint8)((uint8)0x01u << PFN_CTRL_ENABLE_SHIFT)
    #define PFN_CTRL_RESET             (uint8)((uint8)0x01u << PFN_CTRL_RESET_SHIFT)
    #define PFN_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << PFN_CTRL_CMPMODE2_SHIFT)
    #define PFN_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << PFN_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define PFN_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define PFN_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << PFN_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define PFN_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define PFN_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define PFN_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define PFN_STATUS_TC_INT_EN_MASK_SHIFT            (PFN_STATUS_TC_SHIFT - 4u)
    #define PFN_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define PFN_STATUS_CMP1_INT_EN_MASK_SHIFT          (PFN_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define PFN_STATUS_TC              (uint8)((uint8)0x01u << PFN_STATUS_TC_SHIFT)
    #define PFN_STATUS_CMP1            (uint8)((uint8)0x01u << PFN_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define PFN_STATUS_TC_INT_EN_MASK              (uint8)((uint8)PFN_STATUS_TC >> 4u)
    #define PFN_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)PFN_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define PFN_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define PFN_RT1_MASK              (uint8)((uint8)0x03u << PFN_RT1_SHIFT)
    #define PFN_SYNC                  (uint8)((uint8)0x03u << PFN_RT1_SHIFT)
    #define PFN_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define PFN_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << PFN_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define PFN_SYNCDSI_EN            (uint8)((uint8)0x0Fu << PFN_SYNCDSI_SHIFT)


#else
    #define PFN_STATUS                (*(reg8 *)   PFN_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define PFN_STATUS_PTR            ((reg8 *)    PFN_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define PFN_STATUS_MASK           (*(reg8 *)   PFN_PWMUDB_genblk8_stsreg__MASK_REG)
    #define PFN_STATUS_MASK_PTR       ((reg8 *)    PFN_PWMUDB_genblk8_stsreg__MASK_REG)
    #define PFN_STATUS_AUX_CTRL       (*(reg8 *)   PFN_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define PFN_CONTROL               (*(reg8 *)   PFN_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define PFN_CONTROL_PTR           ((reg8 *)    PFN_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define PFN_CTRL_ENABLE_SHIFT      (0x07u)
    #define PFN_CTRL_RESET_SHIFT       (0x06u)
    #define PFN_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define PFN_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define PFN_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define PFN_CTRL_ENABLE            (uint8)((uint8)0x01u << PFN_CTRL_ENABLE_SHIFT)
    #define PFN_CTRL_RESET             (uint8)((uint8)0x01u << PFN_CTRL_RESET_SHIFT)
    #define PFN_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << PFN_CTRL_CMPMODE2_SHIFT)
    #define PFN_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << PFN_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define PFN_STATUS_KILL_SHIFT          (0x05u)
    #define PFN_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define PFN_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define PFN_STATUS_TC_SHIFT            (0x02u)
    #define PFN_STATUS_CMP2_SHIFT          (0x01u)
    #define PFN_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define PFN_STATUS_KILL_INT_EN_MASK_SHIFT          (PFN_STATUS_KILL_SHIFT)
    #define PFN_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (PFN_STATUS_FIFONEMPTY_SHIFT)
    #define PFN_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (PFN_STATUS_FIFOFULL_SHIFT)
    #define PFN_STATUS_TC_INT_EN_MASK_SHIFT            (PFN_STATUS_TC_SHIFT)
    #define PFN_STATUS_CMP2_INT_EN_MASK_SHIFT          (PFN_STATUS_CMP2_SHIFT)
    #define PFN_STATUS_CMP1_INT_EN_MASK_SHIFT          (PFN_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define PFN_STATUS_KILL            (uint8)((uint8)0x00u << PFN_STATUS_KILL_SHIFT )
    #define PFN_STATUS_FIFOFULL        (uint8)((uint8)0x01u << PFN_STATUS_FIFOFULL_SHIFT)
    #define PFN_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << PFN_STATUS_FIFONEMPTY_SHIFT)
    #define PFN_STATUS_TC              (uint8)((uint8)0x01u << PFN_STATUS_TC_SHIFT)
    #define PFN_STATUS_CMP2            (uint8)((uint8)0x01u << PFN_STATUS_CMP2_SHIFT)
    #define PFN_STATUS_CMP1            (uint8)((uint8)0x01u << PFN_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define PFN_STATUS_KILL_INT_EN_MASK            (PFN_STATUS_KILL)
    #define PFN_STATUS_FIFOFULL_INT_EN_MASK        (PFN_STATUS_FIFOFULL)
    #define PFN_STATUS_FIFONEMPTY_INT_EN_MASK      (PFN_STATUS_FIFONEMPTY)
    #define PFN_STATUS_TC_INT_EN_MASK              (PFN_STATUS_TC)
    #define PFN_STATUS_CMP2_INT_EN_MASK            (PFN_STATUS_CMP2)
    #define PFN_STATUS_CMP1_INT_EN_MASK            (PFN_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define PFN_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define PFN_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define PFN_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define PFN_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define PFN_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* PFN_UsingFixedFunction */

#endif  /* CY_PWM_PFN_H */


/* [] END OF FILE */
