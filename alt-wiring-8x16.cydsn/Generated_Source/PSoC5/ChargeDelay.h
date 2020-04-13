/*******************************************************************************
* File Name: ChargeDelay.h
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

#if !defined(CY_PWM_ChargeDelay_H)
#define CY_PWM_ChargeDelay_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 ChargeDelay_initVar;


/***************************************
* Conditional Compilation Parameters
***************************************/
#define ChargeDelay_Resolution                     (8u)
#define ChargeDelay_UsingFixedFunction             (0u)
#define ChargeDelay_DeadBandMode                   (0u)
#define ChargeDelay_KillModeMinTime                (0u)
#define ChargeDelay_KillMode                       (0u)
#define ChargeDelay_PWMMode                        (0u)
#define ChargeDelay_PWMModeIsCenterAligned         (0u)
#define ChargeDelay_DeadBandUsed                   (0u)
#define ChargeDelay_DeadBand2_4                    (0u)

#if !defined(ChargeDelay_PWMUDB_genblk8_stsreg__REMOVED)
    #define ChargeDelay_UseStatus                  (0u)
#else
    #define ChargeDelay_UseStatus                  (0u)
#endif /* !defined(ChargeDelay_PWMUDB_genblk8_stsreg__REMOVED) */

#if !defined(ChargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED)
    #define ChargeDelay_UseControl                 (0u)
#else
    #define ChargeDelay_UseControl                 (0u)
#endif /* !defined(ChargeDelay_PWMUDB_genblk1_ctrlreg__REMOVED) */

#define ChargeDelay_UseOneCompareMode              (1u)
#define ChargeDelay_MinimumKillTime                (1u)
#define ChargeDelay_EnableMode                     (1u)

#define ChargeDelay_CompareMode1SW                 (0u)
#define ChargeDelay_CompareMode2SW                 (0u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component PWM_v3_30 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */

/* Use Kill Mode Enumerated Types */
#define ChargeDelay__B_PWM__DISABLED 0
#define ChargeDelay__B_PWM__ASYNCHRONOUS 1
#define ChargeDelay__B_PWM__SINGLECYCLE 2
#define ChargeDelay__B_PWM__LATCHED 3
#define ChargeDelay__B_PWM__MINTIME 4


/* Use Dead Band Mode Enumerated Types */
#define ChargeDelay__B_PWM__DBMDISABLED 0
#define ChargeDelay__B_PWM__DBM_2_4_CLOCKS 1
#define ChargeDelay__B_PWM__DBM_256_CLOCKS 2


/* Used PWM Mode Enumerated Types */
#define ChargeDelay__B_PWM__ONE_OUTPUT 0
#define ChargeDelay__B_PWM__TWO_OUTPUTS 1
#define ChargeDelay__B_PWM__DUAL_EDGE 2
#define ChargeDelay__B_PWM__CENTER_ALIGN 3
#define ChargeDelay__B_PWM__DITHER 5
#define ChargeDelay__B_PWM__HARDWARESELECT 4


/* Used PWM Compare Mode Enumerated Types */
#define ChargeDelay__B_PWM__LESS_THAN 1
#define ChargeDelay__B_PWM__LESS_THAN_OR_EQUAL 2
#define ChargeDelay__B_PWM__GREATER_THAN 3
#define ChargeDelay__B_PWM__GREATER_THAN_OR_EQUAL_TO 4
#define ChargeDelay__B_PWM__EQUAL 0
#define ChargeDelay__B_PWM__FIRMWARE 5



/***************************************
* Data Struct Definition
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for PWM Component
 *************************************************************************/
typedef struct
{

    uint8 PWMEnableState;

    #if(!ChargeDelay_UsingFixedFunction)
        uint8 PWMUdb;               /* PWM Current Counter value  */
        #if(!ChargeDelay_PWMModeIsCenterAligned)
            uint8 PWMPeriod;
        #endif /* (!ChargeDelay_PWMModeIsCenterAligned) */
        #if (ChargeDelay_UseStatus)
            uint8 InterruptMaskValue;   /* PWM Current Interrupt Mask */
        #endif /* (ChargeDelay_UseStatus) */

        /* Backup for Deadband parameters */
        #if(ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_256_CLOCKS || \
            ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_2_4_CLOCKS)
            uint8 PWMdeadBandValue; /* Dead Band Counter Current Value */
        #endif /* deadband count is either 2-4 clocks or 256 clocks */

        /* Backup Kill Mode Counter*/
        #if(ChargeDelay_KillModeMinTime)
            uint8 PWMKillCounterPeriod; /* Kill Mode period value */
        #endif /* (ChargeDelay_KillModeMinTime) */

        /* Backup control register */
        #if(ChargeDelay_UseControl)
            uint8 PWMControlRegister; /* PWM Control Register value */
        #endif /* (ChargeDelay_UseControl) */

    #endif /* (!ChargeDelay_UsingFixedFunction) */

}ChargeDelay_backupStruct;


