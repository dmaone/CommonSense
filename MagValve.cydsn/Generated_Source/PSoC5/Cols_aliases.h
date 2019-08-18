/*******************************************************************************
* File Name: Cols.h  
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

#if !defined(CY_PINS_Cols_ALIASES_H) /* Pins Cols_ALIASES_H */
#define CY_PINS_Cols_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Cols_0			(Cols__0__PC)
#define Cols_0_INTR	((uint16)((uint16)0x0001u << Cols__0__SHIFT))

#define Cols_1			(Cols__1__PC)
#define Cols_1_INTR	((uint16)((uint16)0x0001u << Cols__1__SHIFT))

#define Cols_2			(Cols__2__PC)
#define Cols_2_INTR	((uint16)((uint16)0x0001u << Cols__2__SHIFT))

#define Cols_3			(Cols__3__PC)
#define Cols_3_INTR	((uint16)((uint16)0x0001u << Cols__3__SHIFT))

#define Cols_INTR_ALL	 ((uint16)(Cols_0_INTR| Cols_1_INTR| Cols_2_INTR| Cols_3_INTR))

#endif /* End Pins Cols_ALIASES_H */


/* [] END OF FILE */
