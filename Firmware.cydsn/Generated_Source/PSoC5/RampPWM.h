/*******************************************************************************
* File Name: RampPWM.h
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

#if !defined(CY_PWM_RampPWM_H)
#define CY_PWM_RampPWM_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 RampPWM_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define RampPWM_Resolution                     (16u)
#define RampPWM_UsingFixedFunction             (0u)
#define RampPWM_DeadBandMode                   (0u)
#define RampPWM_KillModeMinTime                (0u)
#define RampPWM_KillMode                       (0u)
#define RampPWM_PWMMode                        (0u)
#define RampPWM_PWMModeIsCenterAligned         (0u)
#define RampPWM_DeadBandUsed                   (0u)
#define RampPWM_DeadBand2_4                    (0u)

#if !defined(RampPWM_PWMUDB_genblk8_stsreg__REMOVED)
    #define RampPWM_UseStatus                  (0u)
#else
    #define RampPWM_UseStatus                  (0u)
#endif /* !defined(RampPWM_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(RampPWM_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define RampPWM_UseControl                 (1u)
#else
    #define RampPWM_UseControl                 (0u)
#endif /* !defined(RampPWM_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define RampPWM_UseOneCompareMode              (1u)
#define RampPWM_MinimumKillTime                (1u)
#define RampPWM_EnableMode                     (0u)

#define RampPWM_CompareMode1SW                 (0u)
#define RampPWM_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define RampPWM__B_PWM__DISABLED 0
#define RampPWM__B_PWM__ASYNCHRONOUS 1
#define RampPWM__B_PWM__SINGLECYCLE 2
#define RampPWM__B_PWM__LATCHED 3
#define RampPWM__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define RampPWM__B_PWM__DBMDISABLED 0
#define RampPWM__B_PWM__DBM_2_4_CLOCKS 1
#define RampPWM__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define RampPWM__B_PWM__ONE_OUTPUT 0
#define RampPWM__B_PWM__TWO_OUTPUTS 1
#define RampPWM__B_PWM__DUAL_EDGE 2
#define RampPWM__B_PWM__CENTER_ALIGN 3
#define RampPWM__B_PWM__DITHER 5
#define RampPWM__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define RampPWM__B_PWM__LESS_THAN 1
#define RampPWM__B_PWM__LESS_THAN_OR_EQUAL 2
#define RampPWM__B_PWM__GREATER_THAN 3
#define RampPWM__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define RampPWM__B_PWM__EQUAL 0
#define RampPWM__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!RampPWM_UsingFixedFunction)
        uint16 PWMUdb;               /* PWM Current Counter value  */
        #if(!RampPWM_PWMModeIsCenterAligned)
            uint16 PWMPeriod;
        #endif /* (!RampPWM_PWMModeIsCenterAligned) */
        #if (RampPWM_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (RampPWM_UseStatus) */

        /* Backup for Deadband parameters */
        #if(RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_256_CLOCKS || \
            RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(RampPWM_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (RampPWM_KillModeMinTime) */

        /* Backup control register */
        #if(RampPWM_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (RampPWM_UseControl) */

    #endif /* (!RampPWM_UsingFixedFunction) */

}RampPWM_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    RampPWM_Start(void) ;
void    RampPWM_Stop(void) ;

#if (RampPWM_UseStatus || RampPWM_UsingFixedFunction)
    void  RampPWM_SetInterruptMode(uint8 interruptMode) ;
    uint8 RampPWM_ReadStatusRegister(void) ;
#endif /* (RampPWM_UseStatus || RampPWM_UsingFixedFunction) */

#define RampPWM_GetInterruptSource() RampPWM_ReadStatusRegister()

#if (RampPWM_UseControl)
    uint8 RampPWM_ReadControlRegister(void) ;
    void  RampPWM_WriteControlRegister(uint8 control)
          ;
#endif /* (RampPWM_UseControl) */

#if (RampPWM_UseOneCompareMode)
   #if (RampPWM_CompareMode1SW)
       void    RampPWM_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (RampPWM_CompareMode1SW) */
#else
    #if (RampPWM_CompareMode1SW)
        void    RampPWM_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (RampPWM_CompareMode1SW) */
    #if (RampPWM_CompareMode2SW)
        void    RampPWM_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (RampPWM_CompareMode2SW) */
