/*******************************************************************************
* File Name: ADB_Data.h  
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

#if !defined(CY_PINS_ADB_Data_H) /* Pins ADB_Data_H */
#define CY_PINS_ADB_Data_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "ADB_Data_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 ADB_Data__PORT == 15 && ((ADB_Data__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    ADB_Data_Write(uint8 value);
void    ADB_Data_SetDriveMode(uint8 mode);
uint8   ADB_Data_ReadDataReg(void);
uint8   ADB_Data_Read(void);
void    ADB_Data_SetInterruptMode(uint16 position, uint16 mode);
uint8   ADB_Data_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the ADB_Data_SetDriveMode() function.
     *  @{
     */
        #define ADB_Data_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define ADB_Data_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define ADB_Data_DM_RES_UP          PIN_DM_RES_UP
        #define ADB_Data_DM_RES_DWN         PIN_DM_RES_DWN
        #define ADB_Data_DM_OD_LO           PIN_DM_OD_LO
        #define ADB_Data_DM_OD_HI           PIN_DM_OD_HI
        #define ADB_Data_DM_STRONG          PIN_DM_STRONG
        #define ADB_Data_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define ADB_Data_MASK               ADB_Data__MASK
#define ADB_Data_SHIFT              ADB_Data__SHIFT
#define ADB_Data_WIDTH              1u

/* Interrupt constants */
#if defined(ADB_Data__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in ADB_Data_SetInterruptMode() function.
     *  @{
     */
        #define ADB_Data_INTR_NONE      (uint16)(0x0000u)
        #define ADB_Data_INTR_RISING    (uint16)(0x0001u)
        #define ADB_Data_INTR_FALLING   (uint16)(0x0002u)
        #define ADB_Data_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define ADB_Data_INTR_MASK      (0x01u) 
#endif /* (ADB_Data__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define ADB_Data_PS                     (* (reg8 *) ADB_Data__PS)
/* Data Register */
#define ADB_Data_DR                     (* (reg8 *) ADB_Data__DR)
/* Port Number */
#define ADB_Data_PRT_NUM                (* (reg8 *) ADB_Data__PRT) 
/* Connect to Analog Globals */                                                  
#define ADB_Data_AG                     (* (reg8 *) ADB_Data__AG)                       
/* Analog MUX bux enable */
#define ADB_Data_AMUX                   (* (reg8 *) ADB_Data__AMUX) 
/* Bidirectional Enable */                                                        
#define ADB_Data_BIE                    (* (reg8 *) ADB_Data__BIE)
/* Bit-mask for Aliased Register Access */
#define ADB_Data_BIT_MASK               (* (reg8 *) ADB_Data__BIT_MASK)
/* Bypass Enable */
#define ADB_Data_BYP                    (* (reg8 *) ADB_Data__BYP)
/* Port wide control signals */                                                   
#define ADB_Data_CTL                    (* (reg8 *) ADB_Data__CTL)
/* Drive Modes */
#define ADB_Data_DM0                    (* (reg8 *) ADB_Data__DM0) 
#define ADB_Data_DM1                    (* (reg8 *) ADB_Data__DM1)
#define ADB_Data_DM2                    (* (reg8 *) ADB_Data__DM2) 
/* Input Buffer Disable Override */
#define ADB_Data_INP_DIS                (* (reg8 *) ADB_Data__INP_DIS)
/* LCD Common or Segment Drive */
#define ADB_Data_LCD_COM_SEG            (* (reg8 *) ADB_Data__LCD_COM_SEG)
/* Enable Segment LCD */
#define ADB_Data_LCD_EN                 (* (reg8 *) ADB_Data__LCD_EN)
/* Slew Rate Control */
#define ADB_Data_SLW                    (* (reg8 *) ADB_Data__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define ADB_Data_PRTDSI__CAPS_SEL       (* (reg8 *) ADB_Data__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define ADB_Data_PRTDSI__DBL_SYNC_IN    (* (reg8 *) ADB_Data__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define ADB_Data_PRTDSI__OE_SEL0        (* (reg8 *) ADB_Data__PRTDSI__OE_SEL0) 
#define ADB_Data_PRTDSI__OE_SEL1        (* (reg8 *) ADB_Data__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define ADB_Data_PRTDSI__OUT_SEL0       (* (reg8 *) ADB_Data__PRTDSI__OUT_SEL0) 
#define ADB_Data_PRTDSI__OUT_SEL1       (* (reg8 *) ADB_Data__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define ADB_Data_PRTDSI__SYNC_OUT       (* (reg8 *) ADB_Data__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(ADB_Data__SIO_CFG)
    #define ADB_Data_SIO_HYST_EN        (* (reg8 *) ADB_Data__SIO_HYST_EN)
    #define ADB_Data_SIO_REG_HIFREQ     (* (reg8 *) ADB_Data__SIO_REG_HIFREQ)
    #define ADB_Data_SIO_CFG            (* (reg8 *) ADB_Data__SIO_CFG)
    #define ADB_Data_SIO_DIFF           (* (reg8 *) ADB_Data__SIO_DIFF)
#endif /* (ADB_Data__SIO_CFG) */

/* Interrupt Registers */
#if defined(ADB_Data__INTSTAT)
    #define ADB_Data_INTSTAT            (* (reg8 *) ADB_Data__INTSTAT)
    #define ADB_Data_SNAP               (* (reg8 *) ADB_Data__SNAP)
    
	#define ADB_Data_0_INTTYPE_REG 		(* (reg8 *) ADB_Data__0__INTTYPE)
#endif /* (ADB_Data__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_ADB_Data_H */


/* [] END OF FILE */
