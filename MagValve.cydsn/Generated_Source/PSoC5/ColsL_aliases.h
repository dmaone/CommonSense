/*******************************************************************************
* File Name: ColsL.h  
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

#if !defined(CY_PINS_ColsL_ALIASES_H) /* Pins ColsL_ALIASES_H */
#define CY_PINS_ColsL_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define ColsL_0			(ColsL__0__PC)
#define ColsL_0_INTR	((uint16)((uint16)0x0001u << ColsL__0__SHIFT))

#define ColsL_1			(ColsL__1__PC)
#define ColsL_1_INTR	((uint16)((uint16)0x0001u << ColsL__1__SHIFT))

#define ColsL_2			(ColsL__2__PC)
#define ColsL_2_INTR	((uint16)((uint16)0x0001u << ColsL__2__SHIFT))

#define ColsL_3			(ColsL__3__PC)
#define ColsL_3_INTR	((uint16)((uint16)0x0001u << ColsL__3__SHIFT))

#define ColsL_4			(ColsL__4__PC)
#define ColsL_4_INTR	((uint16)((uint16)0x0001u << ColsL__4__SHIFT))

#define ColsL_5			(ColsL__5__PC)
#define ColsL_5_INTR	((uint16)((uint16)0x0001u << ColsL__5__SHIFT))

#define ColsL_6			(ColsL__6__PC)
#define ColsL_6_INTR	((uint16)((uint16)0x0001u << ColsL__6__SHIFT))

#define ColsL_7			(ColsL__7__PC)
#define ColsL_7_INTR	((uint16)((uint16)0x0001u << ColsL__7__SHIFT))

#define ColsL_INTR_ALL	 ((uint16)(ColsL_0_INTR| ColsL_1_INTR| ColsL_2_INTR| ColsL_3_INTR| ColsL_4_INTR| ColsL_5_INTR| ColsL_6_INTR| ColsL_7_INTR))

#endif /* End Pins ColsL_ALIASES_H */


/* [] END OF FILE */
