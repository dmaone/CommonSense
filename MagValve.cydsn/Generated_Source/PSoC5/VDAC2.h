/*******************************************************************************
* File Name: VDAC2.h  
* Version 1.90
*
*  Description:
*    This file contains the function prototypes and constants used in
*    the 8-bit Voltage DAC (vDAC8) User Module.
*
*   Note:
*     
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_VDAC8_VDAC2_H) 
#define CY_VDAC8_VDAC2_H

#include "cytypes.h"
#include "cyfitter.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component VDAC8_v1_90 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/***************************************
*       Type defines
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState; 
    uint8 data_value;
}VDAC2_backupStruct;

/* component init state */
extern uint8 VDAC2_initVar;


/***************************************
*        Function Prototypes 
***************************************/

void VDAC2_Start(void)           ;
void VDAC2_Stop(void)            ;
void VDAC2_SetSpeed(uint8 speed) ;
void VDAC2_SetRange(uint8 range) ;
void VDAC2_SetValue(uint8 value) ;
void VDAC2_DacTrim(void)         ;
void VDAC2_Init(void)            ;
void VDAC2_Enable(void)          ;
void VDAC2_SaveConfig(void)      ;
void VDAC2_RestoreConfig(void)   ;
void VDAC2_Sleep(void)           ;
void VDAC2_Wakeup(void)          ;


/***************************************
*            API Constants
***************************************/

/* SetRange constants */

#define VDAC2_RANGE_1V       0x00u
#define VDAC2_RANGE_4V       0x04u


/* Power setting for Start API  */
#define VDAC2_LOWSPEED       0x00u
#define VDAC2_HIGHSPEED      0x02u


/***************************************
*  Initialization Parameter Constants
***************************************/

 /* Default DAC range */
#define VDAC2_DEFAULT_RANGE    0u
 /* Default DAC speed */
#define VDAC2_DEFAULT_SPEED    2u
 /* Default Control */
#define VDAC2_DEFAULT_CNTL      0x00u
/* Default Strobe mode */
#define VDAC2_DEFAULT_STRB     0u
 /* Initial DAC value */
#define VDAC2_DEFAULT_DATA     1u
 /* Default Data Source */
#define VDAC2_DEFAULT_DATA_SRC 0u


/***************************************
*              Registers        
***************************************/
#define VDAC2_CR0_REG            (* (reg8 *) VDAC2_viDAC8__CR0 )
#define VDAC2_CR0_PTR            (  (reg8 *) VDAC2_viDAC8__CR0 )
#define VDAC2_CR1_REG            (* (reg8 *) VDAC2_viDAC8__CR1 )
#define VDAC2_CR1_PTR            (  (reg8 *) VDAC2_viDAC8__CR1 )
#define VDAC2_Data_REG           (* (reg8 *) VDAC2_viDAC8__D )
#define VDAC2_Data_PTR           (  (reg8 *) VDAC2_viDAC8__D )
#define VDAC2_Strobe_REG         (* (reg8 *) VDAC2_viDAC8__STROBE )
#define VDAC2_Strobe_PTR         (  (reg8 *) VDAC2_viDAC8__STROBE )
#define VDAC2_SW0_REG            (* (reg8 *) VDAC2_viDAC8__SW0 )
#define VDAC2_SW0_PTR            (  (reg8 *) VDAC2_viDAC8__SW0 )
#define VDAC2_SW2_REG            (* (reg8 *) VDAC2_viDAC8__SW2 )
#define VDAC2_SW2_PTR            (  (reg8 *) VDAC2_viDAC8__SW2 )
#define VDAC2_SW3_REG            (* (reg8 *) VDAC2_viDAC8__SW3 )
#define VDAC2_SW3_PTR            (  (reg8 *) VDAC2_viDAC8__SW3 )
#define VDAC2_SW4_REG            (* (reg8 *) VDAC2_viDAC8__SW4 )
#define VDAC2_SW4_PTR            (  (reg8 *) VDAC2_viDAC8__SW4 )
#define VDAC2_TR_REG             (* (reg8 *) VDAC2_viDAC8__TR )
#define VDAC2_TR_PTR             (  (reg8 *) VDAC2_viDAC8__TR )
/* Power manager */
#define VDAC2_PWRMGR_REG         (* (reg8 *) VDAC2_viDAC8__PM_ACT_CFG )
#define VDAC2_PWRMGR_PTR         (  (reg8 *) VDAC2_viDAC8__PM_ACT_CFG )
  /* Standby Power manager */
#define VDAC2_STBY_PWRMGR_REG    (* (reg8 *) VDAC2_viDAC8__PM_STBY_CFG )
#define VDAC2_STBY_PWRMGR_PTR    (  (reg8 *) VDAC2_viDAC8__PM_STBY_CFG )

