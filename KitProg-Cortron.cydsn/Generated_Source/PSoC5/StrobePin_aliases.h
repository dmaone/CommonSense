/*******************************************************************************
* File Name: StrobePin.h  
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

#if !defined(CY_PINS_StrobePin_ALIASES_H) /* Pins StrobePin_ALIASES_H */
#define CY_PINS_StrobePin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define StrobePin_0			(StrobePin__0__PC)
#define StrobePin_0_INTR	((uint16)((uint16)0x0001u << StrobePin__0__SHIFT))

#define StrobePin_1			(StrobePin__1__PC)
#define StrobePin_1_INTR	((uint16)((uint16)0x0001u << StrobePin__1__SHIFT))

#define StrobePin_2			(StrobePin__2__PC)
#define StrobePin_2_INTR	((uint16)((uint16)0x0001u << StrobePin__2__SHIFT))

#define StrobePin_INTR_ALL	 ((uint16)(StrobePin_0_INTR| StrobePin_1_INTR| StrobePin_2_INTR))

#endif /* End Pins StrobePin_ALIASES_H */


/* [] END OF FILE */
