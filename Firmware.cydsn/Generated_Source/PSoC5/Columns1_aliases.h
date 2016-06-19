/*******************************************************************************
* File Name: Columns1.h  
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

#if !defined(CY_PINS_Columns1_ALIASES_H) /* Pins Columns1_ALIASES_H */
#define CY_PINS_Columns1_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Columns1_0			(Columns1__0__PC)
#define Columns1_0_INTR	((uint16)((uint16)0x0001u << Columns1__0__SHIFT))

#define Columns1_1			(Columns1__1__PC)
#define Columns1_1_INTR	((uint16)((uint16)0x0001u << Columns1__1__SHIFT))

#define Columns1_2			(Columns1__2__PC)
#define Columns1_2_INTR	((uint16)((uint16)0x0001u << Columns1__2__SHIFT))

#define Columns1_3			(Columns1__3__PC)
#define Columns1_3_INTR	((uint16)((uint16)0x0001u << Columns1__3__SHIFT))

#define Columns1_4			(Columns1__4__PC)
#define Columns1_4_INTR	((uint16)((uint16)0x0001u << Columns1__4__SHIFT))

#define Columns1_5			(Columns1__5__PC)
#define Columns1_5_INTR	((uint16)((uint16)0x0001u << Columns1__5__SHIFT))

#define Columns1_6			(Columns1__6__PC)
#define Columns1_6_INTR	((uint16)((uint16)0x0001u << Columns1__6__SHIFT))

#define Columns1_7			(Columns1__7__PC)
#define Columns1_7_INTR	((uint16)((uint16)0x0001u << Columns1__7__SHIFT))

#define Columns1_INTR_ALL	 ((uint16)(Columns1_0_INTR| Columns1_1_INTR| Columns1_2_INTR| Columns1_3_INTR| Columns1_4_INTR| Columns1_5_INTR| Columns1_6_INTR| Columns1_7_INTR))

#endif /* End Pins Columns1_ALIASES_H */


/* [] END OF FILE */
