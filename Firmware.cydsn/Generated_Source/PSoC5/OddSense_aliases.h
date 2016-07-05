/*******************************************************************************
* File Name: OddSense.h  
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

#if !defined(CY_PINS_OddSense_ALIASES_H) /* Pins OddSense_ALIASES_H */
#define CY_PINS_OddSense_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define OddSense_0			(OddSense__0__PC)
#define OddSense_0_INTR	((uint16)((uint16)0x0001u << OddSense__0__SHIFT))

#define OddSense_1			(OddSense__1__PC)
#define OddSense_1_INTR	((uint16)((uint16)0x0001u << OddSense__1__SHIFT))

#define OddSense_2			(OddSense__2__PC)
#define OddSense_2_INTR	((uint16)((uint16)0x0001u << OddSense__2__SHIFT))

#define OddSense_3			(OddSense__3__PC)
#define OddSense_3_INTR	((uint16)((uint16)0x0001u << OddSense__3__SHIFT))

#define OddSense_4			(OddSense__4__PC)
#define OddSense_4_INTR	((uint16)((uint16)0x0001u << OddSense__4__SHIFT))

#define OddSense_5			(OddSense__5__PC)
#define OddSense_5_INTR	((uint16)((uint16)0x0001u << OddSense__5__SHIFT))

#define OddSense_6			(OddSense__6__PC)
#define OddSense_6_INTR	((uint16)((uint16)0x0001u << OddSense__6__SHIFT))

#define OddSense_7			(OddSense__7__PC)
#define OddSense_7_INTR	((uint16)((uint16)0x0001u << OddSense__7__SHIFT))

#define OddSense_INTR_ALL	 ((uint16)(OddSense_0_INTR| OddSense_1_INTR| OddSense_2_INTR| OddSense_3_INTR| OddSense_4_INTR| OddSense_5_INTR| OddSense_6_INTR| OddSense_7_INTR))

#endif /* End Pins OddSense_ALIASES_H */


/* [] END OF FILE */
