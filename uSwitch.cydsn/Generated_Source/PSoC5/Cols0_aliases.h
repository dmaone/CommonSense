/*******************************************************************************
* File Name: Cols0.h  
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

#if !defined(CY_PINS_Cols0_ALIASES_H) /* Pins Cols0_ALIASES_H */
#define CY_PINS_Cols0_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Cols0_0			(Cols0__0__PC)
#define Cols0_0_INTR	((uint16)((uint16)0x0001u << Cols0__0__SHIFT))

#define Cols0_1			(Cols0__1__PC)
#define Cols0_1_INTR	((uint16)((uint16)0x0001u << Cols0__1__SHIFT))

#define Cols0_2			(Cols0__2__PC)
#define Cols0_2_INTR	((uint16)((uint16)0x0001u << Cols0__2__SHIFT))

#define Cols0_3			(Cols0__3__PC)
#define Cols0_3_INTR	((uint16)((uint16)0x0001u << Cols0__3__SHIFT))

#define Cols0_4			(Cols0__4__PC)
#define Cols0_4_INTR	((uint16)((uint16)0x0001u << Cols0__4__SHIFT))

#define Cols0_5			(Cols0__5__PC)
#define Cols0_5_INTR	((uint16)((uint16)0x0001u << Cols0__5__SHIFT))

#define Cols0_6			(Cols0__6__PC)
#define Cols0_6_INTR	((uint16)((uint16)0x0001u << Cols0__6__SHIFT))

#define Cols0_7			(Cols0__7__PC)
#define Cols0_7_INTR	((uint16)((uint16)0x0001u << Cols0__7__SHIFT))

#define Cols0_INTR_ALL	 ((uint16)(Cols0_0_INTR| Cols0_1_INTR| Cols0_2_INTR| Cols0_3_INTR| Cols0_4_INTR| Cols0_5_INTR| Cols0_6_INTR| Cols0_7_INTR))

#endif /* End Pins Cols0_ALIASES_H */


/* [] END OF FILE */
