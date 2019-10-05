/*******************************************************************************
* File Name: RowPins.h  
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

#if !defined(CY_PINS_RowPins_ALIASES_H) /* Pins RowPins_ALIASES_H */
#define CY_PINS_RowPins_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define RowPins_0			(RowPins__0__PC)
#define RowPins_0_INTR	((uint16)((uint16)0x0001u << RowPins__0__SHIFT))

#define RowPins_1			(RowPins__1__PC)
#define RowPins_1_INTR	((uint16)((uint16)0x0001u << RowPins__1__SHIFT))

#define RowPins_2			(RowPins__2__PC)
#define RowPins_2_INTR	((uint16)((uint16)0x0001u << RowPins__2__SHIFT))

#define RowPins_3			(RowPins__3__PC)
#define RowPins_3_INTR	((uint16)((uint16)0x0001u << RowPins__3__SHIFT))

#define RowPins_INTR_ALL	 ((uint16)(RowPins_0_INTR| RowPins_1_INTR| RowPins_2_INTR| RowPins_3_INTR))

#endif /* End Pins RowPins_ALIASES_H */


/* [] END OF FILE */
