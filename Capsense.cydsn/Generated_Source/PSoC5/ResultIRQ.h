/*******************************************************************************
* File Name: ResultIRQ.h
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
#if !defined(CY_ISR_ResultIRQ_H)
#define CY_ISR_ResultIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void ResultIRQ_Start(void);
void ResultIRQ_StartEx(cyisraddress address);
void ResultIRQ_Stop(void);

CY_ISR_PROTO(ResultIRQ_Interrupt);

void ResultIRQ_SetVector(cyisraddress address);
cyisraddress ResultIRQ_GetVector(void);

void ResultIRQ_SetPriority(uint8 priority);
uint8 ResultIRQ_GetPriority(void);

void ResultIRQ_Enable(void);
uint8 ResultIRQ_GetState(void);
void ResultIRQ_Disable(void);

void ResultIRQ_SetPending(void);
void ResultIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the ResultIRQ ISR. */
#define ResultIRQ_INTC_VECTOR            ((reg32 *) ResultIRQ__INTC_VECT)

/* Address of the ResultIRQ ISR priority. */
#define ResultIRQ_INTC_PRIOR             ((reg8 *) ResultIRQ__INTC_PRIOR_REG)

/* Priority of the ResultIRQ interrupt. */
#define ResultIRQ_INTC_PRIOR_NUMBER      ResultIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable ResultIRQ interrupt. */
#define ResultIRQ_INTC_SET_EN            ((reg32 *) ResultIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the ResultIRQ interrupt. */
#define ResultIRQ_INTC_CLR_EN            ((reg32 *) ResultIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the ResultIRQ interrupt state to pending. */
#define ResultIRQ_INTC_SET_PD            ((reg32 *) ResultIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the ResultIRQ interrupt. */
#define ResultIRQ_INTC_CLR_PD            ((reg32 *) ResultIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_ResultIRQ_H */


/* [] END OF FILE */
