/*******************************************************************************
* File Name: DischargeDelay.h
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

#if !defined(CY_PWM_DischargeDelay_H)
#define CY_PWM_DischargeDelay_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 DischargeDelay_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define DischargeDelay_Resolution                     (8u)
#define DischargeDelay_UsingFixedFunction             (0u)
#define DischargeDelay_DeadBandMode                   (0u)
#define DischargeDelay_KillModeMinTime                (0u)
#define DischargeDelay_KillMode                       (0u)
#define DischargeDelay_PWMMode                        (0u)
#define DischargeDelay_PWMModeIsCenterAligned         (0u)
#define DischargeDelay_DeadBandUsed                   (0u)
#define DischargeDelay_DeadBand2_4                    (0u)

#if !defined(DischargeDelay_PWMUDB_genblk8_stsreg__REMOVED)
    #define DischargeDelay_UseStatus                  (0u)
#else
    #define DischargeDelay_UseStatus                  (0u)
#endif /* !defined(DischargeDelay_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(DischargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define DischargeDelay_UseControl                 (0u)
#else
    #define DischargeDelay_UseControl                 (0u)
#endif /* !defined(DischargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define DischargeDelay_UseOneCompareMode              (1u)
#define DischargeDelay_MinimumKillTime                (1u)
#define DischargeDelay_EnableMode                     (1u)

#define DischargeDelay_CompareMode1SW                 (0u)
#define DischargeDelay_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define DischargeDelay__B_PWM__DISABLED 0
#define DischargeDelay__B_PWM__ASYNCHRONOUS 1
#define DischargeDelay__B_PWM__SINGLECYCLE 2
#define DischargeDelay__B_PWM__LATCHED 3
#define DischargeDelay__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define DischargeDelay__B_PWM__DBMDISABLED 0
#define DischargeDelay__B_PWM__DBM_2_4_CLOCKS 1
#define DischargeDelay__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define DischargeDelay__B_PWM__ONE_OUTPUT 0
#define DischargeDelay__B_PWM__TWO_OUTPUTS 1
#define DischargeDelay__B_PWM__DUAL_EDGE 2
#define DischargeDelay__B_PWM__CENTER_ALIGN 3
#define DischargeDelay__B_PWM__DITHER 5
#define DischargeDelay__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define DischargeDelay__B_PWM__LESS_THAN 1
#define DischargeDelay__B_PWM__LESS_THAN_OR_EQUAL 2
#define DischargeDelay__B_PWM__GREATER_THAN 3
#define DischargeDelay__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define DischargeDelay__B_PWM__EQUAL 0
#define DischargeDelay__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!DischargeDelay_UsingFixedFunction)
        uint8 PWMUdb;               /* PWM Current Counter value  */
        #if(!DischargeDelay_PWMModeIsCenterAligned)
            uint8 PWMPeriod;
        #endif /* (!DischargeDelay_PWMModeIsCenterAligned) */
        #if (DischargeDelay_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (DischargeDelay_UseStatus) */

        /* Backup for Deadband parameters */
        #if(DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_256_CLOCKS || \
            DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(DischargeDelay_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (DischargeDelay_KillModeMinTime) */

        /* Backup control register */
        #if(DischargeDelay_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (DischargeDelay_UseControl) */

    #endif /* (!DischargeDelay_UsingFixedFunction) */

}DischargeDelay_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    DischargeDelay_Start(void) ;
void    DischargeDelay_Stop(void) ;

#if (DischargeDelay_UseStatus || DischargeDelay_UsingFixedFunction)
    void  DischargeDelay_SetInterruptMode(uint8 interruptMode) ;
    uint8 DischargeDelay_ReadStatusRegister(void) ;
#endif /* (DischargeDelay_UseStatus || DischargeDelay_UsingFixedFunction) */

#define DischargeDelay_GetInterruptSource() DischargeDelay_ReadStatusRegister()

#if (DischargeDelay_UseControl)
    uint8 DischargeDelay_ReadControlRegister(void) ;
    void  DischargeDelay_WriteControlRegister(uint8 control)
          ;
#endif /* (DischargeDelay_UseControl) */

#if (DischargeDelay_UseOneCompareMode)
   #if (DischargeDelay_CompareMode1SW)
       void    DischargeDelay_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (DischargeDelay_CompareMode1SW) */
#else
    #if (DischargeDelay_CompareMode1SW)
        void    DischargeDelay_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (DischargeDelay_CompareMode1SW) */
    #if (DischargeDelay_CompareMode2SW)
        void    DischargeDelay_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (DischargeDelay_CompareMode2SW) */
