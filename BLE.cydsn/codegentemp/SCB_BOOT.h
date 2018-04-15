/***************************************************************************//**
* \file SCB_BOOT.h
* \version 4.0
*
* \brief
*  This file provides constants and parameter values of the bootloader
*  communication APIs for the SCB Component.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2014-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SCB_BOOT_SCB_H)
#define CY_SCB_BOOT_SCB_H

#include "SCB_PVT.h"

#if (SCB_SCB_MODE_I2C_INC)
    #include "SCB_I2C.h"
#endif /* (SCB_SCB_MODE_I2C_INC) */

#if (SCB_SCB_MODE_EZI2C_INC)
    #include "SCB_EZI2C.h"
#endif /* (SCB_SCB_MODE_EZI2C_INC) */

#if (SCB_SCB_MODE_SPI_INC || SCB_SCB_MODE_UART_INC)
    #include "SCB_SPI_UART.h"
#endif /* (SCB_SCB_MODE_SPI_INC || SCB_SCB_MODE_UART_INC) */


/***************************************
*  Conditional Compilation Parameters
****************************************/

/* Bootloader communication interface enable */
#define SCB_BTLDR_COMM_ENABLED ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SCB) || \
                                             (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))

/* Enable I2C bootloader communication */
#if (SCB_SCB_MODE_I2C_INC)
    #define SCB_I2C_BTLDR_COMM_ENABLED     (SCB_BTLDR_COMM_ENABLED && \
                                                            (SCB_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             SCB_I2C_SLAVE_CONST))
#else
     #define SCB_I2C_BTLDR_COMM_ENABLED    (0u)
#endif /* (SCB_SCB_MODE_I2C_INC) */

/* EZI2C does not support bootloader communication. Provide empty APIs */
#if (SCB_SCB_MODE_EZI2C_INC)
    #define SCB_EZI2C_BTLDR_COMM_ENABLED   (SCB_BTLDR_COMM_ENABLED && \
                                                         SCB_SCB_MODE_UNCONFIG_CONST_CFG)
#else
    #define SCB_EZI2C_BTLDR_COMM_ENABLED   (0u)
#endif /* (SCB_EZI2C_BTLDR_COMM_ENABLED) */

/* Enable SPI bootloader communication */
#if (SCB_SCB_MODE_SPI_INC)
    #define SCB_SPI_BTLDR_COMM_ENABLED     (SCB_BTLDR_COMM_ENABLED && \
                                                            (SCB_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             SCB_SPI_SLAVE_CONST))
#else
        #define SCB_SPI_BTLDR_COMM_ENABLED (0u)
#endif /* (SCB_SPI_BTLDR_COMM_ENABLED) */

/* Enable UART bootloader communication */
#if (SCB_SCB_MODE_UART_INC)
       #define SCB_UART_BTLDR_COMM_ENABLED    (SCB_BTLDR_COMM_ENABLED && \
                                                            (SCB_SCB_MODE_UNCONFIG_CONST_CFG || \
                                                             (SCB_UART_RX_DIRECTION && \
                                                              SCB_UART_TX_DIRECTION)))
#else
     #define SCB_UART_BTLDR_COMM_ENABLED   (0u)
#endif /* (SCB_UART_BTLDR_COMM_ENABLED) */

/* Enable bootloader communication */
#define SCB_BTLDR_COMM_MODE_ENABLED    (SCB_I2C_BTLDR_COMM_ENABLED   || \
                                                     SCB_SPI_BTLDR_COMM_ENABLED   || \
                                                     SCB_EZI2C_BTLDR_COMM_ENABLED || \
                                                     SCB_UART_BTLDR_COMM_ENABLED)