#endif /* (RampPWM_UseOneCompareMode) */

#if (!RampPWM_UsingFixedFunction)
    uint16   RampPWM_ReadCounter(void) ;
    uint16 RampPWM_ReadCapture(void) ;

    #if (RampPWM_UseStatus)
            void RampPWM_ClearFIFO(void) ;
    #endif /* (RampPWM_UseStatus) */

    void    RampPWM_WriteCounter(uint16 counter)
            ;
#endif /* (!RampPWM_UsingFixedFunction) */

void    RampPWM_WritePeriod(uint16 period)
        ;
uint16 RampPWM_ReadPeriod(void) ;

#if (RampPWM_UseOneCompareMode)
    void    RampPWM_WriteCompare(uint16 compare)
            ;
    uint16 RampPWM_ReadCompare(void) ;
#else
    void    RampPWM_WriteCompare1(uint16 compare)
            ;
    uint16 RampPWM_ReadCompare1(void) ;
    void    RampPWM_WriteCompare2(uint16 compare)
            ;
    uint16 RampPWM_ReadCompare2(void) ;
#endif /* (RampPWM_UseOneCompareMode) */


#if (RampPWM_DeadBandUsed)
    void    RampPWM_WriteDeadTime(uint8 deadtime) ;
    uint8   RampPWM_ReadDeadTime(void) ;
#endif /* (RampPWM_DeadBandUsed) */

#if ( RampPWM_KillModeMinTime)
    void RampPWM_WriteKillTime(uint8 killtime) ;
    uint8 RampPWM_ReadKillTime(void) ;
#endif /* ( RampPWM_KillModeMinTime) */

void RampPWM_Init(void) ;
void RampPWM_Enable(void) ;
void RampPWM_Sleep(void) ;
void RampPWM_Wakeup(void) ;
void RampPWM_SaveConfig(void) ;
void RampPWM_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define RampPWM_INIT_PERIOD_VALUE          (50u)
#define RampPWM_INIT_COMPARE_VALUE1        (40u)
#define RampPWM_INIT_COMPARE_VALUE2        (81u)
#define RampPWM_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    RampPWM_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    RampPWM_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    RampPWM_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    RampPWM_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define RampPWM_DEFAULT_COMPARE2_MODE      (uint8)((uint8)1u <<  RampPWM_CTRL_CMPMODE2_SHIFT)
#define RampPWM_DEFAULT_COMPARE1_MODE      (uint8)((uint8)2u <<  RampPWM_CTRL_CMPMODE1_SHIFT)
#define RampPWM_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (RampPWM_UsingFixedFunction)
   #define RampPWM_PERIOD_LSB              (*(reg16 *) RampPWM_PWMHW__PER0)
   #define RampPWM_PERIOD_LSB_PTR          ( (reg16 *) RampPWM_PWMHW__PER0)
   #define RampPWM_COMPARE1_LSB            (*(reg16 *) RampPWM_PWMHW__CNT_CMP0)
   #define RampPWM_COMPARE1_LSB_PTR        ( (reg16 *) RampPWM_PWMHW__CNT_CMP0)
   #define RampPWM_COMPARE2_LSB            (0x00u)
   #define RampPWM_COMPARE2_LSB_PTR        (0x00u)
   #define RampPWM_COUNTER_LSB             (*(reg16 *) RampPWM_PWMHW__CNT_CMP0)
   #define RampPWM_COUNTER_LSB_PTR         ( (reg16 *) RampPWM_PWMHW__CNT_CMP0)
   #define RampPWM_CAPTURE_LSB             (*(reg16 *) RampPWM_PWMHW__CAP0)
   #define RampPWM_CAPTURE_LSB_PTR         ( (reg16 *) RampPWM_PWMHW__CAP0)
   #define RampPWM_RT1                     (*(reg8 *)  RampPWM_PWMHW__RT1)
   #define RampPWM_RT1_PTR                 ( (reg8 *)  RampPWM_PWMHW__RT1)