#endif /* (DischargeDelay_UseOneCompareMode) */

#if (!DischargeDelay_UsingFixedFunction)
    uint8   DischargeDelay_ReadCounter(void) ;
    uint8 DischargeDelay_ReadCapture(void) ;

    #if (DischargeDelay_UseStatus)
            void DischargeDelay_ClearFIFO(void) ;
    #endif /* (DischargeDelay_UseStatus) */

    void    DischargeDelay_WriteCounter(uint8 counter)
            ;
#endif /* (!DischargeDelay_UsingFixedFunction) */

void    DischargeDelay_WritePeriod(uint8 period)
        ;
uint8 DischargeDelay_ReadPeriod(void) ;

#if (DischargeDelay_UseOneCompareMode)
    void    DischargeDelay_WriteCompare(uint8 compare)
            ;
    uint8 DischargeDelay_ReadCompare(void) ;
#else
    void    DischargeDelay_WriteCompare1(uint8 compare)
            ;
    uint8 DischargeDelay_ReadCompare1(void) ;
    void    DischargeDelay_WriteCompare2(uint8 compare)
            ;
    uint8 DischargeDelay_ReadCompare2(void) ;
#endif /* (DischargeDelay_UseOneCompareMode) */


#if (DischargeDelay_DeadBandUsed)
    void    DischargeDelay_WriteDeadTime(uint8 deadtime) ;
    uint8   DischargeDelay_ReadDeadTime(void) ;
#endif /* (DischargeDelay_DeadBandUsed) */

#if ( DischargeDelay_KillModeMinTime)
    void DischargeDelay_WriteKillTime(uint8 killtime) ;
    uint8 DischargeDelay_ReadKillTime(void) ;
#endif /* ( DischargeDelay_KillModeMinTime) */

void DischargeDelay_Init(void) ;
void DischargeDelay_Enable(void) ;
void DischargeDelay_Sleep(void) ;
void DischargeDelay_Wakeup(void) ;
void DischargeDelay_SaveConfig(void) ;
void DischargeDelay_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define DischargeDelay_INIT_PERIOD_VALUE          (180u)
#define DischargeDelay_INIT_COMPARE_VALUE1        (1u)
#define DischargeDelay_INIT_COMPARE_VALUE2        (63u)
#define DischargeDelay_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    DischargeDelay_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    DischargeDelay_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    DischargeDelay_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    DischargeDelay_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define DischargeDelay_DEFAULT_COMPARE2_MODE      (uint8)((uint8)1u <<  DischargeDelay_CTRL_CMPMODE2_SHIFT)
#define DischargeDelay_DEFAULT_COMPARE1_MODE      (uint8)((uint8)0u <<  DischargeDelay_CTRL_CMPMODE1_SHIFT)
#define DischargeDelay_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (DischargeDelay_UsingFixedFunction)
   #define DischargeDelay_PERIOD_LSB              (*(reg16 *) DischargeDelay_PWMHW__PER0)
   #define DischargeDelay_PERIOD_LSB_PTR          ( (reg16 *) DischargeDelay_PWMHW__PER0)
   #define DischargeDelay_COMPARE1_LSB            (*(reg16 *) DischargeDelay_PWMHW__CNT_CMP0)
   #define DischargeDelay_COMPARE1_LSB_PTR        ( (reg16 *) DischargeDelay_PWMHW__CNT_CMP0)
   #define DischargeDelay_COMPARE2_LSB            (0x00u)
   #define DischargeDelay_COMPARE2_LSB_PTR        (0x00u)
   #define DischargeDelay_COUNTER_LSB             (*(reg16 *) DischargeDelay_PWMHW__CNT_CMP0)
   #define DischargeDelay_COUNTER_LSB_PTR         ( (reg16 *) DischargeDelay_PWMHW__CNT_CMP0)
   #define DischargeDelay_CAPTURE_LSB             (*(reg16 *) DischargeDelay_PWMHW__CAP0)
   #define DischargeDelay_CAPTURE_LSB_PTR         ( (reg16 *) DischargeDelay_PWMHW__CAP0)
   #define DischargeDelay_RT1                     (*(reg8 *)  DischargeDelay_PWMHW__RT1)
   #define DischargeDelay_RT1_PTR                 ( (reg8 *)  DischargeDelay_PWMHW__RT1)

