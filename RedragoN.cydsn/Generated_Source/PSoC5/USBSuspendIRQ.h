/*******************************************************************************
* File Name: USBSuspendIRQ.h
* Version 1.71
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_USBSuspendIRQ_H)
#define CY_ISR_USBSuspendIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void USBSuspendIRQ_Start(void);
void USBSuspendIRQ_StartEx(cyisraddress address);
void USBSuspendIRQ_Stop(void);

CY_ISR_PROTO(USBSuspendIRQ_Interrupt);

void USBSuspendIRQ_SetVector(cyisraddress address);
cyisraddress USBSuspendIRQ_GetVector(void);

void USBSuspendIRQ_SetPriority(uint8 priority);
uint8 USBSuspendIRQ_GetPriority(void);

void USBSuspendIRQ_Enable(void);
uint8 USBSuspendIRQ_GetState(void);
void USBSuspendIRQ_Disable(void);

void USBSuspendIRQ_SetPending(void);
void USBSuspendIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the USBSuspendIRQ ISR. */
#define USBSuspendIRQ_INTC_VECTOR            ((reg32 *) USBSuspendIRQ__INTC_VECT)

/* Address of the USBSuspendIRQ ISR priority. */
#define USBSuspendIRQ_INTC_PRIOR             ((reg8 *) USBSuspendIRQ__INTC_PRIOR_REG)

/* Priority of the USBSuspendIRQ interrupt. */
#define USBSuspendIRQ_INTC_PRIOR_NUMBER      USBSuspendIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable USBSuspendIRQ interrupt. */
#define USBSuspendIRQ_INTC_SET_EN            ((reg32 *) USBSuspendIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the USBSuspendIRQ interrupt. */
#define USBSuspendIRQ_INTC_CLR_EN            ((reg32 *) USBSuspendIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the USBSuspendIRQ interrupt state to pending. */
#define USBSuspendIRQ_INTC_SET_PD            ((reg32 *) USBSuspendIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the USBSuspendIRQ interrupt. */
#define USBSuspendIRQ_INTC_CLR_PD            ((reg32 *) USBSuspendIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_USBSuspendIRQ_H */


/* [] END OF FILE */