#else
   #if (RampPWM_Resolution == 8u) /* 8bit - PWM */

       #if(RampPWM_PWMModeIsCenterAligned)
           #define RampPWM_PERIOD_LSB      (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
           #define RampPWM_PERIOD_LSB_PTR  ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #else
           #define RampPWM_PERIOD_LSB      (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
           #define RampPWM_PERIOD_LSB_PTR  ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
       #endif /* (RampPWM_PWMModeIsCenterAligned) */

       #define RampPWM_COMPARE1_LSB        (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
       #define RampPWM_COMPARE1_LSB_PTR    ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
       #define RampPWM_COMPARE2_LSB        (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #define RampPWM_COMPARE2_LSB_PTR    ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
       #define RampPWM_COUNTERCAP_LSB      (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
       #define RampPWM_COUNTERCAP_LSB_PTR  ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
       #define RampPWM_COUNTER_LSB         (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
       #define RampPWM_COUNTER_LSB_PTR     ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
       #define RampPWM_CAPTURE_LSB         (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__F1_REG)
       #define RampPWM_CAPTURE_LSB_PTR     ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(RampPWM_PWMModeIsCenterAligned)
               #define RampPWM_PERIOD_LSB      (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
               #define RampPWM_PERIOD_LSB_PTR  ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #else
               #define RampPWM_PERIOD_LSB      (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
               #define RampPWM_PERIOD_LSB_PTR  ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__F0_REG)
            #endif /* (RampPWM_PWMModeIsCenterAligned) */

            #define RampPWM_COMPARE1_LSB       (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
            #define RampPWM_COMPARE1_LSB_PTR   ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__D0_REG)
            #define RampPWM_COMPARE2_LSB       (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #define RampPWM_COMPARE2_LSB_PTR   ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__D1_REG)
            #define RampPWM_COUNTERCAP_LSB     (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
            #define RampPWM_COUNTERCAP_LSB_PTR ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
            #define RampPWM_COUNTER_LSB        (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
            #define RampPWM_COUNTER_LSB_PTR    ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__A0_REG)
            #define RampPWM_CAPTURE_LSB        (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__F1_REG)
            #define RampPWM_CAPTURE_LSB_PTR    ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__F1_REG)
        #else
            #if(RampPWM_PWMModeIsCenterAligned)
               #define RampPWM_PERIOD_LSB      (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
               #define RampPWM_PERIOD_LSB_PTR  ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #else
               #define RampPWM_PERIOD_LSB      (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_F0_REG)
               #define RampPWM_PERIOD_LSB_PTR  ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_F0_REG)
            #endif /* (RampPWM_PWMModeIsCenterAligned) */

            #define RampPWM_COMPARE1_LSB       (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_D0_REG)
            #define RampPWM_COMPARE1_LSB_PTR   ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_D0_REG)
            #define RampPWM_COMPARE2_LSB       (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #define RampPWM_COMPARE2_LSB_PTR   ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_D1_REG)
            #define RampPWM_COUNTERCAP_LSB     (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_A1_REG)
            #define RampPWM_COUNTERCAP_LSB_PTR ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_A1_REG)
            #define RampPWM_COUNTER_LSB        (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_A0_REG)
            #define RampPWM_COUNTER_LSB_PTR    ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_A0_REG)
            #define RampPWM_CAPTURE_LSB        (*(reg16 *) RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_F1_REG)
            #define RampPWM_CAPTURE_LSB_PTR    ((reg16 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define RampPWM_AUX_CONTROLDP1          (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u1__DP_AUX_CTL_REG)
       #define RampPWM_AUX_CONTROLDP1_PTR      ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (RampPWM_Resolution == 8) */

   #define RampPWM_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__A1_REG)
   #define RampPWM_AUX_CONTROLDP0          (*(reg8 *)  RampPWM_PWMUDB_sP16_pwmdp_u0__DP_AUX_CTL_REG)
   #define RampPWM_AUX_CONTROLDP0_PTR      ((reg8 *)   RampPWM_PWMUDB_sP16_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (RampPWM_UsingFixedFunction) */

#if(RampPWM_KillModeMinTime )
    #define RampPWM_KILLMODEMINTIME        (*(reg8 *)  RampPWM_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define RampPWM_KILLMODEMINTIME_PTR    ((reg8 *)   RampPWM_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (RampPWM_KillModeMinTime ) */