/***************************************
*        Function Prototypes
***************************************/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_I2C_BTLDR_COMM_ENABLED)
    /* I2C Bootloader physical layer functions */
    void SCB_I2CCyBtldrCommStart(void);
    void SCB_I2CCyBtldrCommStop (void);
    void SCB_I2CCyBtldrCommReset(void);
    cystatus SCB_I2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB_I2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map I2C specific bootloader communication APIs to SCB specific APIs */
    #if (SCB_SCB_MODE_I2C_CONST_CFG)
        #define SCB_CyBtldrCommStart   SCB_I2CCyBtldrCommStart
        #define SCB_CyBtldrCommStop    SCB_I2CCyBtldrCommStop
        #define SCB_CyBtldrCommReset   SCB_I2CCyBtldrCommReset
        #define SCB_CyBtldrCommRead    SCB_I2CCyBtldrCommRead
        #define SCB_CyBtldrCommWrite   SCB_I2CCyBtldrCommWrite
    #endif /* (SCB_SCB_MODE_I2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_I2C_BTLDR_COMM_ENABLED) */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_EZI2C_BTLDR_COMM_ENABLED)
    /* Bootloader physical layer functions */
    void SCB_EzI2CCyBtldrCommStart(void);
    void SCB_EzI2CCyBtldrCommStop (void);
    void SCB_EzI2CCyBtldrCommReset(void);
    cystatus SCB_EzI2CCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB_EzI2CCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map EZI2C specific bootloader communication APIs to SCB specific APIs */
    #if (SCB_SCB_MODE_EZI2C_CONST_CFG)
        #define SCB_CyBtldrCommStart   SCB_EzI2CCyBtldrCommStart
        #define SCB_CyBtldrCommStop    SCB_EzI2CCyBtldrCommStop
        #define SCB_CyBtldrCommReset   SCB_EzI2CCyBtldrCommReset
        #define SCB_CyBtldrCommRead    SCB_EzI2CCyBtldrCommRead
        #define SCB_CyBtldrCommWrite   SCB_EzI2CCyBtldrCommWrite
    #endif /* (SCB_SCB_MODE_EZI2C_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_EZI2C_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_SPI_BTLDR_COMM_ENABLED)
    /* SPI Bootloader physical layer functions */
    void SCB_SpiCyBtldrCommStart(void);
    void SCB_SpiCyBtldrCommStop (void);
    void SCB_SpiCyBtldrCommReset(void);
    cystatus SCB_SpiCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB_SpiCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map SPI specific bootloader communication APIs to SCB specific APIs */
    #if (SCB_SCB_MODE_SPI_CONST_CFG)
        #define SCB_CyBtldrCommStart   SCB_SpiCyBtldrCommStart
        #define SCB_CyBtldrCommStop    SCB_SpiCyBtldrCommStop
        #define SCB_CyBtldrCommReset   SCB_SpiCyBtldrCommReset
        #define SCB_CyBtldrCommRead    SCB_SpiCyBtldrCommRead
        #define SCB_CyBtldrCommWrite   SCB_SpiCyBtldrCommWrite
    #endif /* (SCB_SCB_MODE_SPI_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_SPI_BTLDR_COMM_ENABLED) */

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_UART_BTLDR_COMM_ENABLED)
    /* UART Bootloader physical layer functions */
    void SCB_UartCyBtldrCommStart(void);
    void SCB_UartCyBtldrCommStop (void);
    void SCB_UartCyBtldrCommReset(void);
    cystatus SCB_UartCyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    cystatus SCB_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);

    /* Map UART specific bootloader communication APIs to SCB specific APIs */
    #if (SCB_SCB_MODE_UART_CONST_CFG)
        #define SCB_CyBtldrCommStart   SCB_UartCyBtldrCommStart
        #define SCB_CyBtldrCommStop    SCB_UartCyBtldrCommStop
        #define SCB_CyBtldrCommReset   SCB_UartCyBtldrCommReset
        #define SCB_CyBtldrCommRead    SCB_UartCyBtldrCommRead
        #define SCB_CyBtldrCommWrite   SCB_UartCyBtldrCommWrite
    #endif /* (SCB_SCB_MODE_UART_CONST_CFG) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_UART_BTLDR_COMM_ENABLED) */

/**
* \addtogroup group_bootloader
* @{
*/

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_BTLDR_COMM_ENABLED)
    #if (SCB_SCB_MODE_UNCONFIG_CONST_CFG)
        /* Bootloader physical layer functions */
        void SCB_CyBtldrCommStart(void);
        void SCB_CyBtldrCommStop (void);
        void SCB_CyBtldrCommReset(void);
        cystatus SCB_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
        cystatus SCB_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut);
    #endif /* (SCB_SCB_MODE_UNCONFIG_CONST_CFG) */

    /* Map SCB specific bootloader communication APIs to common APIs */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SCB)
        #define CyBtldrCommStart    SCB_CyBtldrCommStart
        #define CyBtldrCommStop     SCB_CyBtldrCommStop
        #define CyBtldrCommReset    SCB_CyBtldrCommReset
        #define CyBtldrCommWrite    SCB_CyBtldrCommWrite
        #define CyBtldrCommRead     SCB_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_SCB) */

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB_BTLDR_COMM_ENABLED) */

/** @} group_bootloader */

/***************************************
*           API Constants
***************************************/

/* Timeout unit in milliseconds */
#define SCB_WAIT_1_MS  (1u)

/* Return number of bytes to copy into bootloader buffer */
#define SCB_BYTES_TO_COPY(actBufSize, bufSize) \
                            ( ((uint32)(actBufSize) < (uint32)(bufSize)) ? \
                                ((uint32) (actBufSize)) : ((uint32) (bufSize)) )

/* Size of Read/Write buffers for I2C bootloader  */
#define SCB_I2C_BTLDR_SIZEOF_READ_BUFFER   (64u)
#define SCB_I2C_BTLDR_SIZEOF_WRITE_BUFFER  (64u)

/* Byte to byte time interval: calculated basing on current component
* data rate configuration, can be defined in project if required.
*/
#ifndef SCB_SPI_BYTE_TO_BYTE
    #define SCB_SPI_BYTE_TO_BYTE   (160u)
#endif

/* Byte to byte time interval: calculated basing on current component
* baud rate configuration, can be defined in the project if required.
*/
#ifndef SCB_UART_BYTE_TO_BYTE
    #define SCB_UART_BYTE_TO_BYTE  (2500u)
#endif /* SCB_UART_BYTE_TO_BYTE */

#endif /* (CY_SCB_BOOT_SCB_H) */


/* [] END OF FILE */
