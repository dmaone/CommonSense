/*******************************************************************************
* File Name: EoCIRQ.h
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
#if !defined(CY_ISR_EoCIRQ_H)
#define CY_ISR_EoCIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void EoCIRQ_Start(void);
void EoCIRQ_StartEx(cyisraddress address);
void EoCIRQ_Stop(void);

CY_ISR_PROTO(EoCIRQ_Interrupt);

void EoCIRQ_SetVector(cyisraddress address);
cyisraddress EoCIRQ_GetVector(void);

void EoCIRQ_SetPriority(uint8 priority);
uint8 EoCIRQ_GetPriority(void);

void EoCIRQ_Enable(void);
uint8 EoCIRQ_GetState(void);
void EoCIRQ_Disable(void);

void EoCIRQ_SetPending(void);
void EoCIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the EoCIRQ ISR. */
#define EoCIRQ_INTC_VECTOR            ((reg32 *) EoCIRQ__INTC_VECT)

/* Address of the EoCIRQ ISR priority. */
#define EoCIRQ_INTC_PRIOR             ((reg8 *) EoCIRQ__INTC_PRIOR_REG)

/* Priority of the EoCIRQ interrupt. */
#define EoCIRQ_INTC_PRIOR_NUMBER      EoCIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable EoCIRQ interrupt. */
#define EoCIRQ_INTC_SET_EN            ((reg32 *) EoCIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the EoCIRQ interrupt. */
#define EoCIRQ_INTC_CLR_EN            ((reg32 *) EoCIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the EoCIRQ interrupt state to pending. */
#define EoCIRQ_INTC_SET_PD            ((reg32 *) EoCIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the EoCIRQ interrupt. */
#define EoCIRQ_INTC_CLR_PD            ((reg32 *) EoCIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_EoCIRQ_H */


/* [] END OF FILE */
