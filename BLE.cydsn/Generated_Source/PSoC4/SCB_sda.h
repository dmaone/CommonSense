/*******************************************************************************
* File Name: SCB_sda.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_SCB_sda_H) /* Pins SCB_sda_H */
#define CY_PINS_SCB_sda_H

#include "cytypes.h"
#include "cyfitter.h"
#include "SCB_sda_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} SCB_sda_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   SCB_sda_Read(void);
void    SCB_sda_Write(uint8 value);
uint8   SCB_sda_ReadDataReg(void);
#if defined(SCB_sda__PC) || (CY_PSOC4_4200L) 
    void    SCB_sda_SetDriveMode(uint8 mode);
#endif
void    SCB_sda_SetInterruptMode(uint16 position, uint16 mode);
uint8   SCB_sda_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void SCB_sda_Sleep(void); 
void SCB_sda_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(SCB_sda__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define SCB_sda_DRIVE_MODE_BITS        (3)
    #define SCB_sda_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - SCB_sda_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the SCB_sda_SetDriveMode() function.
         *  @{
         */
        #define SCB_sda_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define SCB_sda_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define SCB_sda_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define SCB_sda_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define SCB_sda_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define SCB_sda_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define SCB_sda_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define SCB_sda_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define SCB_sda_MASK               SCB_sda__MASK
#define SCB_sda_SHIFT              SCB_sda__SHIFT
#define SCB_sda_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SCB_sda_SetInterruptMode() function.
     *  @{
     */
        #define SCB_sda_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define SCB_sda_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define SCB_sda_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define SCB_sda_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(SCB_sda__SIO)
    #define SCB_sda_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(SCB_sda__PC) && (CY_PSOC4_4200L)
    #define SCB_sda_USBIO_ENABLE               ((uint32)0x80000000u)
    #define SCB_sda_USBIO_DISABLE              ((uint32)(~SCB_sda_USBIO_ENABLE))
    #define SCB_sda_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define SCB_sda_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define SCB_sda_USBIO_ENTER_SLEEP          ((uint32)((1u << SCB_sda_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << SCB_sda_USBIO_SUSPEND_DEL_SHIFT)))
    #define SCB_sda_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << SCB_sda_USBIO_SUSPEND_SHIFT)))
    #define SCB_sda_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << SCB_sda_USBIO_SUSPEND_DEL_SHIFT)))
    #define SCB_sda_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(SCB_sda__PC)
    /* Port Configuration */
    #define SCB_sda_PC                 (* (reg32 *) SCB_sda__PC)
#endif
/* Pin State */
#define SCB_sda_PS                     (* (reg32 *) SCB_sda__PS)
/* Data Register */
#define SCB_sda_DR                     (* (reg32 *) SCB_sda__DR)
/* Input Buffer Disable Override */
#define SCB_sda_INP_DIS                (* (reg32 *) SCB_sda__PC2)

/* Interrupt configuration Registers */
#define SCB_sda_INTCFG                 (* (reg32 *) SCB_sda__INTCFG)
#define SCB_sda_INTSTAT                (* (reg32 *) SCB_sda__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define SCB_sda_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(SCB_sda__SIO)
    #define SCB_sda_SIO_REG            (* (reg32 *) SCB_sda__SIO)
#endif /* (SCB_sda__SIO_CFG) */

/* USBIO registers */
#if !defined(SCB_sda__PC) && (CY_PSOC4_4200L)
    #define SCB_sda_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define SCB_sda_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define SCB_sda_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define SCB_sda_DRIVE_MODE_SHIFT       (0x00u)
#define SCB_sda_DRIVE_MODE_MASK        (0x07u << SCB_sda_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins SCB_sda_H */


/* [] END OF FILE */