/***************************************
*        Function Prototypes
 **************************************/

void    ChargeDelay_Start(void) ;
void    ChargeDelay_Stop(void) ;

#if (ChargeDelay_UseStatus || ChargeDelay_UsingFixedFunction)
    void  ChargeDelay_SetInterruptMode(uint8 interruptMode) ;
    uint8 ChargeDelay_ReadStatusRegister(void) ;
#endif /* (ChargeDelay_UseStatus || ChargeDelay_UsingFixedFunction) */

#define ChargeDelay_GetInterruptSource() ChargeDelay_ReadStatusRegister()

#if (ChargeDelay_UseControl)
    uint8 ChargeDelay_ReadControlRegister(void) ;
    void  ChargeDelay_WriteControlRegister(uint8 control)
          ;
#endif /* (ChargeDelay_UseControl) */

#if (ChargeDelay_UseOneCompareMode)
   #if (ChargeDelay_CompareMode1SW)
       void    ChargeDelay_SetCompareMode(uint8 comparemode)
               ;
   #endif /* (ChargeDelay_CompareMode1SW) */
#else
    #if (ChargeDelay_CompareMode1SW)
        void    ChargeDelay_SetCompareMode1(uint8 comparemode)
                ;
    #endif /* (ChargeDelay_CompareMode1SW) */
    #if (ChargeDelay_CompareMode2SW)
        void    ChargeDelay_SetCompareMode2(uint8 comparemode)
                ;
    #endif /* (ChargeDelay_CompareMode2SW) */
#endif /* (ChargeDelay_UseOneCompareMode) */

#if (!ChargeDelay_UsingFixedFunction)
    uint8   ChargeDelay_ReadCounter(void) ;
    uint8 ChargeDelay_ReadCapture(void) ;

    #if (ChargeDelay_UseStatus)
            void ChargeDelay_ClearFIFO(void) ;
    #endif /* (ChargeDelay_UseStatus) */

    void    ChargeDelay_WriteCounter(uint8 counter)
            ;
#endif /* (!ChargeDelay_UsingFixedFunction) */

void    ChargeDelay_WritePeriod(uint8 period)
        ;
uint8 ChargeDelay_ReadPeriod(void) ;

#if (ChargeDelay_UseOneCompareMode)
    void    ChargeDelay_WriteCompare(uint8 compare)
            ;
    uint8 ChargeDelay_ReadCompare(void) ;
#else
    void    ChargeDelay_WriteCompare1(uint8 compare)
            ;
    uint8 ChargeDelay_ReadCompare1(void) ;
    void    ChargeDelay_WriteCompare2(uint8 compare)
            ;
    uint8 ChargeDelay_ReadCompare2(void) ;
#endif /* (ChargeDelay_UseOneCompareMode) */


#if (ChargeDelay_DeadBandUsed)
    void    ChargeDelay_WriteDeadTime(uint8 deadtime) ;
    uint8   ChargeDelay_ReadDeadTime(void) ;
#endif /* (ChargeDelay_DeadBandUsed) */

#if ( ChargeDelay_KillModeMinTime)
    void ChargeDelay_WriteKillTime(uint8 killtime) ;
    uint8 ChargeDelay_ReadKillTime(void) ;
#endif /* ( ChargeDelay_KillModeMinTime) */

void ChargeDelay_Init(void) ;
void ChargeDelay_Enable(void) ;
void ChargeDelay_Sleep(void) ;
void ChargeDelay_Wakeup(void) ;
void ChargeDelay_SaveConfig(void) ;
void ChargeDelay_RestoreConfig(void) ;


