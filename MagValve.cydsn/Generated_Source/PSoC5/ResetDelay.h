/*******************************************************************************
* File Name: ResetDelay.h
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

#if !defined(CY_PWM_ResetDelay_H)
#define CY_PWM_ResetDelay_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 ResetDelay_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define ResetDelay_Resolution                     (8u)
#define ResetDelay_UsingFixedFunction             (0u)
#define ResetDelay_DeadBandMode                   (0u)
#define ResetDelay_KillModeMinTime                (0u)
#define ResetDelay_KillMode                       (0u)
#define ResetDelay_PWMMode                        (0u)
#define ResetDelay_PWMModeIsCenterAligned         (0u)
#define ResetDelay_DeadBandUsed                   (0u)
#define ResetDelay_DeadBand2_4                    (0u)

#if !defined(ResetDelay_PWMUDB_genblk8_stsreg__REMOVED)
    #define ResetDelay_UseStatus                  (0u)
#else
    #define ResetDelay_UseStatus                  (0u)
#endif /* !defined(ResetDelay_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(ResetDelay_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define ResetDelay_UseControl                 (1u)
#else
    #define ResetDelay_UseControl                 (0u)
#endif /* !defined(ResetDelay_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define ResetDelay_UseOneCompareMode              (1u)
#define ResetDelay_MinimumKillTime                (1u)
#define ResetDelay_EnableMode                     (0u)

#define ResetDelay_CompareMode1SW                 (0u)
#define ResetDelay_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define ResetDelay__B_PWM__DISABLED 0
#define ResetDelay__B_PWM__ASYNCHRONOUS 1
#define ResetDelay__B_PWM__SINGLECYCLE 2
#define ResetDelay__B_PWM__LATCHED 3
#define ResetDelay__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define ResetDelay__B_PWM__DBMDISABLED 0
#define ResetDelay__B_PWM__DBM_2_4_CLOCKS 1
#define ResetDelay__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define ResetDelay__B_PWM__ONE_OUTPUT 0
#define ResetDelay__B_PWM__TWO_OUTPUTS 1
#define ResetDelay__B_PWM__DUAL_EDGE 2
#define ResetDelay__B_PWM__CENTER_ALIGN 3
#define ResetDelay__B_PWM__DITHER 5
#define ResetDelay__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define ResetDelay__B_PWM__LESS_THAN 1
#define ResetDelay__B_PWM__LESS_THAN_OR_EQUAL 2
#define ResetDelay__B_PWM__GREATER_THAN 3
#define ResetDelay__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define ResetDelay__B_PWM__EQUAL 0
#define ResetDelay__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!ResetDelay_UsingFixedFunction)
        uint8 PWMUdb;               /* PWM Current Counter value  */
        #if(!ResetDelay_PWMModeIsCenterAligned)
            uint8 PWMPeriod;
        #endif /* (!ResetDelay_PWMModeIsCenterAligned) */
        #if (ResetDelay_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (ResetDelay_UseStatus) */

        /* Backup for Deadband parameters */
        #if(ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_256_CLOCKS || \
            ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(ResetDelay_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (ResetDelay_KillModeMinTime) */

        /* Backup control register */
        #if(ResetDelay_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (ResetDelay_UseControl) */

    #endif /* (!ResetDelay_UsingFixedFunction) */

}ResetDelay_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    ResetDelay_Start(void) ;
void    ResetDelay_Stop(void) ;

#if (ResetDelay_UseStatus || ResetDelay_UsingFixedFunction)
    void  ResetDelay_SetInterruptMode(uint8 interruptMode) ;
    uint8 ResetDelay_ReadStatusRegister(void) ;
#endif /* (ResetDelay_UseStatus || ResetDelay_UsingFixedFunction) */

#define ResetDelay_GetInterruptSource() ResetDelay_ReadStatusRegister()

#if (ResetDelay_UseControl)
    uint8 ResetDelay_ReadControlRegister(void) ;
    void  ResetDelay_WriteControlRegister(uint8 control)
          ;
#endif /* (ResetDelay_UseControl) */

#if (ResetDelay_UseOneCompareMode)
   #if (ResetDelay_CompareMode1SW)
       void    ResetDelay_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (ResetDelay_CompareMode1SW) */
