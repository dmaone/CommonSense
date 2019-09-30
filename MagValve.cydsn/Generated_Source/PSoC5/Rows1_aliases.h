/*******************************************************************************
* File Name: Rows1.h  
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

#if !defined(CY_PINS_Rows1_ALIASES_H) /* Pins Rows1_ALIASES_H */
#define CY_PINS_Rows1_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Rows1_0			(Rows1__0__PC)
#define Rows1_0_INTR	((uint16)((uint16)0x0001u << Rows1__0__SHIFT))

#define Rows1_1			(Rows1__1__PC)
#define Rows1_1_INTR	((uint16)((uint16)0x0001u << Rows1__1__SHIFT))

#define Rows1_2			(Rows1__2__PC)
#define Rows1_2_INTR	((uint16)((uint16)0x0001u << Rows1__2__SHIFT))

#define Rows1_3			(Rows1__3__PC)
#define Rows1_3_INTR	((uint16)((uint16)0x0001u << Rows1__3__SHIFT))

#define Rows1_4			(Rows1__4__PC)
#define Rows1_4_INTR	((uint16)((uint16)0x0001u << Rows1__4__SHIFT))

#define Rows1_5			(Rows1__5__PC)
#define Rows1_5_INTR	((uint16)((uint16)0x0001u << Rows1__5__SHIFT))

#define Rows1_6			(Rows1__6__PC)
#define Rows1_6_INTR	((uint16)((uint16)0x0001u << Rows1__6__SHIFT))

#define Rows1_7			(Rows1__7__PC)
#define Rows1_7_INTR	((uint16)((uint16)0x0001u << Rows1__7__SHIFT))

#define Rows1_INTR_ALL	 ((uint16)(Rows1_0_INTR| Rows1_1_INTR| Rows1_2_INTR| Rows1_3_INTR| Rows1_4_INTR| Rows1_5_INTR| Rows1_6_INTR| Rows1_7_INTR))

#endif /* End Pins Rows1_ALIASES_H */


/* [] END OF FILE */