/***************************************
*         Initialization Values
**************************************/
#define ChargeDelay_INIT_PERIOD_VALUE          (28u)
#define ChargeDelay_INIT_COMPARE_VALUE1        (1u)
#define ChargeDelay_INIT_COMPARE_VALUE2        (63u)
#define ChargeDelay_INIT_INTERRUPTS_MODE       (uint8)(((uint8)(0u <<   \
                                                    ChargeDelay_STATUS_TC_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    ChargeDelay_STATUS_CMP2_INT_EN_MASK_SHIFT)) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    ChargeDelay_STATUS_CMP1_INT_EN_MASK_SHIFT )) | \
                                                    (uint8)((uint8)(0u <<  \
                                                    ChargeDelay_STATUS_KILL_INT_EN_MASK_SHIFT )))
#define ChargeDelay_DEFAULT_COMPARE2_MODE      (uint8)((uint8)1u <<  ChargeDelay_CTRL_CMPMODE2_SHIFT)
#define ChargeDelay_DEFAULT_COMPARE1_MODE      (uint8)((uint8)1u <<  ChargeDelay_CTRL_CMPMODE1_SHIFT)
#define ChargeDelay_INIT_DEAD_TIME             (1u)


/********************************
*         Registers
******************************** */

#if (ChargeDelay_UsingFixedFunction)
   #define ChargeDelay_PERIOD_LSB              (*(reg16 *) ChargeDelay_PWMHW__PER0)
   #define ChargeDelay_PERIOD_LSB_PTR          ( (reg16 *) ChargeDelay_PWMHW__PER0)
   #define ChargeDelay_COMPARE1_LSB            (*(reg16 *) ChargeDelay_PWMHW__CNT_CMP0)
   #define ChargeDelay_COMPARE1_LSB_PTR        ( (reg16 *) ChargeDelay_PWMHW__CNT_CMP0)
   #define ChargeDelay_COMPARE2_LSB            (0x00u)
   #define ChargeDelay_COMPARE2_LSB_PTR        (0x00u)
   #define ChargeDelay_COUNTER_LSB             (*(reg16 *) ChargeDelay_PWMHW__CNT_CMP0)
   #define ChargeDelay_COUNTER_LSB_PTR         ( (reg16 *) ChargeDelay_PWMHW__CNT_CMP0)
   #define ChargeDelay_CAPTURE_LSB             (*(reg16 *) ChargeDelay_PWMHW__CAP0)
   #define ChargeDelay_CAPTURE_LSB_PTR         ( (reg16 *) ChargeDelay_PWMHW__CAP0)
   #define ChargeDelay_RT1                     (*(reg8 *)  ChargeDelay_PWMHW__RT1)
   #define ChargeDelay_RT1_PTR                 ( (reg8 *)  ChargeDelay_PWMHW__RT1)

