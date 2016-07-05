/*******************************************************************************
* File Name: EvenSense.h  
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

#if !defined(CY_PINS_EvenSense_ALIASES_H) /* Pins EvenSense_ALIASES_H */
#define CY_PINS_EvenSense_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define EvenSense_0			(EvenSense__0__PC)
#define EvenSense_0_INTR	((uint16)((uint16)0x0001u << EvenSense__0__SHIFT))

#define EvenSense_1			(EvenSense__1__PC)
#define EvenSense_1_INTR	((uint16)((uint16)0x0001u << EvenSense__1__SHIFT))

#define EvenSense_2			(EvenSense__2__PC)
#define EvenSense_2_INTR	((uint16)((uint16)0x0001u << EvenSense__2__SHIFT))

#define EvenSense_3			(EvenSense__3__PC)
#define EvenSense_3_INTR	((uint16)((uint16)0x0001u << EvenSense__3__SHIFT))

#define EvenSense_4			(EvenSense__4__PC)
#define EvenSense_4_INTR	((uint16)((uint16)0x0001u << EvenSense__4__SHIFT))

#define EvenSense_5			(EvenSense__5__PC)
#define EvenSense_5_INTR	((uint16)((uint16)0x0001u << EvenSense__5__SHIFT))

#define EvenSense_6			(EvenSense__6__PC)
#define EvenSense_6_INTR	((uint16)((uint16)0x0001u << EvenSense__6__SHIFT))

#define EvenSense_7			(EvenSense__7__PC)
#define EvenSense_7_INTR	((uint16)((uint16)0x0001u << EvenSense__7__SHIFT))

#define EvenSense_INTR_ALL	 ((uint16)(EvenSense_0_INTR| EvenSense_1_INTR| EvenSense_2_INTR| EvenSense_3_INTR| EvenSense_4_INTR| EvenSense_5_INTR| EvenSense_6_INTR| EvenSense_7_INTR))

#endif /* End Pins EvenSense_ALIASES_H */


/* [] END OF FILE */
