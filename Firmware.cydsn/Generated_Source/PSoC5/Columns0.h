/*******************************************************************************
* File Name: Columns0.h  
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

#if !defined(CY_PINS_Columns0_H) /* Pins Columns0_H */
#define CY_PINS_Columns0_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Columns0_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Columns0__PORT == 15 && ((Columns0__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Columns0_Write(uint8 value);
void    Columns0_SetDriveMode(uint8 mode);
uint8   Columns0_ReadDataReg(void);
uint8   Columns0_Read(void);
void    Columns0_SetInterruptMode(uint16 position, uint16 mode);
uint8   Columns0_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Columns0_SetDriveMode() function.
     *  @{
     */
        #define Columns0_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Columns0_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Columns0_DM_RES_UP          PIN_DM_RES_UP
        #define Columns0_DM_RES_DWN         PIN_DM_RES_DWN
        #define Columns0_DM_OD_LO           PIN_DM_OD_LO
        #define Columns0_DM_OD_HI           PIN_DM_OD_HI
        #define Columns0_DM_STRONG          PIN_DM_STRONG
        #define Columns0_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Columns0_MASK               Columns0__MASK
#define Columns0_SHIFT              Columns0__SHIFT
#define Columns0_WIDTH              8u

/* Interrupt constants */
#if defined(Columns0__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Columns0_SetInterruptMode() function.
     *  @{
     */
        #define Columns0_INTR_NONE      (uint16)(0x0000u)
        #define Columns0_INTR_RISING    (uint16)(0x0001u)
        #define Columns0_INTR_FALLING   (uint16)(0x0002u)
        #define Columns0_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Columns0_INTR_MASK      (0x01u) 
#endif /* (Columns0__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Columns0_PS                     (* (reg8 *) Columns0__PS)
/* Data Register */
#define Columns0_DR                     (* (reg8 *) Columns0__DR)
/* Port Number */
#define Columns0_PRT_NUM                (* (reg8 *) Columns0__PRT) 
/* Connect to Analog Globals */                                                  
#define Columns0_AG                     (* (reg8 *) Columns0__AG)                       
/* Analog MUX bux enable */
#define Columns0_AMUX                   (* (reg8 *) Columns0__AMUX) 
/* Bidirectional Enable */                                                        
#define Columns0_BIE                    (* (reg8 *) Columns0__BIE)
/* Bit-mask for Aliased Register Access */
#define Columns0_BIT_MASK               (* (reg8 *) Columns0__BIT_MASK)
/* Bypass Enable */
#define Columns0_BYP                    (* (reg8 *) Columns0__BYP)
/* Port wide control signals */                                                   
#define Columns0_CTL                    (* (reg8 *) Columns0__CTL)
/* Drive Modes */
#define Columns0_DM0                    (* (reg8 *) Columns0__DM0) 
#define Columns0_DM1                    (* (reg8 *) Columns0__DM1)
#define Columns0_DM2                    (* (reg8 *) Columns0__DM2) 
/* Input Buffer Disable Override */
#define Columns0_INP_DIS                (* (reg8 *) Columns0__INP_DIS)
/* LCD Common or Segment Drive */
#define Columns0_LCD_COM_SEG            (* (reg8 *) Columns0__LCD_COM_SEG)
/* Enable Segment LCD */
#define Columns0_LCD_EN                 (* (reg8 *) Columns0__LCD_EN)
/* Slew Rate Control */
#define Columns0_SLW                    (* (reg8 *) Columns0__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Columns0_PRTDSI__CAPS_SEL       (* (reg8 *) Columns0__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Columns0_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Columns0__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Columns0_PRTDSI__OE_SEL0        (* (reg8 *) Columns0__PRTDSI__OE_SEL0) 
#define Columns0_PRTDSI__OE_SEL1        (* (reg8 *) Columns0__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Columns0_PRTDSI__OUT_SEL0       (* (reg8 *) Columns0__PRTDSI__OUT_SEL0) 
#define Columns0_PRTDSI__OUT_SEL1       (* (reg8 *) Columns0__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Columns0_PRTDSI__SYNC_OUT       (* (reg8 *) Columns0__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Columns0__SIO_CFG)
    #define Columns0_SIO_HYST_EN        (* (reg8 *) Columns0__SIO_HYST_EN)
    #define Columns0_SIO_REG_HIFREQ     (* (reg8 *) Columns0__SIO_REG_HIFREQ)
    #define Columns0_SIO_CFG            (* (reg8 *) Columns0__SIO_CFG)
    #define Columns0_SIO_DIFF           (* (reg8 *) Columns0__SIO_DIFF)
#endif /* (Columns0__SIO_CFG) */

/* Interrupt Registers */
#if defined(Columns0__INTSTAT)
    #define Columns0_INTSTAT            (* (reg8 *) Columns0__INTSTAT)
    #define Columns0_SNAP               (* (reg8 *) Columns0__SNAP)
    
	#define Columns0_0_INTTYPE_REG 		(* (reg8 *) Columns0__0__INTTYPE)
	#define Columns0_1_INTTYPE_REG 		(* (reg8 *) Columns0__1__INTTYPE)
	#define Columns0_2_INTTYPE_REG 		(* (reg8 *) Columns0__2__INTTYPE)
	#define Columns0_3_INTTYPE_REG 		(* (reg8 *) Columns0__3__INTTYPE)
	#define Columns0_4_INTTYPE_REG 		(* (reg8 *) Columns0__4__INTTYPE)
	#define Columns0_5_INTTYPE_REG 		(* (reg8 *) Columns0__5__INTTYPE)
	#define Columns0_6_INTTYPE_REG 		(* (reg8 *) Columns0__6__INTTYPE)
	#define Columns0_7_INTTYPE_REG 		(* (reg8 *) Columns0__7__INTTYPE)
#endif /* (Columns0__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Columns0_H */


/* [] END OF FILE */