#else
   #if (ChargeDelay_Resolution == 8u) /* 8bit - PWM */

       #if(ChargeDelay_PWMModeIsCenterAligned)
           #define ChargeDelay_PERIOD_LSB      (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
           #define ChargeDelay_PERIOD_LSB_PTR  ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #else
           #define ChargeDelay_PERIOD_LSB      (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
           #define ChargeDelay_PERIOD_LSB_PTR  ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
       #endif /* (ChargeDelay_PWMModeIsCenterAligned) */

       #define ChargeDelay_COMPARE1_LSB        (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define ChargeDelay_COMPARE1_LSB_PTR    ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
       #define ChargeDelay_COMPARE2_LSB        (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define ChargeDelay_COMPARE2_LSB_PTR    ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
       #define ChargeDelay_COUNTERCAP_LSB      (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define ChargeDelay_COUNTERCAP_LSB_PTR  ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
       #define ChargeDelay_COUNTER_LSB         (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define ChargeDelay_COUNTER_LSB_PTR     ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
       #define ChargeDelay_CAPTURE_LSB         (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
       #define ChargeDelay_CAPTURE_LSB_PTR     ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)

   #else
        #if(CY_PSOC3) /* 8-bit address space */
            #if(ChargeDelay_PWMModeIsCenterAligned)
               #define ChargeDelay_PERIOD_LSB      (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
               #define ChargeDelay_PERIOD_LSB_PTR  ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #else
               #define ChargeDelay_PERIOD_LSB      (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
               #define ChargeDelay_PERIOD_LSB_PTR  ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__F0_REG)
            #endif /* (ChargeDelay_PWMModeIsCenterAligned) */

            #define ChargeDelay_COMPARE1_LSB       (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define ChargeDelay_COMPARE1_LSB_PTR   ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__D0_REG)
            #define ChargeDelay_COMPARE2_LSB       (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define ChargeDelay_COMPARE2_LSB_PTR   ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__D1_REG)
            #define ChargeDelay_COUNTERCAP_LSB     (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define ChargeDelay_COUNTERCAP_LSB_PTR ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
            #define ChargeDelay_COUNTER_LSB        (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define ChargeDelay_COUNTER_LSB_PTR    ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__A0_REG)
            #define ChargeDelay_CAPTURE_LSB        (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
            #define ChargeDelay_CAPTURE_LSB_PTR    ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__F1_REG)
        #else
            #if(ChargeDelay_PWMModeIsCenterAligned)
               #define ChargeDelay_PERIOD_LSB      (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
               #define ChargeDelay_PERIOD_LSB_PTR  ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #else
               #define ChargeDelay_PERIOD_LSB      (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
               #define ChargeDelay_PERIOD_LSB_PTR  ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F0_REG)
            #endif /* (ChargeDelay_PWMModeIsCenterAligned) */

            #define ChargeDelay_COMPARE1_LSB       (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define ChargeDelay_COMPARE1_LSB_PTR   ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D0_REG)
            #define ChargeDelay_COMPARE2_LSB       (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define ChargeDelay_COMPARE2_LSB_PTR   ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_D1_REG)
            #define ChargeDelay_COUNTERCAP_LSB     (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define ChargeDelay_COUNTERCAP_LSB_PTR ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A1_REG)
            #define ChargeDelay_COUNTER_LSB        (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define ChargeDelay_COUNTER_LSB_PTR    ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_A0_REG)
            #define ChargeDelay_CAPTURE_LSB        (*(reg16 *) ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
            #define ChargeDelay_CAPTURE_LSB_PTR    ((reg16 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__16BIT_F1_REG)
        #endif /* (CY_PSOC3) */

       #define ChargeDelay_AUX_CONTROLDP1          (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)
       #define ChargeDelay_AUX_CONTROLDP1_PTR      ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u1__DP_AUX_CTL_REG)

   #endif /* (ChargeDelay_Resolution == 8) */

   #define ChargeDelay_COUNTERCAP_LSB_PTR_8BIT ( (reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__A1_REG)
   #define ChargeDelay_AUX_CONTROLDP0          (*(reg8 *)  ChargeDelay_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)
   #define ChargeDelay_AUX_CONTROLDP0_PTR      ((reg8 *)   ChargeDelay_PWMUDB_sP8_pwmdp_u0__DP_AUX_CTL_REG)

#endif /* (ChargeDelay_UsingFixedFunction) */

#if(ChargeDelay_KillModeMinTime )
    #define ChargeDelay_KILLMODEMINTIME        (*(reg8 *)  ChargeDelay_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    #define ChargeDelay_KILLMODEMINTIME_PTR    ((reg8 *)   ChargeDelay_PWMUDB_sKM_killmodecounterdp_u0__D0_REG)
    /* Fixed Function Block has no Kill Mode parameters because it is Asynchronous only */
#endif /* (ChargeDelay_KillModeMinTime ) */

#if(ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_256_CLOCKS)
    #define ChargeDelay_DEADBAND_COUNT         (*(reg8 *)  ChargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define ChargeDelay_DEADBAND_COUNT_PTR     ((reg8 *)   ChargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__D0_REG)
    #define ChargeDelay_DEADBAND_LSB_PTR       ((reg8 *)   ChargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
    #define ChargeDelay_DEADBAND_LSB           (*(reg8 *)  ChargeDelay_PWMUDB_sDB255_deadbandcounterdp_u0__A0_REG)
#elif(ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_2_4_CLOCKS)
    
    /* In Fixed Function Block these bits are in the control blocks control register */
    #if (ChargeDelay_UsingFixedFunction)
        #define ChargeDelay_DEADBAND_COUNT         (*(reg8 *)  ChargeDelay_PWMHW__CFG0)
        #define ChargeDelay_DEADBAND_COUNT_PTR     ((reg8 *)   ChargeDelay_PWMHW__CFG0)
        #define ChargeDelay_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << ChargeDelay_DEADBAND_COUNT_SHIFT)

        /* As defined by the Register Map as DEADBAND_PERIOD[1:0] in CFG0 */
        #define ChargeDelay_DEADBAND_COUNT_SHIFT   (0x06u)
    #else
        /* Lower two bits of the added control register define the count 1-3 */
        #define ChargeDelay_DEADBAND_COUNT         (*(reg8 *)  ChargeDelay_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define ChargeDelay_DEADBAND_COUNT_PTR     ((reg8 *)   ChargeDelay_PWMUDB_genblk7_dbctrlreg__CONTROL_REG)
        #define ChargeDelay_DEADBAND_COUNT_MASK    (uint8)((uint8)0x03u << ChargeDelay_DEADBAND_COUNT_SHIFT)

        /* As defined by the verilog implementation of the Control Register */
        #define ChargeDelay_DEADBAND_COUNT_SHIFT   (0x00u)
    #endif /* (ChargeDelay_UsingFixedFunction) */