#else
   #if (DischargeDelay_Resolution == 8u) /* 8bit - PWM */

       #if(DischargeDelay_PWMModeIsCenterAligned)
           #define DischargeDelay_PERIOD_LSB      (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
           #define DischargeDelay_PERIOD_LSB_PTR  ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #else
           #define DischargeDelay_PERIOD_LSB      (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
           #define DischargeDelay_PERIOD_LSB_PTR  ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
       #endif /* (DischargeDelay_PWMModeIsCenterAligned) */

       #define DischargeDelay_COMPARE1_LSB        (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define DischargeDelay_COMPARE1_LSB_PTR    ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define DischargeDelay_COMPARE2_LSB        (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define DischargeDelay_COMPARE2_LSB_PTR    ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define DischargeDelay_COUNTERCAP_LSB      (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define DischargeDelay_COUNTERCAP_LSB_PTR  ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define DischargeDelay_COUNTER_LSB         (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define DischargeDelay_COUNTER_LSB_PTR     ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define DischargeDelay_CAPTURE_LSB         (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
       #define DischargeDelay_CAPTURE_LSB_PTR     ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(DischargeDelay_PWMModeIsCenterAligned)
               #define DischargeDelay_PERIOD_LSB      (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
               #define DischargeDelay_PERIOD_LSB_PTR  ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #else
               #define DischargeDelay_PERIOD_LSB      (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
               #define DischargeDelay_PERIOD_LSB_PTR  ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
            #endif /* (DischargeDelay_PWMModeIsCenterAligned) */

            #define DischargeDelay_COMPARE1_LSB       (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define DischargeDelay_COMPARE1_LSB_PTR   ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define DischargeDelay_COMPARE2_LSB       (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define DischargeDelay_COMPARE2_LSB_PTR   ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define DischargeDelay_COUNTERCAP_LSB     (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define DischargeDelay_COUNTERCAP_LSB_PTR ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define DischargeDelay_COUNTER_LSB        (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define DischargeDelay_COUNTER_LSB_PTR    ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define DischargeDelay_CAPTURE_LSB        (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
            #define DischargeDelay_CAPTURE_LSB_PTR    ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
        #else
            #if(DischargeDelay_PWMModeIsCenterAligned)
               #define DischargeDelay_PERIOD_LSB      (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
               #define DischargeDelay_PERIOD_LSB_PTR  ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #else
               #define DischargeDelay_PERIOD_LSB      (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
               #define DischargeDelay_PERIOD_LSB_PTR  ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
            #endif /* (DischargeDelay_PWMModeIsCenterAligned) */

            #define DischargeDelay_COMPARE1_LSB       (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define DischargeDelay_COMPARE1_LSB_PTR   ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define DischargeDelay_COMPARE2_LSB       (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define DischargeDelay_COMPARE2_LSB_PTR   ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define DischargeDelay_COUNTERCAP_LSB     (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define DischargeDelay_COUNTERCAP_LSB_PTR ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define DischargeDelay_COUNTER_LSB        (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define DischargeDelay_COUNTER_LSB_PTR    ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define DischargeDelay_CAPTURE_LSB        (*(reg16 *) DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
            #define DischargeDelay_CAPTURE_LSB_PTR    ((reg16 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define DischargeDelay_AUX_CONTROLDP1          (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)
       #define DischargeDelay_AUX_CONTROLDP1_PTR      ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (DischargeDelay_Resolution == 8) */

   #define DischargeDelay_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
   #define DischargeDelay_AUX_CONTROLDP0          (*(reg8 *)  DischargeDelay_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)
   #define DischargeDelay_AUX_CONTROLDP0_PTR      ((reg8 *)   DischargeDelay_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (DischargeDelay_UsingFixedFunction) */

#if(DischargeDelay_KillModeMinTime )
    #define DischargeDelay_KILLMODEMINTIME        (*(reg8 *)  DischargeDelay_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define DischargeDelay_KILLMODEMINTIME_PTR    ((reg8 *)   DischargeDelay_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (DischargeDelay_KillModeMinTime ) */