#else
    #if (ResetDelay_CompareMode1SW)
        void    ResetDelay_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (ResetDelay_CompareMode1SW) */
    #if (ResetDelay_CompareMode2SW)
        void    ResetDelay_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (ResetDelay_CompareMode2SW) */
#endif /* (ResetDelay_UseOneCompareMode) */

#if (!ResetDelay_UsingFixedFunction)
    uint8   ResetDelay_ReadCounter(void) ;
    uint8 ResetDelay_ReadCapture(void) ;

    #if (ResetDelay_UseStatus)
            void ResetDelay_ClearFIFO(void) ;
    #endif /* (ResetDelay_UseStatus) */

    void    ResetDelay_WriteCounter(uint8 counter)
            ;
#endif /* (!ResetDelay_UsingFixedFunction) */

void    ResetDelay_WritePeriod(uint8 period)
        ;
uint8 ResetDelay_ReadPeriod(void) ;

#if (ResetDelay_UseOneCompareMode)
    void    ResetDelay_WriteCompare(uint8 compare)
            ;
    uint8 ResetDelay_ReadCompare(void) ;
#else
    void    ResetDelay_WriteCompare1(uint8 compare)
            ;
    uint8 ResetDelay_ReadCompare1(void) ;
    void    ResetDelay_WriteCompare2(uint8 compare)
            ;
    uint8 ResetDelay_ReadCompare2(void) ;
#endif /* (ResetDelay_UseOneCompareMode) */


#if (ResetDelay_DeadBandUsed)
    void    ResetDelay_WriteDeadTime(uint8 deadtime) ;
    uint8   ResetDelay_ReadDeadTime(void) ;
#endif /* (ResetDelay_DeadBandUsed) */

#if ( ResetDelay_KillModeMinTime)
    void ResetDelay_WriteKillTime(uint8 killtime) ;
    uint8 ResetDelay_ReadKillTime(void) ;
#endif /* ( ResetDelay_KillModeMinTime) */

void ResetDelay_Init(void) ;
void ResetDelay_Enable(void) ;
void ResetDelay_Sleep(void) ;
void ResetDelay_Wakeup(void) ;
void ResetDelay_SaveConfig(void) ;
void ResetDelay_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define ResetDelay_INIT_PERIOD_VALUE          (50u)
#define ResetDelay_INIT_COMPARE_VALUE1        (1u)
#define ResetDelay_INIT_COMPARE_VALUE2        (128u)
#define ResetDelay_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    ResetDelay_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    ResetDelay_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    ResetDelay_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    ResetDelay_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define ResetDelay_DEFAULT_COMPARE2_MODE      (uint8)((uint8)2u <<  ResetDelay_CTRL_CMPMODE2_SHIFT)
#define ResetDelay_DEFAULT_COMPARE1_MODE      (uint8)((uint8)1u <<  ResetDelay_CTRL_CMPMODE1_SHIFT)
#define ResetDelay_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (ResetDelay_UsingFixedFunction)
   #define ResetDelay_PERIOD_LSB              (*(reg16 *) ResetDelay_PWMHW__PER0)
   #define ResetDelay_PERIOD_LSB_PTR          ( (reg16 *) ResetDelay_PWMHW__PER0)
   #define ResetDelay_COMPARE1_LSB            (*(reg16 *) ResetDelay_PWMHW__CNT_CMP0)
   #define ResetDelay_COMPARE1_LSB_PTR        ( (reg16 *) ResetDelay_PWMHW__CNT_CMP0)
   #define ResetDelay_COMPARE2_LSB            (0x00u)
   #define ResetDelay_COMPARE2_LSB_PTR        (0x00u)
   #define ResetDelay_COUNTER_LSB             (*(reg16 *) ResetDelay_PWMHW__CNT_CMP0)
   #define ResetDelay_COUNTER_LSB_PTR         ( (reg16 *) ResetDelay_PWMHW__CNT_CMP0)
   #define ResetDelay_CAPTURE_LSB             (*(reg16 *) ResetDelay_PWMHW__CAP0)
   #define ResetDelay_CAPTURE_LSB_PTR         ( (reg16 *) ResetDelay_PWMHW__CAP0)
   #define ResetDelay_RT1                     (*(reg8 *)  ResetDelay_PWMHW__RT1)
   #define ResetDelay_RT1_PTR                 ( (reg8 *)  ResetDelay_PWMHW__RT1)

