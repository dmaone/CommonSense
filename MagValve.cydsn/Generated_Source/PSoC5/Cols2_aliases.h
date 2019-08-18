/*******************************************************************************
* File Name: Cols2.h  
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

#if !defined(CY_PINS_Cols2_ALIASES_H) /* Pins Cols2_ALIASES_H */
#define CY_PINS_Cols2_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define Cols2_0			(Cols2__0__PC)
#define Cols2_0_INTR	((uint16)((uint16)0x0001u << Cols2__0__SHIFT))

#define Cols2_1			(Cols2__1__PC)
#define Cols2_1_INTR	((uint16)((uint16)0x0001u << Cols2__1__SHIFT))

#define Cols2_2			(Cols2__2__PC)
#define Cols2_2_INTR	((uint16)((uint16)0x0001u << Cols2__2__SHIFT))

#define Cols2_3			(Cols2__3__PC)
#define Cols2_3_INTR	((uint16)((uint16)0x0001u << Cols2__3__SHIFT))

#define Cols2_INTR_ALL	 ((uint16)(Cols2_0_INTR| Cols2_1_INTR| Cols2_2_INTR| Cols2_3_INTR))

#endif /* End Pins Cols2_ALIASES_H */


/* [] END OF FILE */
