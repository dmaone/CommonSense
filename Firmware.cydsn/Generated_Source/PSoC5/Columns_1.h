/*******************************************************************************
* File Name: Columns_1.h  
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

#if !defined(CY_PINS_Columns_1_H) /* Pins Columns_1_H */
#define CY_PINS_Columns_1_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Columns_1_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Columns_1__PORT == 15 && ((Columns_1__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Columns_1_Write(uint8 value);
void    Columns_1_SetDriveMode(uint8 mode);
uint8   Columns_1_ReadDataReg(void);
uint8   Columns_1_Read(void);
void    Columns_1_SetInterruptMode(uint16 position, uint16 mode);
uint8   Columns_1_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Columns_1_SetDriveMode() function.
     *  @{
     */
        #define Columns_1_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Columns_1_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Columns_1_DM_RES_UP          PIN_DM_RES_UP
        #define Columns_1_DM_RES_DWN         PIN_DM_RES_DWN
        #define Columns_1_DM_OD_LO           PIN_DM_OD_LO
        #define Columns_1_DM_OD_HI           PIN_DM_OD_HI
        #define Columns_1_DM_STRONG          PIN_DM_STRONG
        #define Columns_1_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Columns_1_MASK               Columns_1__MASK
#define Columns_1_SHIFT              Columns_1__SHIFT
#define Columns_1_WIDTH              8u

/* Interrupt constants */
#if defined(Columns_1__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Columns_1_SetInterruptMode() function.
     *  @{
     */
        #define Columns_1_INTR_NONE      (uint16)(0x0000u)
        #define Columns_1_INTR_RISING    (uint16)(0x0001u)
        #define Columns_1_INTR_FALLING   (uint16)(0x0002u)
        #define Columns_1_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Columns_1_INTR_MASK      (0x01u) 
#endif /* (Columns_1__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Columns_1_PS                     (* (reg8 *) Columns_1__PS)
/* Data Register */
#define Columns_1_DR                     (* (reg8 *) Columns_1__DR)
/* Port Number */
#define Columns_1_PRT_NUM                (* (reg8 *) Columns_1__PRT) 
/* Connect to Analog Globals */                                                  
#define Columns_1_AG                     (* (reg8 *) Columns_1__AG)                       
/* Analog MUX bux enable */
#define Columns_1_AMUX                   (* (reg8 *) Columns_1__AMUX) 
/* Bidirectional Enable */                                                        
#define Columns_1_BIE                    (* (reg8 *) Columns_1__BIE)
/* Bit-mask for Aliased Register Access */
#define Columns_1_BIT_MASK               (* (reg8 *) Columns_1__BIT_MASK)
/* Bypass Enable */
#define Columns_1_BYP                    (* (reg8 *) Columns_1__BYP)
/* Port wide control signals */                                                   
#define Columns_1_CTL                    (* (reg8 *) Columns_1__CTL)
/* Drive Modes */
#define Columns_1_DM0                    (* (reg8 *) Columns_1__DM0) 
#define Columns_1_DM1                    (* (reg8 *) Columns_1__DM1)
#define Columns_1_DM2                    (* (reg8 *) Columns_1__DM2) 
/* Input Buffer Disable Override */
#define Columns_1_INP_DIS                (* (reg8 *) Columns_1__INP_DIS)
/* LCD Common or Segment Drive */
#define Columns_1_LCD_COM_SEG            (* (reg8 *) Columns_1__LCD_COM_SEG)
/* Enable Segment LCD */
#define Columns_1_LCD_EN                 (* (reg8 *) Columns_1__LCD_EN)
/* Slew Rate Control */
#define Columns_1_SLW                    (* (reg8 *) Columns_1__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Columns_1_PRTDSI__CAPS_SEL       (* (reg8 *) Columns_1__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Columns_1_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Columns_1__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Columns_1_PRTDSI__OE_SEL0        (* (reg8 *) Columns_1__PRTDSI__OE_SEL0) 
#define Columns_1_PRTDSI__OE_SEL1        (* (reg8 *) Columns_1__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Columns_1_PRTDSI__OUT_SEL0       (* (reg8 *) Columns_1__PRTDSI__OUT_SEL0) 
#define Columns_1_PRTDSI__OUT_SEL1       (* (reg8 *) Columns_1__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Columns_1_PRTDSI__SYNC_OUT       (* (reg8 *) Columns_1__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Columns_1__SIO_CFG)
    #define Columns_1_SIO_HYST_EN        (* (reg8 *) Columns_1__SIO_HYST_EN)
    #define Columns_1_SIO_REG_HIFREQ     (* (reg8 *) Columns_1__SIO_REG_HIFREQ)
    #define Columns_1_SIO_CFG            (* (reg8 *) Columns_1__SIO_CFG)
    #define Columns_1_SIO_DIFF           (* (reg8 *) Columns_1__SIO_DIFF)
#endif /* (Columns_1__SIO_CFG) */

/* Interrupt Registers */
#if defined(Columns_1__INTSTAT)
    #define Columns_1_INTSTAT            (* (reg8 *) Columns_1__INTSTAT)
    #define Columns_1_SNAP               (* (reg8 *) Columns_1__SNAP)
    
	#define Columns_1_0_INTTYPE_REG 		(* (reg8 *) Columns_1__0__INTTYPE)
	#define Columns_1_1_INTTYPE_REG 		(* (reg8 *) Columns_1__1__INTTYPE)
	#define Columns_1_2_INTTYPE_REG 		(* (reg8 *) Columns_1__2__INTTYPE)
	#define Columns_1_3_INTTYPE_REG 		(* (reg8 *) Columns_1__3__INTTYPE)
	#define Columns_1_4_INTTYPE_REG 		(* (reg8 *) Columns_1__4__INTTYPE)
	#define Columns_1_5_INTTYPE_REG 		(* (reg8 *) Columns_1__5__INTTYPE)
	#define Columns_1_6_INTTYPE_REG 		(* (reg8 *) Columns_1__6__INTTYPE)
	#define Columns_1_7_INTTYPE_REG 		(* (reg8 *) Columns_1__7__INTTYPE)
#endif /* (Columns_1__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Columns_1_H */


/* [] END OF FILE */
