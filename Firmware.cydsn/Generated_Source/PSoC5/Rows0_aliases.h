/*******************************************************************************
* File Name: Rows0.h  
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

#if !defined(CY_PINS_Rows0_ALIASES_H) /* Pins Rows0_ALIASES_H */
#define CY_PINS_Rows0_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Rows0_0			(Rows0__0__PC)
#define Rows0_0_INTR	((uint16)((uint16)0x0001u << Rows0__0__SHIFT))

#define Rows0_1			(Rows0__1__PC)
#define Rows0_1_INTR	((uint16)((uint16)0x0001u << Rows0__1__SHIFT))

#define Rows0_2			(Rows0__2__PC)
#define Rows0_2_INTR	((uint16)((uint16)0x0001u << Rows0__2__SHIFT))

#define Rows0_3			(Rows0__3__PC)
#define Rows0_3_INTR	((uint16)((uint16)0x0001u << Rows0__3__SHIFT))

#define Rows0_4			(Rows0__4__PC)
#define Rows0_4_INTR	((uint16)((uint16)0x0001u << Rows0__4__SHIFT))

#define Rows0_5			(Rows0__5__PC)
#define Rows0_5_INTR	((uint16)((uint16)0x0001u << Rows0__5__SHIFT))

#define Rows0_6			(Rows0__6__PC)
#define Rows0_6_INTR	((uint16)((uint16)0x0001u << Rows0__6__SHIFT))

#define Rows0_7			(Rows0__7__PC)
#define Rows0_7_INTR	((uint16)((uint16)0x0001u << Rows0__7__SHIFT))

#define Rows0_INTR_ALL	 ((uint16)(Rows0_0_INTR| Rows0_1_INTR| Rows0_2_INTR| Rows0_3_INTR| Rows0_4_INTR| Rows0_5_INTR| Rows0_6_INTR| Rows0_7_INTR))

#endif /* End Pins Rows0_ALIASES_H */


/* [] END OF FILE */
