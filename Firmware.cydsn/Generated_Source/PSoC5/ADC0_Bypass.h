/*******************************************************************************
* File Name: ADC0_Bypass.h  
* Version 1.90
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_ADC0_Bypass_H) /* Pins ADC0_Bypass_H */
#define CY_PINS_ADC0_Bypass_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ADC0_Bypass_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v1_90 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ADC0_Bypass__PORT == 15 && ((ADC0_Bypass__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    ADC0_Bypass_Write(uint8 value) ;
void    ADC0_Bypass_SetDriveMode(uint8 mode) ;
uint8   ADC0_Bypass_ReadDataReg(void) ;
uint8   ADC0_Bypass_Read(void) ;
uint8   ADC0_Bypass_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define ADC0_Bypass_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define ADC0_Bypass_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define ADC0_Bypass_DM_RES_UP          PIN_DM_RES_UP
#define ADC0_Bypass_DM_RES_DWN         PIN_DM_RES_DWN
#define ADC0_Bypass_DM_OD_LO           PIN_DM_OD_LO
#define ADC0_Bypass_DM_OD_HI           PIN_DM_OD_HI
#define ADC0_Bypass_DM_STRONG          PIN_DM_STRONG
#define ADC0_Bypass_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define ADC0_Bypass_MASK               ADC0_Bypass__MASK
#define ADC0_Bypass_SHIFT              ADC0_Bypass__SHIFT
#define ADC0_Bypass_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ADC0_Bypass_PS                     (* (reg8 *) ADC0_Bypass__PS)
/* Data Register */
#define ADC0_Bypass_DR                     (* (reg8 *) ADC0_Bypass__DR)
/* Port Number */
#define ADC0_Bypass_PRT_NUM                (* (reg8 *) ADC0_Bypass__PRT) 
/* Connect to Analog Globals */                                                  
#define ADC0_Bypass_AG                     (* (reg8 *) ADC0_Bypass__AG)                       
/* Analog MUX bux enable */
#define ADC0_Bypass_AMUX                   (* (reg8 *) ADC0_Bypass__AMUX) 
/* Bidirectional Enable */                                                        
#define ADC0_Bypass_BIE                    (* (reg8 *) ADC0_Bypass__BIE)
/* Bit-mask for Aliased Register Access */
#define ADC0_Bypass_BIT_MASK               (* (reg8 *) ADC0_Bypass__BIT_MASK)
/* Bypass Enable */
#define ADC0_Bypass_BYP                    (* (reg8 *) ADC0_Bypass__BYP)
/* Port wide control signals */                                                   
#define ADC0_Bypass_CTL                    (* (reg8 *) ADC0_Bypass__CTL)
/* Drive Modes */
#define ADC0_Bypass_DM0                    (* (reg8 *) ADC0_Bypass__DM0) 
#define ADC0_Bypass_DM1                    (* (reg8 *) ADC0_Bypass__DM1)
#define ADC0_Bypass_DM2                    (* (reg8 *) ADC0_Bypass__DM2) 
/* Input Buffer Disable Override */
#define ADC0_Bypass_INP_DIS                (* (reg8 *) ADC0_Bypass__INP_DIS)
/* LCD Common or Segment Drive */
#define ADC0_Bypass_LCD_COM_SEG            (* (reg8 *) ADC0_Bypass__LCD_COM_SEG)
/* Enable Segment LCD */
#define ADC0_Bypass_LCD_EN                 (* (reg8 *) ADC0_Bypass__LCD_EN)
/* Slew Rate Control */
#define ADC0_Bypass_SLW                    (* (reg8 *) ADC0_Bypass__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ADC0_Bypass_PRTDSI__CAPS_SEL       (* (reg8 *) ADC0_Bypass__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ADC0_Bypass_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ADC0_Bypass__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ADC0_Bypass_PRTDSI__OE_SEL0        (* (reg8 *) ADC0_Bypass__PRTDSI__OE_SEL0) 
#define ADC0_Bypass_PRTDSI__OE_SEL1        (* (reg8 *) ADC0_Bypass__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ADC0_Bypass_PRTDSI__OUT_SEL0       (* (reg8 *) ADC0_Bypass__PRTDSI__OUT_SEL0) 
#define ADC0_Bypass_PRTDSI__OUT_SEL1       (* (reg8 *) ADC0_Bypass__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ADC0_Bypass_PRTDSI__SYNC_OUT       (* (reg8 *) ADC0_Bypass__PRTDSI__SYNC_OUT) 


#if defined(ADC0_Bypass__INTSTAT)  /* Interrupt Registers */

    #define ADC0_Bypass_INTSTAT                (* (reg8 *) ADC0_Bypass__INTSTAT)
    #define ADC0_Bypass_SNAP                   (* (reg8 *) ADC0_Bypass__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ADC0_Bypass_H */


/* [] END OF FILE */
