/*******************************************************************************
* File Name: Cols1.h  
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

#if !defined(CY_PINS_Cols1_ALIASES_H) /* Pins Cols1_ALIASES_H */
#define CY_PINS_Cols1_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Cols1_0			(Cols1__0__PC)
#define Cols1_0_INTR	((uint16)((uint16)0x0001u << Cols1__0__SHIFT))

#define Cols1_1			(Cols1__1__PC)
#define Cols1_1_INTR	((uint16)((uint16)0x0001u << Cols1__1__SHIFT))

#define Cols1_2			(Cols1__2__PC)
#define Cols1_2_INTR	((uint16)((uint16)0x0001u << Cols1__2__SHIFT))

#define Cols1_3			(Cols1__3__PC)
#define Cols1_3_INTR	((uint16)((uint16)0x0001u << Cols1__3__SHIFT))

#define Cols1_INTR_ALL	 ((uint16)(Cols1_0_INTR| Cols1_1_INTR| Cols1_2_INTR| Cols1_3_INTR))

#endif /* End Pins Cols1_ALIASES_H */


/* [] END OF FILE */