#else
   #if (ResetDelay_Resolution == 8u) /* 8bit - PWM */

       #if(ResetDelay_PWMModeIsCenterAligned)
           #define ResetDelay_PERIOD_LSB      (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
           #define ResetDelay_PERIOD_LSB_PTR  ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #else
           #define ResetDelay_PERIOD_LSB      (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
           #define ResetDelay_PERIOD_LSB_PTR  ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
       #endif /* (ResetDelay_PWMModeIsCenterAligned) */

       #define ResetDelay_COMPARE1_LSB        (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define ResetDelay_COMPARE1_LSB_PTR    ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define ResetDelay_COMPARE2_LSB        (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define ResetDelay_COMPARE2_LSB_PTR    ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define ResetDelay_COUNTERCAP_LSB      (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define ResetDelay_COUNTERCAP_LSB_PTR  ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define ResetDelay_COUNTER_LSB         (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define ResetDelay_COUNTER_LSB_PTR     ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define ResetDelay_CAPTURE_LSB         (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
       #define ResetDelay_CAPTURE_LSB_PTR     ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(ResetDelay_PWMModeIsCenterAligned)
               #define ResetDelay_PERIOD_LSB      (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
               #define ResetDelay_PERIOD_LSB_PTR  ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #else
               #define ResetDelay_PERIOD_LSB      (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
               #define ResetDelay_PERIOD_LSB_PTR  ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
            #endif /* (ResetDelay_PWMModeIsCenterAligned) */

            #define ResetDelay_COMPARE1_LSB       (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define ResetDelay_COMPARE1_LSB_PTR   ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define ResetDelay_COMPARE2_LSB       (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define ResetDelay_COMPARE2_LSB_PTR   ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define ResetDelay_COUNTERCAP_LSB     (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define ResetDelay_COUNTERCAP_LSB_PTR ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define ResetDelay_COUNTER_LSB        (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define ResetDelay_COUNTER_LSB_PTR    ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define ResetDelay_CAPTURE_LSB        (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
            #define ResetDelay_CAPTURE_LSB_PTR    ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
        #else
            #if(ResetDelay_PWMModeIsCenterAligned)
               #define ResetDelay_PERIOD_LSB      (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
               #define ResetDelay_PERIOD_LSB_PTR  ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #else
               #define ResetDelay_PERIOD_LSB      (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
               #define ResetDelay_PERIOD_LSB_PTR  ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
            #endif /* (ResetDelay_PWMModeIsCenterAligned) */

            #define ResetDelay_COMPARE1_LSB       (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define ResetDelay_COMPARE1_LSB_PTR   ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define ResetDelay_COMPARE2_LSB       (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define ResetDelay_COMPARE2_LSB_PTR   ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define ResetDelay_COUNTERCAP_LSB     (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define ResetDelay_COUNTERCAP_LSB_PTR ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define ResetDelay_COUNTER_LSB        (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define ResetDelay_COUNTER_LSB_PTR    ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define ResetDelay_CAPTURE_LSB        (*(reg16 *) ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
            #define ResetDelay_CAPTURE_LSB_PTR    ((reg16 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define ResetDelay_AUX_CONTROLDP1          (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)
       #define ResetDelay_AUX_CONTROLDP1_PTR      ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (ResetDelay_Resolution == 8) */

   #define ResetDelay_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
   #define ResetDelay_AUX_CONTROLDP0          (*(reg8 *)  ResetDelay_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)
   #define ResetDelay_AUX_CONTROLDP0_PTR      ((reg8 *)   ResetDelay_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (ResetDelay_UsingFixedFunction) */

#if(ResetDelay_KillModeMinTime )
    #define ResetDelay_KILLMODEMINTIME        (*(reg8 *)  ResetDelay_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define ResetDelay_KILLMODEMINTIME_PTR    ((reg8 *)   ResetDelay_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (ResetDelay_KillModeMinTime ) */