#if(RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_256_CLOCKS)
    #define RampPWM_DEADBAND_COUNT         (*(reg8 *)  RampPWM_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define RampPWM_DEADBAND_COUNT_PTR     ((reg8 *)   RampPWM_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define RampPWM_DEADBAND_LSB_PTR       ((reg8 *)   RampPWM_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define RampPWM_DEADBAND_LSB           (*(reg8 *)  RampPWM_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (RampPWM_UsingFixedFunction)
        #define RampPWM_DEADBAND_COUNT         (*(reg8 *)  RampPWM_PWMHW__CFG0)
        #define RampPWM_DEADBAND_COUNT_PTR     ((reg8 *)   RampPWM_PWMHW__CFG0)
        #define RampPWM_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << RampPWM_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define RampPWM_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define RampPWM_DEADBAND_COUNT         (*(reg8 *)  RampPWM_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define RampPWM_DEADBAND_COUNT_PTR     ((reg8 *)   RampPWM_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define RampPWM_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << RampPWM_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define RampPWM_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (RampPWM_UsingFixedFunction) */
#endif /* (RampPWM_DeadBandMode == RampPWM__B_PWM__DBM_256_CLOCKS) */



#if (RampPWM_UsingFixedFunction)
    #define RampPWM_STATUS                 (*(reg8 *) RampPWM_PWMHW__SR0)
    #define RampPWM_STATUS_PTR             ((reg8 *) RampPWM_PWMHW__SR0)
    #define RampPWM_STATUS_MASK            (*(reg8 *) RampPWM_PWMHW__SR0)
    #define RampPWM_STATUS_MASK_PTR        ((reg8 *) RampPWM_PWMHW__SR0)
    #define RampPWM_CONTROL                (*(reg8 *) RampPWM_PWMHW__CFG0)
    #define RampPWM_CONTROL_PTR            ((reg8 *) RampPWM_PWMHW__CFG0)
    #define RampPWM_CONTROL2               (*(reg8 *) RampPWM_PWMHW__CFG1)
    #define RampPWM_CONTROL3               (*(reg8 *) RampPWM_PWMHW__CFG2)
    #define RampPWM_GLOBAL_ENABLE          (*(reg8 *) RampPWM_PWMHW__PM_ACT_CFG)
    #define RampPWM_GLOBAL_ENABLE_PTR      ( (reg8 *) RampPWM_PWMHW__PM_ACT_CFG)
    #define RampPWM_GLOBAL_STBY_ENABLE     (*(reg8 *) RampPWM_PWMHW__PM_STBY_CFG)
    #define RampPWM_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) RampPWM_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define RampPWM_BLOCK_EN_MASK          (RampPWM_PWMHW__PM_ACT_MSK)
    #define RampPWM_BLOCK_STBY_EN_MASK     (RampPWM_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define RampPWM_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define RampPWM_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define RampPWM_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define RampPWM_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define RampPWM_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define RampPWM_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define RampPWM_CTRL_ENABLE            (uint8)((uint8)0x01u << RampPWM_CTRL_ENABLE_SHIFT)
    #define RampPWM_CTRL_RESET             (uint8)((uint8)0x01u << RampPWM_CTRL_RESET_SHIFT)
    #define RampPWM_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << RampPWM_CTRL_CMPMODE2_SHIFT)
    #define RampPWM_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << RampPWM_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define RampPWM_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define RampPWM_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << RampPWM_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define RampPWM_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define RampPWM_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define RampPWM_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define RampPWM_STATUS_TC_INT_EN_MASK_SHIFT            (RampPWM_STATUS_TC_SHIFT - 4u)
    #define RampPWM_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define RampPWM_STATUS_CMP1_INT_EN_MASK_SHIFT          (RampPWM_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define RampPWM_STATUS_TC              (uint8)((uint8)0x01u << RampPWM_STATUS_TC_SHIFT)
    #define RampPWM_STATUS_CMP1            (uint8)((uint8)0x01u << RampPWM_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define RampPWM_STATUS_TC_INT_EN_MASK              (uint8)((uint8)RampPWM_STATUS_TC >> 4u)
    #define RampPWM_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)RampPWM_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define RampPWM_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define RampPWM_RT1_MASK              (uint8)((uint8)0x03u << RampPWM_RT1_SHIFT)
    #define RampPWM_SYNC                  (uint8)((uint8)0x03u << RampPWM_RT1_SHIFT)
    #define RampPWM_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define RampPWM_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << RampPWM_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define RampPWM_SYNCDSI_EN            (uint8)((uint8)0x0Fu << RampPWM_SYNCDSI_SHIFT)


