/*******************************************************************************
* File Name: Pins.h  
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

#if !defined(CY_PINS_Pins_ALIASES_H) /* Pins Pins_ALIASES_H */
#define CY_PINS_Pins_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Pins_0			(Pins__0__PC)
#define Pins_0_INTR	((uint16)((uint16)0x0001u << Pins__0__SHIFT))

#define Pins_1			(Pins__1__PC)
#define Pins_1_INTR	((uint16)((uint16)0x0001u << Pins__1__SHIFT))

#define Pins_2			(Pins__2__PC)
#define Pins_2_INTR	((uint16)((uint16)0x0001u << Pins__2__SHIFT))

#define Pins_3			(Pins__3__PC)
#define Pins_3_INTR	((uint16)((uint16)0x0001u << Pins__3__SHIFT))

#define Pins_4			(Pins__4__PC)
#define Pins_4_INTR	((uint16)((uint16)0x0001u << Pins__4__SHIFT))

#define Pins_5			(Pins__5__PC)
#define Pins_5_INTR	((uint16)((uint16)0x0001u << Pins__5__SHIFT))

#define Pins_6			(Pins__6__PC)
#define Pins_6_INTR	((uint16)((uint16)0x0001u << Pins__6__SHIFT))

#define Pins_7			(Pins__7__PC)
#define Pins_7_INTR	((uint16)((uint16)0x0001u << Pins__7__SHIFT))

#define Pins_INTR_ALL	 ((uint16)(Pins_0_INTR| Pins_1_INTR| Pins_2_INTR| Pins_3_INTR| Pins_4_INTR| Pins_5_INTR| Pins_6_INTR| Pins_7_INTR))
#define Pins_Button			(Pins__Button__PC)
#define Pins_Button_INTR	((uint16)((uint16)0x0001u << Pins__0__SHIFT))

#define Pins_LED			(Pins__LED__PC)
#define Pins_LED_INTR	((uint16)((uint16)0x0001u << Pins__1__SHIFT))

#define Pins_POR0			(Pins__POR0__PC)
#define Pins_POR0_INTR	((uint16)((uint16)0x0001u << Pins__2__SHIFT))

#define Pins_POR1			(Pins__POR1__PC)
#define Pins_POR1_INTR	((uint16)((uint16)0x0001u << Pins__3__SHIFT))

#define Pins_POR2			(Pins__POR2__PC)
#define Pins_POR2_INTR	((uint16)((uint16)0x0001u << Pins__4__SHIFT))

#define Pins_POR3			(Pins__POR3__PC)
#define Pins_POR3_INTR	((uint16)((uint16)0x0001u << Pins__5__SHIFT))

#define Pins_POR12			(Pins__POR12__PC)
#define Pins_POR12_INTR	((uint16)((uint16)0x0001u << Pins__6__SHIFT))

#define Pins_POR15			(Pins__POR15__PC)
#define Pins_POR15_INTR	((uint16)((uint16)0x0001u << Pins__7__SHIFT))

#endif /* End Pins Pins_ALIASES_H */


/* [] END OF FILE */
