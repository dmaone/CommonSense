/*******************************************************************************
* File Name: PS2CLK_IRQ.h
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
#if !defined(CY_ISR_PS2CLK_IRQ_H)
#define CY_ISR_PS2CLK_IRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void PS2CLK_IRQ_Start(void);
void PS2CLK_IRQ_StartEx(cyisraddress address);
void PS2CLK_IRQ_Stop(void);

CY_ISR_PROTO(PS2CLK_IRQ_Interrupt);

void PS2CLK_IRQ_SetVector(cyisraddress address);
cyisraddress PS2CLK_IRQ_GetVector(void);

void PS2CLK_IRQ_SetPriority(uint8 priority);
uint8 PS2CLK_IRQ_GetPriority(void);

void PS2CLK_IRQ_Enable(void);
uint8 PS2CLK_IRQ_GetState(void);
void PS2CLK_IRQ_Disable(void);

void PS2CLK_IRQ_SetPending(void);
void PS2CLK_IRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the PS2CLK_IRQ ISR. */
#define PS2CLK_IRQ_INTC_VECTOR            ((reg32 *) PS2CLK_IRQ__INTC_VECT)

/* Address of the PS2CLK_IRQ ISR priority. */
#define PS2CLK_IRQ_INTC_PRIOR             ((reg8 *) PS2CLK_IRQ__INTC_PRIOR_REG)

/* Priority of the PS2CLK_IRQ interrupt. */
#define PS2CLK_IRQ_INTC_PRIOR_NUMBER      PS2CLK_IRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable PS2CLK_IRQ interrupt. */
#define PS2CLK_IRQ_INTC_SET_EN            ((reg32 *) PS2CLK_IRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the PS2CLK_IRQ interrupt. */
#define PS2CLK_IRQ_INTC_CLR_EN            ((reg32 *) PS2CLK_IRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the PS2CLK_IRQ interrupt state to pending. */
#define PS2CLK_IRQ_INTC_SET_PD            ((reg32 *) PS2CLK_IRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the PS2CLK_IRQ interrupt. */
#define PS2CLK_IRQ_INTC_CLR_PD            ((reg32 *) PS2CLK_IRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_PS2CLK_IRQ_H */


/* [] END OF FILE */
