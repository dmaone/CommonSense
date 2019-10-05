/*******************************************************************************
* File Name: DrivePins.h  
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

#if !defined(CY_PINS_DrivePins_ALIASES_H) /* Pins DrivePins_ALIASES_H */
#define CY_PINS_DrivePins_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define DrivePins_0			(DrivePins__0__PC)
#define DrivePins_0_INTR	((uint16)((uint16)0x0001u << DrivePins__0__SHIFT))

#define DrivePins_1			(DrivePins__1__PC)
#define DrivePins_1_INTR	((uint16)((uint16)0x0001u << DrivePins__1__SHIFT))

#define DrivePins_2			(DrivePins__2__PC)
#define DrivePins_2_INTR	((uint16)((uint16)0x0001u << DrivePins__2__SHIFT))

#define DrivePins_3			(DrivePins__3__PC)
#define DrivePins_3_INTR	((uint16)((uint16)0x0001u << DrivePins__3__SHIFT))

#define DrivePins_4			(DrivePins__4__PC)
#define DrivePins_4_INTR	((uint16)((uint16)0x0001u << DrivePins__4__SHIFT))

#define DrivePins_5			(DrivePins__5__PC)
#define DrivePins_5_INTR	((uint16)((uint16)0x0001u << DrivePins__5__SHIFT))

#define DrivePins_6			(DrivePins__6__PC)
#define DrivePins_6_INTR	((uint16)((uint16)0x0001u << DrivePins__6__SHIFT))

#define DrivePins_7			(DrivePins__7__PC)
#define DrivePins_7_INTR	((uint16)((uint16)0x0001u << DrivePins__7__SHIFT))

#define DrivePins_INTR_ALL	 ((uint16)(DrivePins_0_INTR| DrivePins_1_INTR| DrivePins_2_INTR| DrivePins_3_INTR| DrivePins_4_INTR| DrivePins_5_INTR| DrivePins_6_INTR| DrivePins_7_INTR))

#endif /* End Pins DrivePins_ALIASES_H */


/* [] END OF FILE */
