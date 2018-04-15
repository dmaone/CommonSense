/***************************************************************************//**
* \file .h
* \version 4.0
*
* \brief
*  This private file provides constants and parameter values for the
*  SCB Component in I2C mode.
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

#if !defined(CY_SCB_I2C_PVT_SCB_H)
#define CY_SCB_I2C_PVT_SCB_H

#include "SCB_I2C.h"


/***************************************
*     Private Global Vars
***************************************/

extern volatile uint8 SCB_state; /* Current state of I2C FSM */

#if(SCB_I2C_SLAVE_CONST)
    extern volatile uint8 SCB_slStatus;          /* Slave Status */

    /* Receive buffer variables */
    extern volatile uint8 * SCB_slWrBufPtr;      /* Pointer to Receive buffer  */
    extern volatile uint32  SCB_slWrBufSize;     /* Slave Receive buffer size  */
    extern volatile uint32  SCB_slWrBufIndex;    /* Slave Receive buffer Index */

    /* Transmit buffer variables */
    extern volatile uint8 * SCB_slRdBufPtr;      /* Pointer to Transmit buffer  */
    extern volatile uint32  SCB_slRdBufSize;     /* Slave Transmit buffer size  */
    extern volatile uint32  SCB_slRdBufIndex;    /* Slave Transmit buffer Index */
    extern volatile uint32  SCB_slRdBufIndexTmp; /* Slave Transmit buffer Index Tmp */
    extern volatile uint8   SCB_slOverFlowCount; /* Slave Transmit Overflow counter */
#endif /* (SCB_I2C_SLAVE_CONST) */

#if(SCB_I2C_MASTER_CONST)
    extern volatile uint16 SCB_mstrStatus;      /* Master Status byte  */
    extern volatile uint8  SCB_mstrControl;     /* Master Control byte */

    /* Receive buffer variables */
    extern volatile uint8 * SCB_mstrRdBufPtr;   /* Pointer to Master Read buffer */
    extern volatile uint32  SCB_mstrRdBufSize;  /* Master Read buffer size       */
    extern volatile uint32  SCB_mstrRdBufIndex; /* Master Read buffer Index      */

    /* Transmit buffer variables */
    extern volatile uint8 * SCB_mstrWrBufPtr;   /* Pointer to Master Write buffer */
    extern volatile uint32  SCB_mstrWrBufSize;  /* Master Write buffer size       */
    extern volatile uint32  SCB_mstrWrBufIndex; /* Master Write buffer Index      */
    extern volatile uint32  SCB_mstrWrBufIndexTmp; /* Master Write buffer Index Tmp */
#endif /* (SCB_I2C_MASTER_CONST) */

#if (SCB_I2C_CUSTOM_ADDRESS_HANDLER_CONST)
    extern uint32 (*SCB_customAddressHandler) (void);
#endif /* (SCB_I2C_CUSTOM_ADDRESS_HANDLER_CONST) */

/***************************************
*     Private Function Prototypes
***************************************/

#if(SCB_SCB_MODE_I2C_CONST_CFG)
    void SCB_I2CInit(void);
#endif /* (SCB_SCB_MODE_I2C_CONST_CFG) */

void SCB_I2CStop(void);
void SCB_I2CFwBlockReset(void);

void SCB_I2CSaveConfig(void);
void SCB_I2CRestoreConfig(void);

#if(SCB_I2C_MASTER_CONST)
    void SCB_I2CReStartGeneration(void);
#endif /* (SCB_I2C_MASTER_CONST) */

#endif /* (CY_SCB_I2C_PVT_SCB_H) */


/* [] END OF FILE */
