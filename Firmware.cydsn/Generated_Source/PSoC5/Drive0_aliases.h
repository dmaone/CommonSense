/*******************************************************************************
* File Name: Drive0.h  
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

#if !defined(CY_PINS_Drive0_ALIASES_H) /* Pins Drive0_ALIASES_H */
#define CY_PINS_Drive0_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Drive0_0			(Drive0__0__PC)
#define Drive0_0_INTR	((uint16)((uint16)0x0001u << Drive0__0__SHIFT))

#define Drive0_1			(Drive0__1__PC)
#define Drive0_1_INTR	((uint16)((uint16)0x0001u << Drive0__1__SHIFT))

#define Drive0_2			(Drive0__2__PC)
#define Drive0_2_INTR	((uint16)((uint16)0x0001u << Drive0__2__SHIFT))

#define Drive0_3			(Drive0__3__PC)
#define Drive0_3_INTR	((uint16)((uint16)0x0001u << Drive0__3__SHIFT))

#define Drive0_4			(Drive0__4__PC)
#define Drive0_4_INTR	((uint16)((uint16)0x0001u << Drive0__4__SHIFT))

#define Drive0_5			(Drive0__5__PC)
#define Drive0_5_INTR	((uint16)((uint16)0x0001u << Drive0__5__SHIFT))

#define Drive0_6			(Drive0__6__PC)
#define Drive0_6_INTR	((uint16)((uint16)0x0001u << Drive0__6__SHIFT))

#define Drive0_7			(Drive0__7__PC)
#define Drive0_7_INTR	((uint16)((uint16)0x0001u << Drive0__7__SHIFT))

#define Drive0_INTR_ALL	 ((uint16)(Drive0_0_INTR| Drive0_1_INTR| Drive0_2_INTR| Drive0_3_INTR| Drive0_4_INTR| Drive0_5_INTR| Drive0_6_INTR| Drive0_7_INTR))

#endif /* End Pins Drive0_ALIASES_H */


/* [] END OF FILE */