/***************************************
*  Registers definitions
* for backward capability        
***************************************/
#define VDAC2_CR0         (* (reg8 *) VDAC2_viDAC8__CR0 )
#define VDAC2_CR1         (* (reg8 *) VDAC2_viDAC8__CR1 )
#define VDAC2_Data        (* (reg8 *) VDAC2_viDAC8__D )
#define VDAC2_Data_PTR    (  (reg8 *) VDAC2_viDAC8__D )
#define VDAC2_Strobe      (* (reg8 *) VDAC2_viDAC8__STROBE )
#define VDAC2_SW0         (* (reg8 *) VDAC2_viDAC8__SW0 )
#define VDAC2_SW2         (* (reg8 *) VDAC2_viDAC8__SW2 )
#define VDAC2_SW3         (* (reg8 *) VDAC2_viDAC8__SW3 )
#define VDAC2_SW4         (* (reg8 *) VDAC2_viDAC8__SW4 )
#define VDAC2_TR          (* (reg8 *) VDAC2_viDAC8__TR )
/* Power manager */
#define VDAC2_PWRMGR      (* (reg8 *) VDAC2_viDAC8__PM_ACT_CFG )
  /* Standby Power manager */
#define VDAC2_STBY_PWRMGR (* (reg8 *) VDAC2_viDAC8__PM_STBY_CFG )


/***************************************
*         Register Constants       
***************************************/

/* CR0 vDac Control Register 0 definitions */

/* Bit Field  DAC_HS_MODE                  */
#define VDAC2_HS_MASK        0x02u
#define VDAC2_HS_LOWPOWER    0x00u
#define VDAC2_HS_HIGHSPEED   0x02u

/* Bit Field  DAC_MODE                  */
#define VDAC2_MODE_MASK      0x10u
#define VDAC2_MODE_V         0x00u
#define VDAC2_MODE_I         0x10u

/* Bit Field  DAC_RANGE                  */
#define VDAC2_RANGE_MASK     0x0Cu
#define VDAC2_RANGE_0        0x00u
#define VDAC2_RANGE_1        0x04u

/* CR1 iDac Control Register 1 definitions */

/* Bit Field  DAC_MX_DATA                  */
#define VDAC2_SRC_MASK       0x20u
#define VDAC2_SRC_REG        0x00u
#define VDAC2_SRC_UDB        0x20u

/* This bit enable reset from UDB array      */
#define VDAC2_RESET_MASK     0x10u
#define VDAC2_RESET_ENABLE   0x10u
#define VDAC2_RESET_DISABLE  0x00u

/* This bit enables data from DAC bus      */
#define VDAC2_DACBUS_MASK     0x20u
#define VDAC2_DACBUS_ENABLE   0x20u
#define VDAC2_DACBUS_DISABLE  0x00u

/* DAC STROBE Strobe Control Register definitions */

/* Bit Field  DAC_MX_STROBE                  */
#define VDAC2_STRB_MASK     0x08u
#define VDAC2_STRB_EN       0x08u
#define VDAC2_STRB_DIS      0x00u

/* PM_ACT_CFG (Active Power Mode CFG Register)     */ 
#define VDAC2_ACT_PWR_EN   VDAC2_viDAC8__PM_ACT_MSK
  /* Standby Power enable mask */
#define VDAC2_STBY_PWR_EN  VDAC2_viDAC8__PM_STBY_MSK


/*******************************************************************************
*              Trim    
* Note - VDAC trim values are stored in the "Customer Table" area in * Row 1 of
*the Hidden Flash.  There are 8 bytes of trim data for each VDAC block.
* The values are:
*       I Gain offset, min range, Sourcing
*       I Gain offset, min range, Sinking
*       I Gain offset, med range, Sourcing
*       I Gain offset, med range, Sinking
*       I Gain offset, max range, Sourcing
*       I Gain offset, max range, Sinking
*       V Gain offset, 1V range
*       V Gain offset, 4V range
*
* The data set for the 4 VDACs are arranged using a left side/right side
* approach:
*   Left 0, Left 1, Right 0, Right 1.
* When mapped to the VDAC0 thru VDAC3 as:
*   VDAC 0, VDAC 2, VDAC 1, VDAC 3
*******************************************************************************/
#define VDAC2_TRIM_M7_1V_RNG_OFFSET  0x06u
#define VDAC2_TRIM_M8_4V_RNG_OFFSET  0x07u
/*Constatnt to set DAC in current mode and turnoff output */
#define VDAC2_CUR_MODE_OUT_OFF       0x1Eu 
#define VDAC2_DAC_TRIM_BASE          (VDAC2_viDAC8__TRIM__M1)

#endif /* CY_VDAC8_VDAC2_H  */


/* [] END OF FILE */


