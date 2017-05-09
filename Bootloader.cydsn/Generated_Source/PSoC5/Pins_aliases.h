/*******************************************************************************
* File Name: Pins.h  
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

#if !defined(CY_PINS_Pins_ALIASES_H) /* Pins Pins_ALIASES_H */
#define CY_PINS_Pins_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Pins_0			(Pins__0__PC)
#define Pins_0_INTR	((uint16)((uint16)0x0001u << Pins__0__SHIFT))

#define Pins_1			(Pins__1__PC)
#define Pins_1_INTR	((uint16)((uint16)0x0001u << Pins__1__SHIFT))

#define Pins_INTR_ALL	 ((uint16)(Pins_0_INTR| Pins_1_INTR))
#define Pins_Button			(Pins__Button__PC)
#define Pins_Button_INTR	((uint16)((uint16)0x0001u << Pins__0__SHIFT))

#define Pins_LED			(Pins__LED__PC)
#define Pins_LED_INTR	((uint16)((uint16)0x0001u << Pins__1__SHIFT))

#endif /* End Pins Pins_ALIASES_H */


/* [] END OF FILE */
