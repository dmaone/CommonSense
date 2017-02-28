/*******************************************************************************
* File Name: ADC1_ExtVref.h  
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

#if !defined(CY_PINS_ADC1_ExtVref_H) /* Pins ADC1_ExtVref_H */
#define CY_PINS_ADC1_ExtVref_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ADC1_ExtVref_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v1_90 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ADC1_ExtVref__PORT == 15 && ((ADC1_ExtVref__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    ADC1_ExtVref_Write(uint8 value) ;
void    ADC1_ExtVref_SetDriveMode(uint8 mode) ;
uint8   ADC1_ExtVref_ReadDataReg(void) ;
uint8   ADC1_ExtVref_Read(void) ;
uint8   ADC1_ExtVref_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define ADC1_ExtVref_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define ADC1_ExtVref_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define ADC1_ExtVref_DM_RES_UP          PIN_DM_RES_UP
#define ADC1_ExtVref_DM_RES_DWN         PIN_DM_RES_DWN
#define ADC1_ExtVref_DM_OD_LO           PIN_DM_OD_LO
#define ADC1_ExtVref_DM_OD_HI           PIN_DM_OD_HI
#define ADC1_ExtVref_DM_STRONG          PIN_DM_STRONG
#define ADC1_ExtVref_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define ADC1_ExtVref_MASK               ADC1_ExtVref__MASK
#define ADC1_ExtVref_SHIFT              ADC1_ExtVref__SHIFT
#define ADC1_ExtVref_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ADC1_ExtVref_PS                     (* (reg8 *) ADC1_ExtVref__PS)
/* Data Register */
#define ADC1_ExtVref_DR                     (* (reg8 *) ADC1_ExtVref__DR)
/* Port Number */
#define ADC1_ExtVref_PRT_NUM                (* (reg8 *) ADC1_ExtVref__PRT) 
/* Connect to Analog Globals */                                                  
#define ADC1_ExtVref_AG                     (* (reg8 *) ADC1_ExtVref__AG)                       
/* Analog MUX bux enable */
#define ADC1_ExtVref_AMUX                   (* (reg8 *) ADC1_ExtVref__AMUX) 
/* Bidirectional Enable */                                                        
#define ADC1_ExtVref_BIE                    (* (reg8 *) ADC1_ExtVref__BIE)
/* Bit-mask for Aliased Register Access */
#define ADC1_ExtVref_BIT_MASK               (* (reg8 *) ADC1_ExtVref__BIT_MASK)
/* Bypass Enable */
#define ADC1_ExtVref_BYP                    (* (reg8 *) ADC1_ExtVref__BYP)
/* Port wide control signals */                                                   
#define ADC1_ExtVref_CTL                    (* (reg8 *) ADC1_ExtVref__CTL)
/* Drive Modes */
#define ADC1_ExtVref_DM0                    (* (reg8 *) ADC1_ExtVref__DM0) 
#define ADC1_ExtVref_DM1                    (* (reg8 *) ADC1_ExtVref__DM1)
#define ADC1_ExtVref_DM2                    (* (reg8 *) ADC1_ExtVref__DM2) 
/* Input Buffer Disable Override */
#define ADC1_ExtVref_INP_DIS                (* (reg8 *) ADC1_ExtVref__INP_DIS)
/* LCD Common or Segment Drive */
#define ADC1_ExtVref_LCD_COM_SEG            (* (reg8 *) ADC1_ExtVref__LCD_COM_SEG)
/* Enable Segment LCD */
#define ADC1_ExtVref_LCD_EN                 (* (reg8 *) ADC1_ExtVref__LCD_EN)
/* Slew Rate Control */
#define ADC1_ExtVref_SLW                    (* (reg8 *) ADC1_ExtVref__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ADC1_ExtVref_PRTDSI__CAPS_SEL       (* (reg8 *) ADC1_ExtVref__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ADC1_ExtVref_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ADC1_ExtVref__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ADC1_ExtVref_PRTDSI__OE_SEL0        (* (reg8 *) ADC1_ExtVref__PRTDSI__OE_SEL0) 
#define ADC1_ExtVref_PRTDSI__OE_SEL1        (* (reg8 *) ADC1_ExtVref__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ADC1_ExtVref_PRTDSI__OUT_SEL0       (* (reg8 *) ADC1_ExtVref__PRTDSI__OUT_SEL0) 
#define ADC1_ExtVref_PRTDSI__OUT_SEL1       (* (reg8 *) ADC1_ExtVref__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ADC1_ExtVref_PRTDSI__SYNC_OUT       (* (reg8 *) ADC1_ExtVref__PRTDSI__SYNC_OUT) 


#if defined(ADC1_ExtVref__INTSTAT)  /* Interrupt Registers */

    #define ADC1_ExtVref_INTSTAT                (* (reg8 *) ADC1_ExtVref__INTSTAT)
    #define ADC1_ExtVref_SNAP                   (* (reg8 *) ADC1_ExtVref__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ADC1_ExtVref_H */


/* [] END OF FILE */
