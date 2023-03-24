/*******************************************************************************
* File Name: BootIRQ.h
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
#if !defined(CY_ISR_BootIRQ_H)
#define CY_ISR_BootIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void BootIRQ_Start(void);
void BootIRQ_StartEx(cyisraddress address);
void BootIRQ_Stop(void);

CY_ISR_PROTO(BootIRQ_Interrupt);

void BootIRQ_SetVector(cyisraddress address);
cyisraddress BootIRQ_GetVector(void);

void BootIRQ_SetPriority(uint8 priority);
uint8 BootIRQ_GetPriority(void);

void BootIRQ_Enable(void);
uint8 BootIRQ_GetState(void);
void BootIRQ_Disable(void);

void BootIRQ_SetPending(void);
void BootIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the BootIRQ ISR. */
#define BootIRQ_INTC_VECTOR            ((reg32 *) BootIRQ__INTC_VECT)

/* Address of the BootIRQ ISR priority. */
#define BootIRQ_INTC_PRIOR             ((reg8 *) BootIRQ__INTC_PRIOR_REG)

/* Priority of the BootIRQ interrupt. */
#define BootIRQ_INTC_PRIOR_NUMBER      BootIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable BootIRQ interrupt. */
#define BootIRQ_INTC_SET_EN            ((reg32 *) BootIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the BootIRQ interrupt. */
#define BootIRQ_INTC_CLR_EN            ((reg32 *) BootIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the BootIRQ interrupt state to pending. */
#define BootIRQ_INTC_SET_PD            ((reg32 *) BootIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the BootIRQ interrupt. */
#define BootIRQ_INTC_CLR_PD            ((reg32 *) BootIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_BootIRQ_H */


/* [] END OF FILE */