#endif /* (ChargeDelay_DeadBandMode == ChargeDelay__B_PWM__DBM_256_CLOCKS) */



#if (ChargeDelay_UsingFixedFunction)
    #define ChargeDelay_STATUS                 (*(reg8 *) ChargeDelay_PWMHW__SR0)
    #define ChargeDelay_STATUS_PTR             ((reg8 *) ChargeDelay_PWMHW__SR0)
    #define ChargeDelay_STATUS_MASK            (*(reg8 *) ChargeDelay_PWMHW__SR0)
    #define ChargeDelay_STATUS_MASK_PTR        ((reg8 *) ChargeDelay_PWMHW__SR0)
    #define ChargeDelay_CONTROL                (*(reg8 *) ChargeDelay_PWMHW__CFG0)
    #define ChargeDelay_CONTROL_PTR            ((reg8 *) ChargeDelay_PWMHW__CFG0)
    #define ChargeDelay_CONTROL2               (*(reg8 *) ChargeDelay_PWMHW__CFG1)
    #define ChargeDelay_CONTROL3               (*(reg8 *) ChargeDelay_PWMHW__CFG2)
    #define ChargeDelay_GLOBAL_ENABLE          (*(reg8 *) ChargeDelay_PWMHW__PM_ACT_CFG)
    #define ChargeDelay_GLOBAL_ENABLE_PTR      ( (reg8 *) ChargeDelay_PWMHW__PM_ACT_CFG)
    #define ChargeDelay_GLOBAL_STBY_ENABLE     (*(reg8 *) ChargeDelay_PWMHW__PM_STBY_CFG)
    #define ChargeDelay_GLOBAL_STBY_ENABLE_PTR ( (reg8 *) ChargeDelay_PWMHW__PM_STBY_CFG)


    /***********************************
    *          Constants
    ***********************************/

    /* Fixed Function Block Chosen */
    #define ChargeDelay_BLOCK_EN_MASK          (ChargeDelay_PWMHW__PM_ACT_MSK)
    #define ChargeDelay_BLOCK_STBY_EN_MASK     (ChargeDelay_PWMHW__PM_STBY_MSK)
    
    /* Control Register definitions */
    #define ChargeDelay_CTRL_ENABLE_SHIFT      (0x00u)

    /* As defined by Register map as MODE_CFG bits in CFG2*/
    #define ChargeDelay_CTRL_CMPMODE1_SHIFT    (0x04u)

    /* As defined by Register map */
    #define ChargeDelay_CTRL_DEAD_TIME_SHIFT   (0x06u)  

    /* Fixed Function Block Only CFG register bit definitions */
    /*  Set to compare mode */
    #define ChargeDelay_CFG0_MODE              (0x02u)   

    /* Enable the block to run */
    #define ChargeDelay_CFG0_ENABLE            (0x01u)   
    
    /* As defined by Register map as DB bit in CFG0 */
    #define ChargeDelay_CFG0_DB                (0x20u)   

    /* Control Register Bit Masks */
    #define ChargeDelay_CTRL_ENABLE            (uint8)((uint8)0x01u << ChargeDelay_CTRL_ENABLE_SHIFT)
    #define ChargeDelay_CTRL_RESET             (uint8)((uint8)0x01u << ChargeDelay_CTRL_RESET_SHIFT)
    #define ChargeDelay_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << ChargeDelay_CTRL_CMPMODE2_SHIFT)
    #define ChargeDelay_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << ChargeDelay_CTRL_CMPMODE1_SHIFT)

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define ChargeDelay_CTRL2_IRQ_SEL_SHIFT    (0x00u)
    #define ChargeDelay_CTRL2_IRQ_SEL          (uint8)((uint8)0x01u << ChargeDelay_CTRL2_IRQ_SEL_SHIFT)

    /* Status Register Bit Locations */
    /* As defined by Register map as TC in SR0 */
    #define ChargeDelay_STATUS_TC_SHIFT        (0x07u)   
    
    /* As defined by the Register map as CAP_CMP in SR0 */
    #define ChargeDelay_STATUS_CMP1_SHIFT      (0x06u)   

    /* Status Register Interrupt Enable Bit Locations */
    #define ChargeDelay_STATUS_KILL_INT_EN_MASK_SHIFT          (0x00u)
    #define ChargeDelay_STATUS_TC_INT_EN_MASK_SHIFT            (ChargeDelay_STATUS_TC_SHIFT - 4u)
    #define ChargeDelay_STATUS_CMP2_INT_EN_MASK_SHIFT          (0x00u)
    #define ChargeDelay_STATUS_CMP1_INT_EN_MASK_SHIFT          (ChargeDelay_STATUS_CMP1_SHIFT - 4u)

    /* Status Register Bit Masks */
    #define ChargeDelay_STATUS_TC              (uint8)((uint8)0x01u << ChargeDelay_STATUS_TC_SHIFT)
    #define ChargeDelay_STATUS_CMP1            (uint8)((uint8)0x01u << ChargeDelay_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks */
    #define ChargeDelay_STATUS_TC_INT_EN_MASK              (uint8)((uint8)ChargeDelay_STATUS_TC >> 4u)
    #define ChargeDelay_STATUS_CMP1_INT_EN_MASK            (uint8)((uint8)ChargeDelay_STATUS_CMP1 >> 4u)

    /*RT1 Synch Constants */
    #define ChargeDelay_RT1_SHIFT             (0x04u)

    /* Sync TC and CMP bit masks */
    #define ChargeDelay_RT1_MASK              (uint8)((uint8)0x03u << ChargeDelay_RT1_SHIFT)
    #define ChargeDelay_SYNC                  (uint8)((uint8)0x03u << ChargeDelay_RT1_SHIFT)
    #define ChargeDelay_SYNCDSI_SHIFT         (0x00u)

    /* Sync all DSI inputs */
    #define ChargeDelay_SYNCDSI_MASK          (uint8)((uint8)0x0Fu << ChargeDelay_SYNCDSI_SHIFT)

    /* Sync all DSI inputs */
    #define ChargeDelay_SYNCDSI_EN            (uint8)((uint8)0x0Fu << ChargeDelay_SYNCDSI_SHIFT)


