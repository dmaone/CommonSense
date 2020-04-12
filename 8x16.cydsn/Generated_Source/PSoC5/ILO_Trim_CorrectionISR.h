/*******************************************************************************
* File Name: ILO_Trim_CorrectionISR.h
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
#if !defined(CY_ISR_ILO_Trim_CorrectionISR_H)
#define CY_ISR_ILO_Trim_CorrectionISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void ILO_Trim_CorrectionISR_Start(void);
void ILO_Trim_CorrectionISR_StartEx(cyisraddress address);
void ILO_Trim_CorrectionISR_Stop(void);

CY_ISR_PROTO(ILO_Trim_CorrectionISR_Interrupt);

void ILO_Trim_CorrectionISR_SetVector(cyisraddress address);
cyisraddress ILO_Trim_CorrectionISR_GetVector(void);

void ILO_Trim_CorrectionISR_SetPriority(uint8 priority);
uint8 ILO_Trim_CorrectionISR_GetPriority(void);

void ILO_Trim_CorrectionISR_Enable(void);
uint8 ILO_Trim_CorrectionISR_GetState(void);
void ILO_Trim_CorrectionISR_Disable(void);

void ILO_Trim_CorrectionISR_SetPending(void);
void ILO_Trim_CorrectionISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the ILO_Trim_CorrectionISR ISR. */
#define ILO_Trim_CorrectionISR_INTC_VECTOR            ((reg32 *) ILO_Trim_CorrectionISR__INTC_VECT)

/* Address of the ILO_Trim_CorrectionISR ISR priority. */
#define ILO_Trim_CorrectionISR_INTC_PRIOR             ((reg8 *) ILO_Trim_CorrectionISR__INTC_PRIOR_REG)

/* Priority of the ILO_Trim_CorrectionISR interrupt. */
#define ILO_Trim_CorrectionISR_INTC_PRIOR_NUMBER      ILO_Trim_CorrectionISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable ILO_Trim_CorrectionISR interrupt. */
#define ILO_Trim_CorrectionISR_INTC_SET_EN            ((reg32 *) ILO_Trim_CorrectionISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the ILO_Trim_CorrectionISR interrupt. */
#define ILO_Trim_CorrectionISR_INTC_CLR_EN            ((reg32 *) ILO_Trim_CorrectionISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the ILO_Trim_CorrectionISR interrupt state to pending. */
#define ILO_Trim_CorrectionISR_INTC_SET_PD            ((reg32 *) ILO_Trim_CorrectionISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the ILO_Trim_CorrectionISR interrupt. */
#define ILO_Trim_CorrectionISR_INTC_CLR_PD            ((reg32 *) ILO_Trim_CorrectionISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_ILO_Trim_CorrectionISR_H */


/* [] END OF FILE */
