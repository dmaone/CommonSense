/*******************************************************************************
* File Name: ADC0_ExtVref.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_ADC0_ExtVref_H) /* Pins ADC0_ExtVref_H */
#define CY_PINS_ADC0_ExtVref_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ADC0_ExtVref_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ADC0_ExtVref__PORT == 15 && ((ADC0_ExtVref__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ADC0_ExtVref_Write(uint8 value);
void    ADC0_ExtVref_SetDriveMode(uint8 mode);
uint8   ADC0_ExtVref_ReadDataReg(void);
uint8   ADC0_ExtVref_Read(void);
void    ADC0_ExtVref_SetInterruptMode(uint16 position, uint16 mode);
uint8   ADC0_ExtVref_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ADC0_ExtVref_SetDriveMode() function.
     *  @{
     */
        #define ADC0_ExtVref_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ADC0_ExtVref_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ADC0_ExtVref_DM_RES_UP          PIN_DM_RES_UP
        #define ADC0_ExtVref_DM_RES_DWN         PIN_DM_RES_DWN
        #define ADC0_ExtVref_DM_OD_LO           PIN_DM_OD_LO
        #define ADC0_ExtVref_DM_OD_HI           PIN_DM_OD_HI
        #define ADC0_ExtVref_DM_STRONG          PIN_DM_STRONG
        #define ADC0_ExtVref_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ADC0_ExtVref_MASK               ADC0_ExtVref__MASK
#define ADC0_ExtVref_SHIFT              ADC0_ExtVref__SHIFT
#define ADC0_ExtVref_WIDTH              1u

/* Interrupt constants */
#if defined(ADC0_ExtVref__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ADC0_ExtVref_SetInterruptMode() function.
     *  @{
     */
        #define ADC0_ExtVref_INTR_NONE      (uint16)(0x0000u)
        #define ADC0_ExtVref_INTR_RISING    (uint16)(0x0001u)
        #define ADC0_ExtVref_INTR_FALLING   (uint16)(0x0002u)
        #define ADC0_ExtVref_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ADC0_ExtVref_INTR_MASK      (0x01u) 
#endif /* (ADC0_ExtVref__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ADC0_ExtVref_PS                     (* (reg8 *) ADC0_ExtVref__PS)
/* Data Register */
#define ADC0_ExtVref_DR                     (* (reg8 *) ADC0_ExtVref__DR)
/* Port Number */
#define ADC0_ExtVref_PRT_NUM                (* (reg8 *) ADC0_ExtVref__PRT) 
/* Connect to Analog Globals */                                                  
#define ADC0_ExtVref_AG                     (* (reg8 *) ADC0_ExtVref__AG)                       
/* Analog MUX bux enable */
#define ADC0_ExtVref_AMUX                   (* (reg8 *) ADC0_ExtVref__AMUX) 
/* Bidirectional Enable */                                                        
#define ADC0_ExtVref_BIE                    (* (reg8 *) ADC0_ExtVref__BIE)
/* Bit-mask for Aliased Register Access */
#define ADC0_ExtVref_BIT_MASK               (* (reg8 *) ADC0_ExtVref__BIT_MASK)
/* Bypass Enable */
#define ADC0_ExtVref_BYP                    (* (reg8 *) ADC0_ExtVref__BYP)
/* Port wide control signals */                                                   
#define ADC0_ExtVref_CTL                    (* (reg8 *) ADC0_ExtVref__CTL)
/* Drive Modes */
#define ADC0_ExtVref_DM0                    (* (reg8 *) ADC0_ExtVref__DM0) 
#define ADC0_ExtVref_DM1                    (* (reg8 *) ADC0_ExtVref__DM1)
#define ADC0_ExtVref_DM2                    (* (reg8 *) ADC0_ExtVref__DM2) 
/* Input Buffer Disable Override */
#define ADC0_ExtVref_INP_DIS                (* (reg8 *) ADC0_ExtVref__INP_DIS)
/* LCD Common or Segment Drive */
#define ADC0_ExtVref_LCD_COM_SEG            (* (reg8 *) ADC0_ExtVref__LCD_COM_SEG)
/* Enable Segment LCD */
#define ADC0_ExtVref_LCD_EN                 (* (reg8 *) ADC0_ExtVref__LCD_EN)
/* Slew Rate Control */
#define ADC0_ExtVref_SLW                    (* (reg8 *) ADC0_ExtVref__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ADC0_ExtVref_PRTDSI__CAPS_SEL       (* (reg8 *) ADC0_ExtVref__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ADC0_ExtVref_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ADC0_ExtVref__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ADC0_ExtVref_PRTDSI__OE_SEL0        (* (reg8 *) ADC0_ExtVref__PRTDSI__OE_SEL0) 
#define ADC0_ExtVref_PRTDSI__OE_SEL1        (* (reg8 *) ADC0_ExtVref__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ADC0_ExtVref_PRTDSI__OUT_SEL0       (* (reg8 *) ADC0_ExtVref__PRTDSI__OUT_SEL0) 
#define ADC0_ExtVref_PRTDSI__OUT_SEL1       (* (reg8 *) ADC0_ExtVref__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ADC0_ExtVref_PRTDSI__SYNC_OUT       (* (reg8 *) ADC0_ExtVref__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ADC0_ExtVref__SIO_CFG)
    #define ADC0_ExtVref_SIO_HYST_EN        (* (reg8 *) ADC0_ExtVref__SIO_HYST_EN)
    #define ADC0_ExtVref_SIO_REG_HIFREQ     (* (reg8 *) ADC0_ExtVref__SIO_REG_HIFREQ)
    #define ADC0_ExtVref_SIO_CFG            (* (reg8 *) ADC0_ExtVref__SIO_CFG)
    #define ADC0_ExtVref_SIO_DIFF           (* (reg8 *) ADC0_ExtVref__SIO_DIFF)
#endif /* (ADC0_ExtVref__SIO_CFG) */

/* Interrupt Registers */
#if defined(ADC0_ExtVref__INTSTAT)
    #define ADC0_ExtVref_INTSTAT            (* (reg8 *) ADC0_ExtVref__INTSTAT)
    #define ADC0_ExtVref_SNAP               (* (reg8 *) ADC0_ExtVref__SNAP)
    
	#define ADC0_ExtVref_0_INTTYPE_REG 		(* (reg8 *) ADC0_ExtVref__0__INTTYPE)
#endif /* (ADC0_ExtVref__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ADC0_ExtVref_H */


/* [] END OF FILE */
