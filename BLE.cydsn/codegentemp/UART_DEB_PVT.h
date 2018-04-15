/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component.
*  Please do not use this file or its content in your project.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_PVT_UART_DEB_H)
#define CY_SCB_PVT_UART_DEB_H

#include "UART_DEB.h"


/***************************************
*     Private Function Prototypes
***************************************/

/* APIs to service INTR_I2C_EC register */
#define UART_DEB_SetI2CExtClkInterruptMode(interruptMask) UART_DEB_WRITE_INTR_I2C_EC_MASK(interruptMask)
#define UART_DEB_ClearI2CExtClkInterruptSource(interruptMask) UART_DEB_CLEAR_INTR_I2C_EC(interruptMask)
#define UART_DEB_GetI2CExtClkInterruptSource()                (UART_DEB_INTR_I2C_EC_REG)
#define UART_DEB_GetI2CExtClkInterruptMode()                  (UART_DEB_INTR_I2C_EC_MASK_REG)
#define UART_DEB_GetI2CExtClkInterruptSourceMasked()          (UART_DEB_INTR_I2C_EC_MASKED_REG)

#if (!UART_DEB_CY_SCBIP_V1)
    /* APIs to service INTR_SPI_EC register */
    #define UART_DEB_SetSpiExtClkInterruptMode(interruptMask) \
                                                                UART_DEB_WRITE_INTR_SPI_EC_MASK(interruptMask)
    #define UART_DEB_ClearSpiExtClkInterruptSource(interruptMask) \
                                                                UART_DEB_CLEAR_INTR_SPI_EC(interruptMask)
    #define UART_DEB_GetExtSpiClkInterruptSource()                 (UART_DEB_INTR_SPI_EC_REG)
    #define UART_DEB_GetExtSpiClkInterruptMode()                   (UART_DEB_INTR_SPI_EC_MASK_REG)
    #define UART_DEB_GetExtSpiClkInterruptSourceMasked()           (UART_DEB_INTR_SPI_EC_MASKED_REG)
#endif /* (!UART_DEB_CY_SCBIP_V1) */

#if(UART_DEB_SCB_MODE_UNCONFIG_CONST_CFG)
    extern void UART_DEB_SetPins(uint32 mode, uint32 subMode, uint32 uartEnableMask);
#endif /* (UART_DEB_SCB_MODE_UNCONFIG_CONST_CFG) */


/***************************************
*     Vars with External Linkage
***************************************/

#if (UART_DEB_SCB_IRQ_INTERNAL)
#if !defined (CY_REMOVE_UART_DEB_CUSTOM_INTR_HANDLER)
    extern cyisraddress UART_DEB_customIntrHandler;
#endif /* !defined (CY_REMOVE_UART_DEB_CUSTOM_INTR_HANDLER) */
#endif /* (UART_DEB_SCB_IRQ_INTERNAL) */

extern UART_DEB_BACKUP_STRUCT UART_DEB_backup;

#if(UART_DEB_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Common configuration variables */
    extern uint8 UART_DEB_scbMode;
    extern uint8 UART_DEB_scbEnableWake;
    extern uint8 UART_DEB_scbEnableIntr;

    /* I2C configuration variables */
    extern uint8 UART_DEB_mode;
    extern uint8 UART_DEB_acceptAddr;

    /* SPI/UART configuration variables */
    extern volatile uint8 * UART_DEB_rxBuffer;
    extern uint8   UART_DEB_rxDataBits;
    extern uint32  UART_DEB_rxBufferSize;

    extern volatile uint8 * UART_DEB_txBuffer;
    extern uint8   UART_DEB_txDataBits;
    extern uint32  UART_DEB_txBufferSize;

    /* EZI2C configuration variables */
    extern uint8 UART_DEB_numberOfAddr;
    extern uint8 UART_DEB_subAddrSize;
#endif /* (UART_DEB_SCB_MODE_UNCONFIG_CONST_CFG) */

#if (! (UART_DEB_SCB_MODE_I2C_CONST_CFG || \
        UART_DEB_SCB_MODE_EZI2C_CONST_CFG))
    extern uint16 UART_DEB_IntrTxMask;
#endif /* (! (UART_DEB_SCB_MODE_I2C_CONST_CFG || \
              UART_DEB_SCB_MODE_EZI2C_CONST_CFG)) */


/***************************************
*        Conditional Macro
****************************************/

#if(UART_DEB_SCB_MODE_UNCONFIG_CONST_CFG)
    /* Defines run time operation mode */
    #define UART_DEB_SCB_MODE_I2C_RUNTM_CFG     (UART_DEB_SCB_MODE_I2C      == UART_DEB_scbMode)
    #define UART_DEB_SCB_MODE_SPI_RUNTM_CFG     (UART_DEB_SCB_MODE_SPI      == UART_DEB_scbMode)
    #define UART_DEB_SCB_MODE_UART_RUNTM_CFG    (UART_DEB_SCB_MODE_UART     == UART_DEB_scbMode)
    #define UART_DEB_SCB_MODE_EZI2C_RUNTM_CFG   (UART_DEB_SCB_MODE_EZI2C    == UART_DEB_scbMode)
    #define UART_DEB_SCB_MODE_UNCONFIG_RUNTM_CFG \
                                                        (UART_DEB_SCB_MODE_UNCONFIG == UART_DEB_scbMode)

    /* Defines wakeup enable */
    #define UART_DEB_SCB_WAKE_ENABLE_CHECK       (0u != UART_DEB_scbEnableWake)
#endif /* (UART_DEB_SCB_MODE_UNCONFIG_CONST_CFG) */

/* Defines maximum number of SCB pins */
#if (!UART_DEB_CY_SCBIP_V1)
    #define UART_DEB_SCB_PINS_NUMBER    (7u)
#else
    #define UART_DEB_SCB_PINS_NUMBER    (2u)
#endif /* (!UART_DEB_CY_SCBIP_V1) */

#endif /* (CY_SCB_PVT_UART_DEB_H) */


/* [] END OF FILE */