#if(DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_256_CLOCKS)
    #define DischargeDelay_DEADBAND_COUNT         (*(reg8 *)  DischargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define DischargeDelay_DEADBAND_COUNT_PTR     ((reg8 *)   DischargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define DischargeDelay_DEADBAND_LSB_PTR       ((reg8 *)   DischargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define DischargeDelay_DEADBAND_LSB           (*(reg8 *)  DischargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (DischargeDelay_UsingFixedFunction)
        #define DischargeDelay_DEADBAND_COUNT         (*(reg8 *)  DischargeDelay_PWMHW__CFG0)
        #define DischargeDelay_DEADBAND_COUNT_PTR     ((reg8 *)   DischargeDelay_PWMHW__CFG0)
        #define DischargeDelay_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << DischargeDelay_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define DischargeDelay_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define DischargeDelay_DEADBAND_COUNT         (*(reg8 *)  DischargeDelay_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define DischargeDelay_DEADBAND_COUNT_PTR     ((reg8 *)   DischargeDelay_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define DischargeDelay_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << DischargeDelay_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define DischargeDelay_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (DischargeDelay_UsingFixedFunction) */
#endif /* (DischargeDelay_DeadBandMode == DischargeDelay__B_PWM__DBM_256_CLOCKS) */



#if (DischargeDelay_UsingFixedFunction)
    #define DischargeDelay_STATUS                 (*(reg8 *) DischargeDelay_PWMHW__SR0)
    #define DischargeDelay_STATUS_PTR             ((reg8 *) DischargeDelay_PWMHW__SR0)
    #define DischargeDelay_STATUS_MASK            (*(reg8 *) DischargeDelay_PWMHW__SR0)
    #define DischargeDelay_STATUS_MASK_PTR        ((reg8 *) DischargeDelay_PWMHW__SR0)
    #define DischargeDelay_CONTROL                (*(reg8 *) DischargeDelay_PWMHW__CFG0)
    #define DischargeDelay_CONTROL_PTR            ((reg8 *) DischargeDelay_PWMHW__CFG0)
    #define DischargeDelay_CONTROL2               (*(reg8 *) DischargeDelay_PWMHW__CFG1)
    #define DischargeDelay_CONTROL3               (*(reg8 *) DischargeDelay_PWMHW__CFG2)
    #define DischargeDelay_GLOBAL_ENABLE          (*(reg8 *) DischargeDelay_PWMHW__PM_ACT_CFG)
    #define DischargeDelay_GLOBAL_ENABLE_PTR      ( (reg8 *) DischargeDelay_PWMHW__PM_ACT_CFG)
    #define DischargeDelay_GLOBAL_STBY_ENABLE     (*(reg8 *) DischargeDelay_PWMHW__PM_STBY_CFG)
    #define DischargeDelay_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) DischargeDelay_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define DischargeDelay_BLOCK_EN_MASK          (DischargeDelay_PWMHW__PM_ACT_MSK)
    #define DischargeDelay_BLOCK_STBY_EN_MASK     (DischargeDelay_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define DischargeDelay_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define DischargeDelay_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define DischargeDelay_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define DischargeDelay_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define DischargeDelay_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define DischargeDelay_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define DischargeDelay_CTRL_ENABLE            (uint8)((uint8)0x01u << DischargeDelay_CTRL_ENABLE_SHIFT)
    #define DischargeDelay_CTRL_RESET             (uint8)((uint8)0x01u << DischargeDelay_CTRL_RESET_SHIFT)
    #define DischargeDelay_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << DischargeDelay_CTRL_CMPMODE2_SHIFT)
    #define DischargeDelay_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << DischargeDelay_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define DischargeDelay_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define DischargeDelay_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << DischargeDelay_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define DischargeDelay_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define DischargeDelay_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define DischargeDelay_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define DischargeDelay_STATUS_TC_INT_EN_MASK_SHIFT            (DischargeDelay_STATUS_TC_SHIFT - 4u)
    #define DischargeDelay_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define DischargeDelay_STATUS_CMP1_INT_EN_MASK_SHIFT          (DischargeDelay_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define DischargeDelay_STATUS_TC              (uint8)((uint8)0x01u << DischargeDelay_STATUS_TC_SHIFT)
    #define DischargeDelay_STATUS_CMP1            (uint8)((uint8)0x01u << DischargeDelay_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define DischargeDelay_STATUS_TC_INT_EN_MASK              (uint8)((uint8)DischargeDelay_STATUS_TC >> 4u)
    #define DischargeDelay_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)DischargeDelay_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define DischargeDelay_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define DischargeDelay_RT1_MASK              (uint8)((uint8)0x03u << DischargeDelay_RT1_SHIFT)
    #define DischargeDelay_SYNC                  (uint8)((uint8)0x03u << DischargeDelay_RT1_SHIFT)
    #define DischargeDelay_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define DischargeDelay_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << DischargeDelay_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define DischargeDelay_SYNCDSI_EN            (uint8)((uint8)0x0Fu << DischargeDelay_SYNCDSI_SHIFT)


