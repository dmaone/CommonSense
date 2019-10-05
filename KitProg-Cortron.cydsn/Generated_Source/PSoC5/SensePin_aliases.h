/*******************************************************************************
* File Name: SensePin.h  
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

#if !defined(CY_PINS_SensePin_ALIASES_H) /* Pins SensePin_ALIASES_H */
#define CY_PINS_SensePin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define SensePin_0			(SensePin__0__PC)
#define SensePin_0_INTR	((uint16)((uint16)0x0001u << SensePin__0__SHIFT))

#define SensePin_INTR_ALL	 ((uint16)(SensePin_0_INTR))

#endif /* End Pins SensePin_ALIASES_H */


/* [] END OF FILE */
