/*******************************************************************************
* File Name: Special.h  
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

#if !defined(CY_PINS_Special_ALIASES_H) /* Pins Special_ALIASES_H */
#define CY_PINS_Special_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Special_0			(Special__0__PC)
#define Special_0_INTR	((uint16)((uint16)0x0001u << Special__0__SHIFT))

#define Special_1			(Special__1__PC)
#define Special_1_INTR	((uint16)((uint16)0x0001u << Special__1__SHIFT))

#define Special_2			(Special__2__PC)
#define Special_2_INTR	((uint16)((uint16)0x0001u << Special__2__SHIFT))

#define Special_3			(Special__3__PC)
#define Special_3_INTR	((uint16)((uint16)0x0001u << Special__3__SHIFT))

#define Special_4			(Special__4__PC)
#define Special_4_INTR	((uint16)((uint16)0x0001u << Special__4__SHIFT))

#define Special_5			(Special__5__PC)
#define Special_5_INTR	((uint16)((uint16)0x0001u << Special__5__SHIFT))

#define Special_6			(Special__6__PC)
#define Special_6_INTR	((uint16)((uint16)0x0001u << Special__6__SHIFT))

#define Special_INTR_ALL	 ((uint16)(Special_0_INTR| Special_1_INTR| Special_2_INTR| Special_3_INTR| Special_4_INTR| Special_5_INTR| Special_6_INTR))

#endif /* End Pins Special_ALIASES_H */


/* [] END OF FILE */
