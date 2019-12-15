/*******************************************************************************
* File Name: SenseIRQ.h
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
#if !defined(CY_ISR_SenseIRQ_H)
#define CY_ISR_SenseIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void SenseIRQ_Start(void);
void SenseIRQ_StartEx(cyisraddress address);
void SenseIRQ_Stop(void);

CY_ISR_PROTO(SenseIRQ_Interrupt);

void SenseIRQ_SetVector(cyisraddress address);
cyisraddress SenseIRQ_GetVector(void);

void SenseIRQ_SetPriority(uint8 priority);
uint8 SenseIRQ_GetPriority(void);

void SenseIRQ_Enable(void);
uint8 SenseIRQ_GetState(void);
void SenseIRQ_Disable(void);

void SenseIRQ_SetPending(void);
void SenseIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the SenseIRQ ISR. */
#define SenseIRQ_INTC_VECTOR            ((reg32 *) SenseIRQ__INTC_VECT)

/* Address of the SenseIRQ ISR priority. */
#define SenseIRQ_INTC_PRIOR             ((reg8 *) SenseIRQ__INTC_PRIOR_REG)

/* Priority of the SenseIRQ interrupt. */
#define SenseIRQ_INTC_PRIOR_NUMBER      SenseIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable SenseIRQ interrupt. */
#define SenseIRQ_INTC_SET_EN            ((reg32 *) SenseIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the SenseIRQ interrupt. */
#define SenseIRQ_INTC_CLR_EN            ((reg32 *) SenseIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the SenseIRQ interrupt state to pending. */
#define SenseIRQ_INTC_SET_PD            ((reg32 *) SenseIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the SenseIRQ interrupt. */
#define SenseIRQ_INTC_CLR_PD            ((reg32 *) SenseIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_SenseIRQ_H */


/* [] END OF FILE */
