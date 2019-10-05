/*******************************************************************************
* File Name: ColPins.h  
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

#if !defined(CY_PINS_ColPins_ALIASES_H) /* Pins ColPins_ALIASES_H */
#define CY_PINS_ColPins_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define ColPins_0			(ColPins__0__PC)
#define ColPins_0_INTR	((uint16)((uint16)0x0001u << ColPins__0__SHIFT))

#define ColPins_1			(ColPins__1__PC)
#define ColPins_1_INTR	((uint16)((uint16)0x0001u << ColPins__1__SHIFT))

#define ColPins_2			(ColPins__2__PC)
#define ColPins_2_INTR	((uint16)((uint16)0x0001u << ColPins__2__SHIFT))

#define ColPins_3			(ColPins__3__PC)
#define ColPins_3_INTR	((uint16)((uint16)0x0001u << ColPins__3__SHIFT))

#define ColPins_INTR_ALL	 ((uint16)(ColPins_0_INTR| ColPins_1_INTR| ColPins_2_INTR| ColPins_3_INTR))

#endif /* End Pins ColPins_ALIASES_H */


/* [] END OF FILE */
