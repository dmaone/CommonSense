/*******************************************************************************
* File Name: Columns0.h  
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

#if !defined(CY_PINS_Columns0_ALIASES_H) /* Pins Columns0_ALIASES_H */
#define CY_PINS_Columns0_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Columns0_0			(Columns0__0__PC)
#define Columns0_0_INTR	((uint16)((uint16)0x0001u << Columns0__0__SHIFT))

#define Columns0_1			(Columns0__1__PC)
#define Columns0_1_INTR	((uint16)((uint16)0x0001u << Columns0__1__SHIFT))

#define Columns0_2			(Columns0__2__PC)
#define Columns0_2_INTR	((uint16)((uint16)0x0001u << Columns0__2__SHIFT))

#define Columns0_3			(Columns0__3__PC)
#define Columns0_3_INTR	((uint16)((uint16)0x0001u << Columns0__3__SHIFT))

#define Columns0_4			(Columns0__4__PC)
#define Columns0_4_INTR	((uint16)((uint16)0x0001u << Columns0__4__SHIFT))

#define Columns0_5			(Columns0__5__PC)
#define Columns0_5_INTR	((uint16)((uint16)0x0001u << Columns0__5__SHIFT))

#define Columns0_6			(Columns0__6__PC)
#define Columns0_6_INTR	((uint16)((uint16)0x0001u << Columns0__6__SHIFT))

#define Columns0_7			(Columns0__7__PC)
#define Columns0_7_INTR	((uint16)((uint16)0x0001u << Columns0__7__SHIFT))

#define Columns0_INTR_ALL	 ((uint16)(Columns0_0_INTR| Columns0_1_INTR| Columns0_2_INTR| Columns0_3_INTR| Columns0_4_INTR| Columns0_5_INTR| Columns0_6_INTR| Columns0_7_INTR))

#endif /* End Pins Columns0_ALIASES_H */


/* [] END OF FILE */