#else
    #define DischargeDelay_STATUS                (*(reg8 *)   DischargeDelay_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define DischargeDelay_STATUS_PTR            ((reg8 *)    DischargeDelay_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define DischargeDelay_STATUS_MASK           (*(reg8 *)   DischargeDelay_PWMUDB_genblk8_stsreg__MASK_REG)
    #define DischargeDelay_STATUS_MASK_PTR       ((reg8 *)    DischargeDelay_PWMUDB_genblk8_stsreg__MASK_REG)
    #define DischargeDelay_STATUS_AUX_CTRL       (*(reg8 *)   DischargeDelay_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define DischargeDelay_CONTROL               (*(reg8 *)   DischargeDelay_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define DischargeDelay_CONTROL_PTR           ((reg8 *)    DischargeDelay_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define DischargeDelay_CTRL_ENABLE_SHIFT      (0x07u)
    #define DischargeDelay_CTRL_RESET_SHIFT       (0x06u)
    #define DischargeDelay_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define DischargeDelay_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define DischargeDelay_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define DischargeDelay_CTRL_ENABLE            (uint8)((uint8)0x01u << DischargeDelay_CTRL_ENABLE_SHIFT)
    #define DischargeDelay_CTRL_RESET             (uint8)((uint8)0x01u << DischargeDelay_CTRL_RESET_SHIFT)
    #define DischargeDelay_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << DischargeDelay_CTRL_CMPMODE2_SHIFT)
    #define DischargeDelay_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << DischargeDelay_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define DischargeDelay_STATUS_KILL_SHIFT          (0x05u)
    #define DischargeDelay_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define DischargeDelay_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define DischargeDelay_STATUS_TC_SHIFT            (0x02u)
    #define DischargeDelay_STATUS_CMP2_SHIFT          (0x01u)
    #define DischargeDelay_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define DischargeDelay_STATUS_KILL_INT_EN_MASK_SHIFT          (DischargeDelay_STATUS_KILL_SHIFT)
    #define DischargeDelay_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (DischargeDelay_STATUS_FIFONEMPTY_SHIFT)
    #define DischargeDelay_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (DischargeDelay_STATUS_FIFOFULL_SHIFT)
    #define DischargeDelay_STATUS_TC_INT_EN_MASK_SHIFT            (DischargeDelay_STATUS_TC_SHIFT)
    #define DischargeDelay_STATUS_CMP2_INT_EN_MASK_SHIFT          (DischargeDelay_STATUS_CMP2_SHIFT)
    #define DischargeDelay_STATUS_CMP1_INT_EN_MASK_SHIFT          (DischargeDelay_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define DischargeDelay_STATUS_KILL            (uint8)((uint8)0x00u << DischargeDelay_STATUS_KILL_SHIFT )
    #define DischargeDelay_STATUS_FIFOFULL        (uint8)((uint8)0x01u << DischargeDelay_STATUS_FIFOFULL_SHIFT)
    #define DischargeDelay_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << DischargeDelay_STATUS_FIFONEMPTY_SHIFT)
    #define DischargeDelay_STATUS_TC              (uint8)((uint8)0x01u << DischargeDelay_STATUS_TC_SHIFT)
    #define DischargeDelay_STATUS_CMP2            (uint8)((uint8)0x01u << DischargeDelay_STATUS_CMP2_SHIFT)
    #define DischargeDelay_STATUS_CMP1            (uint8)((uint8)0x01u << DischargeDelay_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define DischargeDelay_STATUS_KILL_INT_EN_MASK            (DischargeDelay_STATUS_KILL)
    #define DischargeDelay_STATUS_FIFOFULL_INT_EN_MASK        (DischargeDelay_STATUS_FIFOFULL)
    #define DischargeDelay_STATUS_FIFONEMPTY_INT_EN_MASK      (DischargeDelay_STATUS_FIFONEMPTY)
    #define DischargeDelay_STATUS_TC_INT_EN_MASK              (DischargeDelay_STATUS_TC)
    #define DischargeDelay_STATUS_CMP2_INT_EN_MASK            (DischargeDelay_STATUS_CMP2)
    #define DischargeDelay_STATUS_CMP1_INT_EN_MASK            (DischargeDelay_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define DischargeDelay_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define DischargeDelay_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define DischargeDelay_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define DischargeDelay_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define DischargeDelay_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* DischargeDelay_UsingFixedFunction */

#endif  /* CY_PWM_DischargeDelay_H */


/* [] END OF FILE */