#else
    #define RampPWM_STATUS                (*(reg8 *)   RampPWM_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define RampPWM_STATUS_PTR            ((reg8 *)    RampPWM_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define RampPWM_STATUS_MASK           (*(reg8 *)   RampPWM_PWMUDB_genblk8_stsreg__MASK_REG)
    #define RampPWM_STATUS_MASK_PTR       ((reg8 *)    RampPWM_PWMUDB_genblk8_stsreg__MASK_REG)
    #define RampPWM_STATUS_AUX_CTRL       (*(reg8 *)   RampPWM_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define RampPWM_CONTROL               (*(reg8 *)   RampPWM_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define RampPWM_CONTROL_PTR           ((reg8 *)    RampPWM_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define RampPWM_CTRL_ENABLE_SHIFT      (0x07u)
    #define RampPWM_CTRL_RESET_SHIFT       (0x06u)
    #define RampPWM_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define RampPWM_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define RampPWM_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define RampPWM_CTRL_ENABLE            (uint8)((uint8)0x01u << RampPWM_CTRL_ENABLE_SHIFT)
    #define RampPWM_CTRL_RESET             (uint8)((uint8)0x01u << RampPWM_CTRL_RESET_SHIFT)
    #define RampPWM_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << RampPWM_CTRL_CMPMODE2_SHIFT)
    #define RampPWM_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << RampPWM_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define RampPWM_STATUS_KILL_SHIFT          (0x05u)
    #define RampPWM_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define RampPWM_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define RampPWM_STATUS_TC_SHIFT            (0x02u)
    #define RampPWM_STATUS_CMP2_SHIFT          (0x01u)
    #define RampPWM_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define RampPWM_STATUS_KILL_INT_EN_MASK_SHIFT          (RampPWM_STATUS_KILL_SHIFT)
    #define RampPWM_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (RampPWM_STATUS_FIFONEMPTY_SHIFT)
    #define RampPWM_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (RampPWM_STATUS_FIFOFULL_SHIFT)
    #define RampPWM_STATUS_TC_INT_EN_MASK_SHIFT            (RampPWM_STATUS_TC_SHIFT)
    #define RampPWM_STATUS_CMP2_INT_EN_MASK_SHIFT          (RampPWM_STATUS_CMP2_SHIFT)
    #define RampPWM_STATUS_CMP1_INT_EN_MASK_SHIFT          (RampPWM_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define RampPWM_STATUS_KILL            (uint8)((uint8)0x00u << RampPWM_STATUS_KILL_SHIFT )
    #define RampPWM_STATUS_FIFOFULL        (uint8)((uint8)0x01u << RampPWM_STATUS_FIFOFULL_SHIFT)
    #define RampPWM_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << RampPWM_STATUS_FIFONEMPTY_SHIFT)
    #define RampPWM_STATUS_TC              (uint8)((uint8)0x01u << RampPWM_STATUS_TC_SHIFT)
    #define RampPWM_STATUS_CMP2            (uint8)((uint8)0x01u << RampPWM_STATUS_CMP2_SHIFT)
    #define RampPWM_STATUS_CMP1            (uint8)((uint8)0x01u << RampPWM_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define RampPWM_STATUS_KILL_INT_EN_MASK            (RampPWM_STATUS_KILL)
    #define RampPWM_STATUS_FIFOFULL_INT_EN_MASK        (RampPWM_STATUS_FIFOFULL)
    #define RampPWM_STATUS_FIFONEMPTY_INT_EN_MASK      (RampPWM_STATUS_FIFONEMPTY)
    #define RampPWM_STATUS_TC_INT_EN_MASK              (RampPWM_STATUS_TC)
    #define RampPWM_STATUS_CMP2_INT_EN_MASK            (RampPWM_STATUS_CMP2)
    #define RampPWM_STATUS_CMP1_INT_EN_MASK            (RampPWM_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define RampPWM_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define RampPWM_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define RampPWM_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define RampPWM_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define RampPWM_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* RampPWM_UsingFixedFunction */

#endif  /* CY_PWM_RampPWM_H */


/* [] END OF FILE */
