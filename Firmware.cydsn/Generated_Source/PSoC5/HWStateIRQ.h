/*******************************************************************************
* File Name: HWStateIRQ.h
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
#if !defined(CY_ISR_HWStateIRQ_H)
#define CY_ISR_HWStateIRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void HWStateIRQ_Start(void);
void HWStateIRQ_StartEx(cyisraddress address);
void HWStateIRQ_Stop(void);

CY_ISR_PROTO(HWStateIRQ_Interrupt);

void HWStateIRQ_SetVector(cyisraddress address);
cyisraddress HWStateIRQ_GetVector(void);

void HWStateIRQ_SetPriority(uint8 priority);
uint8 HWStateIRQ_GetPriority(void);

void HWStateIRQ_Enable(void);
uint8 HWStateIRQ_GetState(void);
void HWStateIRQ_Disable(void);

void HWStateIRQ_SetPending(void);
void HWStateIRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the HWStateIRQ ISR. */
#define HWStateIRQ_INTC_VECTOR            ((reg32 *) HWStateIRQ__INTC_VECT)

/* Address of the HWStateIRQ ISR priority. */
#define HWStateIRQ_INTC_PRIOR             ((reg8 *) HWStateIRQ__INTC_PRIOR_REG)

/* Priority of the HWStateIRQ interrupt. */
#define HWStateIRQ_INTC_PRIOR_NUMBER      HWStateIRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable HWStateIRQ interrupt. */
#define HWStateIRQ_INTC_SET_EN            ((reg32 *) HWStateIRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the HWStateIRQ interrupt. */
#define HWStateIRQ_INTC_CLR_EN            ((reg32 *) HWStateIRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the HWStateIRQ interrupt state to pending. */
#define HWStateIRQ_INTC_SET_PD            ((reg32 *) HWStateIRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the HWStateIRQ interrupt. */
#define HWStateIRQ_INTC_CLR_PD            ((reg32 *) HWStateIRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_HWStateIRQ_H */


/* [] END OF FILE */
