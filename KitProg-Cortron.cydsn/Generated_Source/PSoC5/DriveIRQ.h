/*******************************************************************************
* File Name: DriveIRQ.h
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
#if !defined(CY_ISR_DriveIRQ_H)
#define CY_ISR_DriveIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void DriveIRQ_Start(void);
void DriveIRQ_StartEx(cyisraddress address);
void DriveIRQ_Stop(void);

CY_ISR_PROTO(DriveIRQ_Interrupt);

void DriveIRQ_SetVector(cyisraddress address);
cyisraddress DriveIRQ_GetVector(void);

void DriveIRQ_SetPriority(uint8 priority);
uint8 DriveIRQ_GetPriority(void);

void DriveIRQ_Enable(void);
uint8 DriveIRQ_GetState(void);
void DriveIRQ_Disable(void);

void DriveIRQ_SetPending(void);
void DriveIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the DriveIRQ ISR. */
#define DriveIRQ_INTC_VECTOR            ((reg32 *) DriveIRQ__INTC_VECT)

/* Address of the DriveIRQ ISR priority. */
#define DriveIRQ_INTC_PRIOR             ((reg8 *) DriveIRQ__INTC_PRIOR_REG)

/* Priority of the DriveIRQ interrupt. */
#define DriveIRQ_INTC_PRIOR_NUMBER      DriveIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable DriveIRQ interrupt. */
#define DriveIRQ_INTC_SET_EN            ((reg32 *) DriveIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the DriveIRQ interrupt. */
#define DriveIRQ_INTC_CLR_EN            ((reg32 *) DriveIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the DriveIRQ interrupt state to pending. */
#define DriveIRQ_INTC_SET_PD            ((reg32 *) DriveIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the DriveIRQ interrupt. */
#define DriveIRQ_INTC_CLR_PD            ((reg32 *) DriveIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_DriveIRQ_H */


/* [] END OF FILE */
