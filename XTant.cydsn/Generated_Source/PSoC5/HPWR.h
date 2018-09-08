/*******************************************************************************
* File Name: HPWR.h  
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

#if !defined(CY_PINS_HPWR_H) /* Pins HPWR_H */
#define CY_PINS_HPWR_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "HPWR_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 HPWR__PORT == 15 && ((HPWR__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    HPWR_Write(uint8 value);
void    HPWR_SetDriveMode(uint8 mode);
uint8   HPWR_ReadDataReg(void);
uint8   HPWR_Read(void);
void    HPWR_SetInterruptMode(uint16 position, uint16 mode);
uint8   HPWR_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the HPWR_SetDriveMode() function.
     *  @{
     */
        #define HPWR_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define HPWR_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define HPWR_DM_RES_UP          PIN_DM_RES_UP
        #define HPWR_DM_RES_DWN         PIN_DM_RES_DWN
        #define HPWR_DM_OD_LO           PIN_DM_OD_LO
        #define HPWR_DM_OD_HI           PIN_DM_OD_HI
        #define HPWR_DM_STRONG          PIN_DM_STRONG
        #define HPWR_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define HPWR_MASK               HPWR__MASK
#define HPWR_SHIFT              HPWR__SHIFT
#define HPWR_WIDTH              1u

/* Interrupt constants */
#if defined(HPWR__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in HPWR_SetInterruptMode() function.
     *  @{
     */
        #define HPWR_INTR_NONE      (uint16)(0x0000u)
        #define HPWR_INTR_RISING    (uint16)(0x0001u)
        #define HPWR_INTR_FALLING   (uint16)(0x0002u)
        #define HPWR_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define HPWR_INTR_MASK      (0x01u) 
#endif /* (HPWR__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define HPWR_PS                     (* (reg8 *) HPWR__PS)
/* Data Register */
#define HPWR_DR                     (* (reg8 *) HPWR__DR)
/* Port Number */
#define HPWR_PRT_NUM                (* (reg8 *) HPWR__PRT) 
/* Connect to Analog Globals */                                                  
#define HPWR_AG                     (* (reg8 *) HPWR__AG)                       
/* Analog MUX bux enable */
#define HPWR_AMUX                   (* (reg8 *) HPWR__AMUX) 
/* Bidirectional Enable */                                                        
#define HPWR_BIE                    (* (reg8 *) HPWR__BIE)
/* Bit-mask for Aliased Register Access */
#define HPWR_BIT_MASK               (* (reg8 *) HPWR__BIT_MASK)
/* Bypass Enable */
#define HPWR_BYP                    (* (reg8 *) HPWR__BYP)
/* Port wide control signals */                                                   
#define HPWR_CTL                    (* (reg8 *) HPWR__CTL)
/* Drive Modes */
#define HPWR_DM0                    (* (reg8 *) HPWR__DM0) 
#define HPWR_DM1                    (* (reg8 *) HPWR__DM1)
#define HPWR_DM2                    (* (reg8 *) HPWR__DM2) 
/* Input Buffer Disable Override */
#define HPWR_INP_DIS                (* (reg8 *) HPWR__INP_DIS)
/* LCD Common or Segment Drive */
#define HPWR_LCD_COM_SEG            (* (reg8 *) HPWR__LCD_COM_SEG)
/* Enable Segment LCD */
#define HPWR_LCD_EN                 (* (reg8 *) HPWR__LCD_EN)
/* Slew Rate Control */
#define HPWR_SLW                    (* (reg8 *) HPWR__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define HPWR_PRTDSI__CAPS_SEL       (* (reg8 *) HPWR__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define HPWR_PRTDSI__DBL_SYNC_IN    (* (reg8 *) HPWR__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define HPWR_PRTDSI__OE_SEL0        (* (reg8 *) HPWR__PRTDSI__OE_SEL0) 
#define HPWR_PRTDSI__OE_SEL1        (* (reg8 *) HPWR__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define HPWR_PRTDSI__OUT_SEL0       (* (reg8 *) HPWR__PRTDSI__OUT_SEL0) 
#define HPWR_PRTDSI__OUT_SEL1       (* (reg8 *) HPWR__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define HPWR_PRTDSI__SYNC_OUT       (* (reg8 *) HPWR__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(HPWR__SIO_CFG)
    #define HPWR_SIO_HYST_EN        (* (reg8 *) HPWR__SIO_HYST_EN)
    #define HPWR_SIO_REG_HIFREQ     (* (reg8 *) HPWR__SIO_REG_HIFREQ)
    #define HPWR_SIO_CFG            (* (reg8 *) HPWR__SIO_CFG)
    #define HPWR_SIO_DIFF           (* (reg8 *) HPWR__SIO_DIFF)
#endif /* (HPWR__SIO_CFG) */

/* Interrupt Registers */
#if defined(HPWR__INTSTAT)
    #define HPWR_INTSTAT            (* (reg8 *) HPWR__INTSTAT)
    #define HPWR_SNAP               (* (reg8 *) HPWR__SNAP)
    
	#define HPWR_0_INTTYPE_REG 		(* (reg8 *) HPWR__0__INTTYPE)
#endif /* (HPWR__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_HPWR_H */


/* [] END OF FILE */
