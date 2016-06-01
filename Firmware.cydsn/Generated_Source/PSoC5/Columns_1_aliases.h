/*******************************************************************************
* File Name: Columns_1.h  
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

#if !defined(CY_PINS_Columns_1_ALIASES_H) /* Pins Columns_1_ALIASES_H */
#define CY_PINS_Columns_1_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Columns_1_0			(Columns_1__0__PC)
#define Columns_1_0_INTR	((uint16)((uint16)0x0001u << Columns_1__0__SHIFT))

#define Columns_1_1			(Columns_1__1__PC)
#define Columns_1_1_INTR	((uint16)((uint16)0x0001u << Columns_1__1__SHIFT))

#define Columns_1_2			(Columns_1__2__PC)
#define Columns_1_2_INTR	((uint16)((uint16)0x0001u << Columns_1__2__SHIFT))

#define Columns_1_3			(Columns_1__3__PC)
#define Columns_1_3_INTR	((uint16)((uint16)0x0001u << Columns_1__3__SHIFT))

#define Columns_1_4			(Columns_1__4__PC)
#define Columns_1_4_INTR	((uint16)((uint16)0x0001u << Columns_1__4__SHIFT))

#define Columns_1_5			(Columns_1__5__PC)
#define Columns_1_5_INTR	((uint16)((uint16)0x0001u << Columns_1__5__SHIFT))

#define Columns_1_6			(Columns_1__6__PC)
#define Columns_1_6_INTR	((uint16)((uint16)0x0001u << Columns_1__6__SHIFT))

#define Columns_1_7			(Columns_1__7__PC)
#define Columns_1_7_INTR	((uint16)((uint16)0x0001u << Columns_1__7__SHIFT))

#define Columns_1_INTR_ALL	 ((uint16)(Columns_1_0_INTR| Columns_1_1_INTR| Columns_1_2_INTR| Columns_1_3_INTR| Columns_1_4_INTR| Columns_1_5_INTR| Columns_1_6_INTR| Columns_1_7_INTR))

#endif /* End Pins Columns_1_ALIASES_H */


/* [] END OF FILE */