#else
    #define ChargeDelay_STATUS                (*(reg8 *)   ChargeDelay_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define ChargeDelay_STATUS_PTR            ((reg8 *)    ChargeDelay_PWMUDB_genblk8_stsreg__STATUS_REG )
    #define ChargeDelay_STATUS_MASK           (*(reg8 *)   ChargeDelay_PWMUDB_genblk8_stsreg__MASK_REG)
    #define ChargeDelay_STATUS_MASK_PTR       ((reg8 *)    ChargeDelay_PWMUDB_genblk8_stsreg__MASK_REG)
    #define ChargeDelay_STATUS_AUX_CTRL       (*(reg8 *)   ChargeDelay_PWMUDB_genblk8_stsreg__STATUS_AUX_CTL_REG)
    #define ChargeDelay_CONTROL               (*(reg8 *)   ChargeDelay_PWMUDB_genblk1_ctrlreg__CONTROL_REG)
    #define ChargeDelay_CONTROL_PTR           ((reg8 *)    ChargeDelay_PWMUDB_genblk1_ctrlreg__CONTROL_REG)


    /***********************************
    *          Constants
    ***********************************/

    /* Control Register bit definitions */
    #define ChargeDelay_CTRL_ENABLE_SHIFT      (0x07u)
    #define ChargeDelay_CTRL_RESET_SHIFT       (0x06u)
    #define ChargeDelay_CTRL_CMPMODE2_SHIFT    (0x03u)
    #define ChargeDelay_CTRL_CMPMODE1_SHIFT    (0x00u)
    #define ChargeDelay_CTRL_DEAD_TIME_SHIFT   (0x00u)   /* No Shift Needed for UDB block */
    
    /* Control Register Bit Masks */
    #define ChargeDelay_CTRL_ENABLE            (uint8)((uint8)0x01u << ChargeDelay_CTRL_ENABLE_SHIFT)
    #define ChargeDelay_CTRL_RESET             (uint8)((uint8)0x01u << ChargeDelay_CTRL_RESET_SHIFT)
    #define ChargeDelay_CTRL_CMPMODE2_MASK     (uint8)((uint8)0x07u << ChargeDelay_CTRL_CMPMODE2_SHIFT)
    #define ChargeDelay_CTRL_CMPMODE1_MASK     (uint8)((uint8)0x07u << ChargeDelay_CTRL_CMPMODE1_SHIFT)

    /* Status Register Bit Locations */
    #define ChargeDelay_STATUS_KILL_SHIFT          (0x05u)
    #define ChargeDelay_STATUS_FIFONEMPTY_SHIFT    (0x04u)
    #define ChargeDelay_STATUS_FIFOFULL_SHIFT      (0x03u)
    #define ChargeDelay_STATUS_TC_SHIFT            (0x02u)
    #define ChargeDelay_STATUS_CMP2_SHIFT          (0x01u)
    #define ChargeDelay_STATUS_CMP1_SHIFT          (0x00u)

    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define ChargeDelay_STATUS_KILL_INT_EN_MASK_SHIFT          (ChargeDelay_STATUS_KILL_SHIFT)
    #define ChargeDelay_STATUS_FIFONEMPTY_INT_EN_MASK_SHIFT    (ChargeDelay_STATUS_FIFONEMPTY_SHIFT)
    #define ChargeDelay_STATUS_FIFOFULL_INT_EN_MASK_SHIFT      (ChargeDelay_STATUS_FIFOFULL_SHIFT)
    #define ChargeDelay_STATUS_TC_INT_EN_MASK_SHIFT            (ChargeDelay_STATUS_TC_SHIFT)
    #define ChargeDelay_STATUS_CMP2_INT_EN_MASK_SHIFT          (ChargeDelay_STATUS_CMP2_SHIFT)
    #define ChargeDelay_STATUS_CMP1_INT_EN_MASK_SHIFT          (ChargeDelay_STATUS_CMP1_SHIFT)

    /* Status Register Bit Masks */
    #define ChargeDelay_STATUS_KILL            (uint8)((uint8)0x00u << ChargeDelay_STATUS_KILL_SHIFT )
    #define ChargeDelay_STATUS_FIFOFULL        (uint8)((uint8)0x01u << ChargeDelay_STATUS_FIFOFULL_SHIFT)
    #define ChargeDelay_STATUS_FIFONEMPTY      (uint8)((uint8)0x01u << ChargeDelay_STATUS_FIFONEMPTY_SHIFT)
    #define ChargeDelay_STATUS_TC              (uint8)((uint8)0x01u << ChargeDelay_STATUS_TC_SHIFT)
    #define ChargeDelay_STATUS_CMP2            (uint8)((uint8)0x01u << ChargeDelay_STATUS_CMP2_SHIFT)
    #define ChargeDelay_STATUS_CMP1            (uint8)((uint8)0x01u << ChargeDelay_STATUS_CMP1_SHIFT)

    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define ChargeDelay_STATUS_KILL_INT_EN_MASK            (ChargeDelay_STATUS_KILL)
    #define ChargeDelay_STATUS_FIFOFULL_INT_EN_MASK        (ChargeDelay_STATUS_FIFOFULL)
    #define ChargeDelay_STATUS_FIFONEMPTY_INT_EN_MASK      (ChargeDelay_STATUS_FIFONEMPTY)
    #define ChargeDelay_STATUS_TC_INT_EN_MASK              (ChargeDelay_STATUS_TC)
    #define ChargeDelay_STATUS_CMP2_INT_EN_MASK            (ChargeDelay_STATUS_CMP2)
    #define ChargeDelay_STATUS_CMP1_INT_EN_MASK            (ChargeDelay_STATUS_CMP1)

    /* Datapath Auxillary Control Register bit definitions */
    #define ChargeDelay_AUX_CTRL_FIFO0_CLR         (0x01u)
    #define ChargeDelay_AUX_CTRL_FIFO1_CLR         (0x02u)
    #define ChargeDelay_AUX_CTRL_FIFO0_LVL         (0x04u)
    #define ChargeDelay_AUX_CTRL_FIFO1_LVL         (0x08u)
    #define ChargeDelay_STATUS_ACTL_INT_EN_MASK    (0x10u) /* As defined for the ACTL Register */
#endif /* ChargeDelay_UsingFixedFunction */

#endif  /* CY_PWM_ChargeDelay_H */


/* [] END OF FILE */
