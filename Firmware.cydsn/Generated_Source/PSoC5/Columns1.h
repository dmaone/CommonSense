/*******************************************************************************
* File Name: Columns1.h  
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

#if !defined(CY_PINS_Columns1_H) /* Pins Columns1_H */
#define CY_PINS_Columns1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Columns1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Columns1__PORT == 15 && ((Columns1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Columns1_Write(uint8 value);
void    Columns1_SetDriveMode(uint8 mode);
uint8   Columns1_ReadDataReg(void);
uint8   Columns1_Read(void);
void    Columns1_SetInterruptMode(uint16 position, uint16 mode);
uint8   Columns1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Columns1_SetDriveMode() function.
     *  @{
     */
        #define Columns1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Columns1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Columns1_DM_RES_UP          PIN_DM_RES_UP
        #define Columns1_DM_RES_DWN         PIN_DM_RES_DWN
        #define Columns1_DM_OD_LO           PIN_DM_OD_LO
        #define Columns1_DM_OD_HI           PIN_DM_OD_HI
        #define Columns1_DM_STRONG          PIN_DM_STRONG
        #define Columns1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Columns1_MASK               Columns1__MASK
#define Columns1_SHIFT              Columns1__SHIFT
#define Columns1_WIDTH              8u

/* Interrupt constants */
#if defined(Columns1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Columns1_SetInterruptMode() function.
     *  @{
     */
        #define Columns1_INTR_NONE      (uint16)(0x0000u)
        #define Columns1_INTR_RISING    (uint16)(0x0001u)
        #define Columns1_INTR_FALLING   (uint16)(0x0002u)
        #define Columns1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Columns1_INTR_MASK      (0x01u) 
#endif /* (Columns1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Columns1_PS                     (* (reg8 *) Columns1__PS)
/* Data Register */
#define Columns1_DR                     (* (reg8 *) Columns1__DR)
/* Port Number */
#define Columns1_PRT_NUM                (* (reg8 *) Columns1__PRT) 
/* Connect to Analog Globals */                                                  
#define Columns1_AG                     (* (reg8 *) Columns1__AG)                       
/* Analog MUX bux enable */
#define Columns1_AMUX                   (* (reg8 *) Columns1__AMUX) 
/* Bidirectional Enable */                                                        
#define Columns1_BIE                    (* (reg8 *) Columns1__BIE)
/* Bit-mask for Aliased Register Access */
#define Columns1_BIT_MASK               (* (reg8 *) Columns1__BIT_MASK)
/* Bypass Enable */
#define Columns1_BYP                    (* (reg8 *) Columns1__BYP)
/* Port wide control signals */                                                   
#define Columns1_CTL                    (* (reg8 *) Columns1__CTL)
/* Drive Modes */
#define Columns1_DM0                    (* (reg8 *) Columns1__DM0) 
#define Columns1_DM1                    (* (reg8 *) Columns1__DM1)
#define Columns1_DM2                    (* (reg8 *) Columns1__DM2) 
/* Input Buffer Disable Override */
#define Columns1_INP_DIS                (* (reg8 *) Columns1__INP_DIS)
/* LCD Common or Segment Drive */
#define Columns1_LCD_COM_SEG            (* (reg8 *) Columns1__LCD_COM_SEG)
/* Enable Segment LCD */
#define Columns1_LCD_EN                 (* (reg8 *) Columns1__LCD_EN)
/* Slew Rate Control */
#define Columns1_SLW                    (* (reg8 *) Columns1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Columns1_PRTDSI__CAPS_SEL       (* (reg8 *) Columns1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Columns1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Columns1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Columns1_PRTDSI__OE_SEL0        (* (reg8 *) Columns1__PRTDSI__OE_SEL0) 
#define Columns1_PRTDSI__OE_SEL1        (* (reg8 *) Columns1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Columns1_PRTDSI__OUT_SEL0       (* (reg8 *) Columns1__PRTDSI__OUT_SEL0) 
#define Columns1_PRTDSI__OUT_SEL1       (* (reg8 *) Columns1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Columns1_PRTDSI__SYNC_OUT       (* (reg8 *) Columns1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Columns1__SIO_CFG)
    #define Columns1_SIO_HYST_EN        (* (reg8 *) Columns1__SIO_HYST_EN)
    #define Columns1_SIO_REG_HIFREQ     (* (reg8 *) Columns1__SIO_REG_HIFREQ)
    #define Columns1_SIO_CFG            (* (reg8 *) Columns1__SIO_CFG)
    #define Columns1_SIO_DIFF           (* (reg8 *) Columns1__SIO_DIFF)
#endif /* (Columns1__SIO_CFG) */

/* Interrupt Registers */
#if defined(Columns1__INTSTAT)
    #define Columns1_INTSTAT            (* (reg8 *) Columns1__INTSTAT)
    #define Columns1_SNAP               (* (reg8 *) Columns1__SNAP)
    
	#define Columns1_0_INTTYPE_REG 		(* (reg8 *) Columns1__0__INTTYPE)
	#define Columns1_1_INTTYPE_REG 		(* (reg8 *) Columns1__1__INTTYPE)
	#define Columns1_2_INTTYPE_REG 		(* (reg8 *) Columns1__2__INTTYPE)
	#define Columns1_3_INTTYPE_REG 		(* (reg8 *) Columns1__3__INTTYPE)
	#define Columns1_4_INTTYPE_REG 		(* (reg8 *) Columns1__4__INTTYPE)
	#define Columns1_5_INTTYPE_REG 		(* (reg8 *) Columns1__5__INTTYPE)
	#define Columns1_6_INTTYPE_REG 		(* (reg8 *) Columns1__6__INTTYPE)
	#define Columns1_7_INTTYPE_REG 		(* (reg8 *) Columns1__7__INTTYPE)
#endif /* (Columns1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Columns1_H */


/* [] END OF FILE */