#if(ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_256_CLOCKS)
    #define ResetDelay_DEADBAND_COUNT         (*(reg8 *)  ResetDelay_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define ResetDelay_DEADBAND_COUNT_PTR     ((reg8 *)   ResetDelay_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define ResetDelay_DEADBAND_LSB_PTR       ((reg8 *)   ResetDelay_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define ResetDelay_DEADBAND_LSB           (*(reg8 *)  ResetDelay_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (ResetDelay_UsingFixedFunction)
        #define ResetDelay_DEADBAND_COUNT         (*(reg8 *)  ResetDelay_PWMHW__CFG0)
        #define ResetDelay_DEADBAND_COUNT_PTR     ((reg8 *)   ResetDelay_PWMHW__CFG0)
        #define ResetDelay_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << ResetDelay_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define ResetDelay_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define ResetDelay_DEADBAND_COUNT         (*(reg8 *)  ResetDelay_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define ResetDelay_DEADBAND_COUNT_PTR     ((reg8 *)   ResetDelay_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define ResetDelay_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << ResetDelay_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define ResetDelay_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (ResetDelay_UsingFixedFunction) */
#endif /* (ResetDelay_DeadBandMode == ResetDelay__B_PWM__DBM_256_CLOCKS) */



#if (ResetDelay_UsingFixedFunction)
    #define ResetDelay_STATUS                 (*(reg8 *) ResetDelay_PWMHW__SR0)
    #define ResetDelay_STATUS_PTR             ((reg8 *) ResetDelay_PWMHW__SR0)
    #define ResetDelay_STATUS_MASK            (*(reg8 *) ResetDelay_PWMHW__SR0)
    #define ResetDelay_STATUS_MASK_PTR        ((reg8 *) ResetDelay_PWMHW__SR0)
    #define ResetDelay_CONTROL                (*(reg8 *) ResetDelay_PWMHW__CFG0)
    #define ResetDelay_CONTROL_PTR            ((reg8 *) ResetDelay_PWMHW__CFG0)
    #define ResetDelay_CONTROL2               (*(reg8 *) ResetDelay_PWMHW__CFG1)
    #define ResetDelay_CONTROL3               (*(reg8 *) ResetDelay_PWMHW__CFG2)
    #define ResetDelay_GLOBAL_ENABLE          (*(reg8 *) ResetDelay_PWMHW__PM_ACT_CFG)
    #define ResetDelay_GLOBAL_ENABLE_PTR      ( (reg8 *) ResetDelay_PWMHW__PM_ACT_CFG)
    #define ResetDelay_GLOBAL_STBY_ENABLE     (*(reg8 *) ResetDelay_PWMHW__PM_STBY_CFG)
    #define ResetDelay_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) ResetDelay_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define ResetDelay_BLOCK_EN_MASK          (ResetDelay_PWMHW__PM_ACT_MSK)
    #define ResetDelay_BLOCK_STBY_EN_MASK     (ResetDelay_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define ResetDelay_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define ResetDelay_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define ResetDelay_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define ResetDelay_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define ResetDelay_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define ResetDelay_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define ResetDelay_CTRL_ENABLE            (uint8)((uint8)0x01u << ResetDelay_CTRL_ENABLE_SHIFT)
    #define ResetDelay_CTRL_RESET             (uint8)((uint8)0x01u << ResetDelay_CTRL_RESET_SHIFT)
    #define ResetDelay_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << ResetDelay_CTRL_CMPMODE2_SHIFT)
    #define ResetDelay_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << ResetDelay_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define ResetDelay_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define ResetDelay_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << ResetDelay_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define ResetDelay_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define ResetDelay_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define ResetDelay_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define ResetDelay_STATUS_TC_INT_EN_MASK_SHIFT            (ResetDelay_STATUS_TC_SHIFT - 4u)
    #define ResetDelay_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define ResetDelay_STATUS_CMP1_INT_EN_MASK_SHIFT          (ResetDelay_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define ResetDelay_STATUS_TC              (uint8)((uint8)0x01u << ResetDelay_STATUS_TC_SHIFT)
    #define ResetDelay_STATUS_CMP1            (uint8)((uint8)0x01u << ResetDelay_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define ResetDelay_STATUS_TC_INT_EN_MASK              (uint8)((uint8)ResetDelay_STATUS_TC >> 4u)
    #define ResetDelay_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)ResetDelay_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define ResetDelay_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define ResetDelay_RT1_MASK              (uint8)((uint8)0x03u << ResetDelay_RT1_SHIFT)
    #define ResetDelay_SYNC                  (uint8)((uint8)0x03u << ResetDelay_RT1_SHIFT)
    #define ResetDelay_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define ResetDelay_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << ResetDelay_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define ResetDelay_SYNCDSI_EN            (uint8)((uint8)0x0Fu << ResetDelay_SYNCDSI_SHIFT)


