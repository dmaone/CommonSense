/*******************************************************************************
* File Name: TimerIRQ.h
* Version 1.70
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
#if !defined(CY_ISR_TimerIRQ_H)
#define CY_ISR_TimerIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void TimerIRQ_Start(void);
void TimerIRQ_StartEx(cyisraddress address);
void TimerIRQ_Stop(void);

CY_ISR_PROTO(TimerIRQ_Interrupt);

void TimerIRQ_SetVector(cyisraddress address);
cyisraddress TimerIRQ_GetVector(void);

void TimerIRQ_SetPriority(uint8 priority);
uint8 TimerIRQ_GetPriority(void);

void TimerIRQ_Enable(void);
uint8 TimerIRQ_GetState(void);
void TimerIRQ_Disable(void);

void TimerIRQ_SetPending(void);
void TimerIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the TimerIRQ ISR. */
#define TimerIRQ_INTC_VECTOR            ((reg32 *) TimerIRQ__INTC_VECT)

/* Address of the TimerIRQ ISR priority. */
#define TimerIRQ_INTC_PRIOR             ((reg8 *) TimerIRQ__INTC_PRIOR_REG)

/* Priority of the TimerIRQ interrupt. */
#define TimerIRQ_INTC_PRIOR_NUMBER      TimerIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable TimerIRQ interrupt. */
#define TimerIRQ_INTC_SET_EN            ((reg32 *) TimerIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the TimerIRQ interrupt. */
#define TimerIRQ_INTC_CLR_EN            ((reg32 *) TimerIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the TimerIRQ interrupt state to pending. */
#define TimerIRQ_INTC_SET_PD            ((reg32 *) TimerIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the TimerIRQ interrupt. */
#define TimerIRQ_INTC_CLR_PD            ((reg32 *) TimerIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_TimerIRQ_H */


/* [] END OF FILE */
