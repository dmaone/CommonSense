/*******************************************************************************
* File Name: BootPin.h  
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

#if !defined(CY_PINS_BootPin_H) /* Pins BootPin_H */
#define CY_PINS_BootPin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "BootPin_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 BootPin__PORT == 15 && ((BootPin__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    BootPin_Write(uint8 value);
void    BootPin_SetDriveMode(uint8 mode);
uint8   BootPin_ReadDataReg(void);
uint8   BootPin_Read(void);
void    BootPin_SetInterruptMode(uint16 position, uint16 mode);
uint8   BootPin_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the BootPin_SetDriveMode() function.
     *  @{
     */
        #define BootPin_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define BootPin_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define BootPin_DM_RES_UP          PIN_DM_RES_UP
        #define BootPin_DM_RES_DWN         PIN_DM_RES_DWN
        #define BootPin_DM_OD_LO           PIN_DM_OD_LO
        #define BootPin_DM_OD_HI           PIN_DM_OD_HI
        #define BootPin_DM_STRONG          PIN_DM_STRONG
        #define BootPin_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define BootPin_MASK               BootPin__MASK
#define BootPin_SHIFT              BootPin__SHIFT
#define BootPin_WIDTH              1u

/* Interrupt constants */
#if defined(BootPin__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in BootPin_SetInterruptMode() function.
     *  @{
     */
        #define BootPin_INTR_NONE      (uint16)(0x0000u)
        #define BootPin_INTR_RISING    (uint16)(0x0001u)
        #define BootPin_INTR_FALLING   (uint16)(0x0002u)
        #define BootPin_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define BootPin_INTR_MASK      (0x01u) 
#endif /* (BootPin__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define BootPin_PS                     (* (reg8 *) BootPin__PS)
/* Data Register */
#define BootPin_DR                     (* (reg8 *) BootPin__DR)
/* Port Number */
#define BootPin_PRT_NUM                (* (reg8 *) BootPin__PRT) 
/* Connect to Analog Globals */                                                  
#define BootPin_AG                     (* (reg8 *) BootPin__AG)                       
/* Analog MUX bux enable */
#define BootPin_AMUX                   (* (reg8 *) BootPin__AMUX) 
/* Bidirectional Enable */                                                        
#define BootPin_BIE                    (* (reg8 *) BootPin__BIE)
/* Bit-mask for Aliased Register Access */
#define BootPin_BIT_MASK               (* (reg8 *) BootPin__BIT_MASK)
/* Bypass Enable */
#define BootPin_BYP                    (* (reg8 *) BootPin__BYP)
/* Port wide control signals */                                                   
#define BootPin_CTL                    (* (reg8 *) BootPin__CTL)
/* Drive Modes */
#define BootPin_DM0                    (* (reg8 *) BootPin__DM0) 
#define BootPin_DM1                    (* (reg8 *) BootPin__DM1)
#define BootPin_DM2                    (* (reg8 *) BootPin__DM2) 
/* Input Buffer Disable Override */
#define BootPin_INP_DIS                (* (reg8 *) BootPin__INP_DIS)
/* LCD Common or Segment Drive */
#define BootPin_LCD_COM_SEG            (* (reg8 *) BootPin__LCD_COM_SEG)
/* Enable Segment LCD */
#define BootPin_LCD_EN                 (* (reg8 *) BootPin__LCD_EN)
/* Slew Rate Control */
#define BootPin_SLW                    (* (reg8 *) BootPin__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define BootPin_PRTDSI__CAPS_SEL       (* (reg8 *) BootPin__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define BootPin_PRTDSI__DBL_SYNC_IN    (* (reg8 *) BootPin__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define BootPin_PRTDSI__OE_SEL0        (* (reg8 *) BootPin__PRTDSI__OE_SEL0) 
#define BootPin_PRTDSI__OE_SEL1        (* (reg8 *) BootPin__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define BootPin_PRTDSI__OUT_SEL0       (* (reg8 *) BootPin__PRTDSI__OUT_SEL0) 
#define BootPin_PRTDSI__OUT_SEL1       (* (reg8 *) BootPin__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define BootPin_PRTDSI__SYNC_OUT       (* (reg8 *) BootPin__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(BootPin__SIO_CFG)
    #define BootPin_SIO_HYST_EN        (* (reg8 *) BootPin__SIO_HYST_EN)
    #define BootPin_SIO_REG_HIFREQ     (* (reg8 *) BootPin__SIO_REG_HIFREQ)
    #define BootPin_SIO_CFG            (* (reg8 *) BootPin__SIO_CFG)
    #define BootPin_SIO_DIFF           (* (reg8 *) BootPin__SIO_DIFF)
#endif /* (BootPin__SIO_CFG) */

/* Interrupt Registers */
#if defined(BootPin__INTSTAT)
    #define BootPin_INTSTAT            (* (reg8 *) BootPin__INTSTAT)
    #define BootPin_SNAP               (* (reg8 *) BootPin__SNAP)
    
	#define BootPin_0_INTTYPE_REG 		(* (reg8 *) BootPin__0__INTTYPE)
#endif /* (BootPin__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_BootPin_H */


/* [] END OF FILE */