#else
    #define ResetDelay_STATUS                (*(reg8 *)   ResetDelay_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define ResetDelay_STATUS_PTR            ((reg8 *)    ResetDelay_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define ResetDelay_STATUS_MASK           (*(reg8 *)   ResetDelay_PWMUDB_genblk8_stsreg__MASK_REG)
    #define ResetDelay_STATUS_MASK_PTR       ((reg8 *)    ResetDelay_PWMUDB_genblk8_stsreg__MASK_REG)
    #define ResetDelay_STATUS_AUX_CTRL       (*(reg8 *)   ResetDelay_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define ResetDelay_CONTROL               (*(reg8 *)   ResetDelay_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define ResetDelay_CONTROL_PTR           ((reg8 *)    ResetDelay_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define ResetDelay_CTRL_ENABLE_SHIFT      (0x07u)
    #define ResetDelay_CTRL_RESET_SHIFT       (0x06u)
    #define ResetDelay_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define ResetDelay_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define ResetDelay_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define ResetDelay_CTRL_ENABLE            (uint8)((uint8)0x01u << ResetDelay_CTRL_ENABLE_SHIFT)
    #define ResetDelay_CTRL_RESET             (uint8)((uint8)0x01u << ResetDelay_CTRL_RESET_SHIFT)
    #define ResetDelay_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << ResetDelay_CTRL_CMPMODE2_SHIFT)
    #define ResetDelay_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << ResetDelay_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define ResetDelay_STATUS_KILL_SHIFT          (0x05u)
    #define ResetDelay_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define ResetDelay_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define ResetDelay_STATUS_TC_SHIFT            (0x02u)
    #define ResetDelay_STATUS_CMP2_SHIFT          (0x01u)
    #define ResetDelay_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define ResetDelay_STATUS_KILL_INT_EN_MASK_SHIFT          (ResetDelay_STATUS_KILL_SHIFT)
    #define ResetDelay_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (ResetDelay_STATUS_FIFONEMPTY_SHIFT)
    #define ResetDelay_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (ResetDelay_STATUS_FIFOFULL_SHIFT)
    #define ResetDelay_STATUS_TC_INT_EN_MASK_SHIFT            (ResetDelay_STATUS_TC_SHIFT)
    #define ResetDelay_STATUS_CMP2_INT_EN_MASK_SHIFT          (ResetDelay_STATUS_CMP2_SHIFT)
    #define ResetDelay_STATUS_CMP1_INT_EN_MASK_SHIFT          (ResetDelay_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define ResetDelay_STATUS_KILL            (uint8)((uint8)0x00u << ResetDelay_STATUS_KILL_SHIFT )
    #define ResetDelay_STATUS_FIFOFULL        (uint8)((uint8)0x01u << ResetDelay_STATUS_FIFOFULL_SHIFT)
    #define ResetDelay_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << ResetDelay_STATUS_FIFONEMPTY_SHIFT)
    #define ResetDelay_STATUS_TC              (uint8)((uint8)0x01u << ResetDelay_STATUS_TC_SHIFT)
    #define ResetDelay_STATUS_CMP2            (uint8)((uint8)0x01u << ResetDelay_STATUS_CMP2_SHIFT)
    #define ResetDelay_STATUS_CMP1            (uint8)((uint8)0x01u << ResetDelay_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define ResetDelay_STATUS_KILL_INT_EN_MASK            (ResetDelay_STATUS_KILL)
    #define ResetDelay_STATUS_FIFOFULL_INT_EN_MASK        (ResetDelay_STATUS_FIFOFULL)
    #define ResetDelay_STATUS_FIFONEMPTY_INT_EN_MASK      (ResetDelay_STATUS_FIFONEMPTY)
    #define ResetDelay_STATUS_TC_INT_EN_MASK              (ResetDelay_STATUS_TC)
    #define ResetDelay_STATUS_CMP2_INT_EN_MASK            (ResetDelay_STATUS_CMP2)
    #define ResetDelay_STATUS_CMP1_INT_EN_MASK            (ResetDelay_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define ResetDelay_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define ResetDelay_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define ResetDelay_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define ResetDelay_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define ResetDelay_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* ResetDelay_UsingFixedFunction */

#endif  /* CY_PWM_ResetDelay_H */


/* [] END OF FILE */
