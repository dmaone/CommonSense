/*******************************************************************************
* File Name: Cols3.h  
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

#if !defined(CY_PINS_Cols3_ALIASES_H) /* Pins Cols3_ALIASES_H */
#define CY_PINS_Cols3_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Cols3_0			(Cols3__0__PC)
#define Cols3_0_INTR	((uint16)((uint16)0x0001u << Cols3__0__SHIFT))

#define Cols3_1			(Cols3__1__PC)
#define Cols3_1_INTR	((uint16)((uint16)0x0001u << Cols3__1__SHIFT))

#define Cols3_2			(Cols3__2__PC)
#define Cols3_2_INTR	((uint16)((uint16)0x0001u << Cols3__2__SHIFT))

#define Cols3_3			(Cols3__3__PC)
#define Cols3_3_INTR	((uint16)((uint16)0x0001u << Cols3__3__SHIFT))

#define Cols3_INTR_ALL	 ((uint16)(Cols3_0_INTR| Cols3_1_INTR| Cols3_2_INTR| Cols3_3_INTR))

#endif /* End Pins Cols3_ALIASES_H */


/* [] END OF FILE */
