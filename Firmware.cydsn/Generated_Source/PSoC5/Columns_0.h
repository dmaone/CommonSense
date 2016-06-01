/*******************************************************************************
* File Name: Columns_0.h  
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

#if !defined(CY_PINS_Columns_0_H) /* Pins Columns_0_H */
#define CY_PINS_Columns_0_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Columns_0_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Columns_0__PORT == 15 && ((Columns_0__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    Columns_0_Write(uint8 value);
void    Columns_0_SetDriveMode(uint8 mode);
uint8   Columns_0_ReadDataReg(void);
uint8   Columns_0_Read(void);
void    Columns_0_SetInterruptMode(uint16 position, uint16 mode);
uint8   Columns_0_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the Columns_0_SetDriveMode() function.
     *  @{
     */
        #define Columns_0_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define Columns_0_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define Columns_0_DM_RES_UP          PIN_DM_RES_UP
        #define Columns_0_DM_RES_DWN         PIN_DM_RES_DWN
        #define Columns_0_DM_OD_LO           PIN_DM_OD_LO
        #define Columns_0_DM_OD_HI           PIN_DM_OD_HI
        #define Columns_0_DM_STRONG          PIN_DM_STRONG
        #define Columns_0_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define Columns_0_MASK               Columns_0__MASK
#define Columns_0_SHIFT              Columns_0__SHIFT
#define Columns_0_WIDTH              8u

/* Interrupt constants */
#if defined(Columns_0__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in Columns_0_SetInterruptMode() function.
     *  @{
     */
        #define Columns_0_INTR_NONE      (uint16)(0x0000u)
        #define Columns_0_INTR_RISING    (uint16)(0x0001u)
        #define Columns_0_INTR_FALLING   (uint16)(0x0002u)
        #define Columns_0_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define Columns_0_INTR_MASK      (0x01u) 
#endif /* (Columns_0__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Columns_0_PS                     (* (reg8 *) Columns_0__PS)
/* Data Register */
#define Columns_0_DR                     (* (reg8 *) Columns_0__DR)
/* Port Number */
#define Columns_0_PRT_NUM                (* (reg8 *) Columns_0__PRT) 
/* Connect to Analog Globals */                                                  
#define Columns_0_AG                     (* (reg8 *) Columns_0__AG)                       
/* Analog MUX bux enable */
#define Columns_0_AMUX                   (* (reg8 *) Columns_0__AMUX) 
/* Bidirectional Enable */                                                        
#define Columns_0_BIE                    (* (reg8 *) Columns_0__BIE)
/* Bit-mask for Aliased Register Access */
#define Columns_0_BIT_MASK               (* (reg8 *) Columns_0__BIT_MASK)
/* Bypass Enable */
#define Columns_0_BYP                    (* (reg8 *) Columns_0__BYP)
/* Port wide control signals */                                                   
#define Columns_0_CTL                    (* (reg8 *) Columns_0__CTL)
/* Drive Modes */
#define Columns_0_DM0                    (* (reg8 *) Columns_0__DM0) 
#define Columns_0_DM1                    (* (reg8 *) Columns_0__DM1)
#define Columns_0_DM2                    (* (reg8 *) Columns_0__DM2) 
/* Input Buffer Disable Override */
#define Columns_0_INP_DIS                (* (reg8 *) Columns_0__INP_DIS)
/* LCD Common or Segment Drive */
#define Columns_0_LCD_COM_SEG            (* (reg8 *) Columns_0__LCD_COM_SEG)
/* Enable Segment LCD */
#define Columns_0_LCD_EN                 (* (reg8 *) Columns_0__LCD_EN)
/* Slew Rate Control */
#define Columns_0_SLW                    (* (reg8 *) Columns_0__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Columns_0_PRTDSI__CAPS_SEL       (* (reg8 *) Columns_0__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Columns_0_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Columns_0__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Columns_0_PRTDSI__OE_SEL0        (* (reg8 *) Columns_0__PRTDSI__OE_SEL0) 
#define Columns_0_PRTDSI__OE_SEL1        (* (reg8 *) Columns_0__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Columns_0_PRTDSI__OUT_SEL0       (* (reg8 *) Columns_0__PRTDSI__OUT_SEL0) 
#define Columns_0_PRTDSI__OUT_SEL1       (* (reg8 *) Columns_0__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Columns_0_PRTDSI__SYNC_OUT       (* (reg8 *) Columns_0__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(Columns_0__SIO_CFG)
    #define Columns_0_SIO_HYST_EN        (* (reg8 *) Columns_0__SIO_HYST_EN)
    #define Columns_0_SIO_REG_HIFREQ     (* (reg8 *) Columns_0__SIO_REG_HIFREQ)
    #define Columns_0_SIO_CFG            (* (reg8 *) Columns_0__SIO_CFG)
    #define Columns_0_SIO_DIFF           (* (reg8 *) Columns_0__SIO_DIFF)
#endif /* (Columns_0__SIO_CFG) */

/* Interrupt Registers */
#if defined(Columns_0__INTSTAT)
    #define Columns_0_INTSTAT            (* (reg8 *) Columns_0__INTSTAT)
    #define Columns_0_SNAP               (* (reg8 *) Columns_0__SNAP)
    
	#define Columns_0_0_INTTYPE_REG 		(* (reg8 *) Columns_0__0__INTTYPE)
	#define Columns_0_1_INTTYPE_REG 		(* (reg8 *) Columns_0__1__INTTYPE)
	#define Columns_0_2_INTTYPE_REG 		(* (reg8 *) Columns_0__2__INTTYPE)
	#define Columns_0_3_INTTYPE_REG 		(* (reg8 *) Columns_0__3__INTTYPE)
	#define Columns_0_4_INTTYPE_REG 		(* (reg8 *) Columns_0__4__INTTYPE)
	#define Columns_0_5_INTTYPE_REG 		(* (reg8 *) Columns_0__5__INTTYPE)
	#define Columns_0_6_INTTYPE_REG 		(* (reg8 *) Columns_0__6__INTTYPE)
	#define Columns_0_7_INTTYPE_REG 		(* (reg8 *) Columns_0__7__INTTYPE)
#endif /* (Columns_0__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Columns_0_H */


/* [] END OF FILE */
