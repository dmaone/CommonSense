/*******************************************************************************
* File Name: Pedals0.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Pedals0_ALIASES_H) /* Pins Pedals0_ALIASES_H */
#define CY_PINS_Pedals0_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Pedals0_0			(Pedals0__0__PC)
#define Pedals0_0_INTR	((uint16)((uint16)0x0001u << Pedals0__0__SHIFT))

#define Pedals0_1			(Pedals0__1__PC)
#define Pedals0_1_INTR	((uint16)((uint16)0x0001u << Pedals0__1__SHIFT))

#define Pedals0_2			(Pedals0__2__PC)
#define Pedals0_2_INTR	((uint16)((uint16)0x0001u << Pedals0__2__SHIFT))

#define Pedals0_3			(Pedals0__3__PC)
#define Pedals0_3_INTR	((uint16)((uint16)0x0001u << Pedals0__3__SHIFT))

#define Pedals0_4			(Pedals0__4__PC)
#define Pedals0_4_INTR	((uint16)((uint16)0x0001u << Pedals0__4__SHIFT))

#define Pedals0_5			(Pedals0__5__PC)
#define Pedals0_5_INTR	((uint16)((uint16)0x0001u << Pedals0__5__SHIFT))

#define Pedals0_6			(Pedals0__6__PC)
#define Pedals0_6_INTR	((uint16)((uint16)0x0001u << Pedals0__6__SHIFT))

#define Pedals0_7			(Pedals0__7__PC)
#define Pedals0_7_INTR	((uint16)((uint16)0x0001u << Pedals0__7__SHIFT))

#define Pedals0_INTR_ALL	 ((uint16)(Pedals0_0_INTR| Pedals0_1_INTR| Pedals0_2_INTR| Pedals0_3_INTR| Pedals0_4_INTR| Pedals0_5_INTR| Pedals0_6_INTR| Pedals0_7_INTR))

#endif /* End Pins Pedals0_ALIASES_H */


/* [] END OF FILE */
