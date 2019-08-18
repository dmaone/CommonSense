/*******************************************************************************
* File Name: ColsR.h  
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

#if !defined(CY_PINS_ColsR_ALIASES_H) /* Pins ColsR_ALIASES_H */
#define CY_PINS_ColsR_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define ColsR_0			(ColsR__0__PC)
#define ColsR_0_INTR	((uint16)((uint16)0x0001u << ColsR__0__SHIFT))

#define ColsR_1			(ColsR__1__PC)
#define ColsR_1_INTR	((uint16)((uint16)0x0001u << ColsR__1__SHIFT))

#define ColsR_2			(ColsR__2__PC)
#define ColsR_2_INTR	((uint16)((uint16)0x0001u << ColsR__2__SHIFT))

#define ColsR_3			(ColsR__3__PC)
#define ColsR_3_INTR	((uint16)((uint16)0x0001u << ColsR__3__SHIFT))

#define ColsR_4			(ColsR__4__PC)
#define ColsR_4_INTR	((uint16)((uint16)0x0001u << ColsR__4__SHIFT))

#define ColsR_5			(ColsR__5__PC)
#define ColsR_5_INTR	((uint16)((uint16)0x0001u << ColsR__5__SHIFT))

#define ColsR_6			(ColsR__6__PC)
#define ColsR_6_INTR	((uint16)((uint16)0x0001u << ColsR__6__SHIFT))

#define ColsR_7			(ColsR__7__PC)
#define ColsR_7_INTR	((uint16)((uint16)0x0001u << ColsR__7__SHIFT))

#define ColsR_INTR_ALL	 ((uint16)(ColsR_0_INTR| ColsR_1_INTR| ColsR_2_INTR| ColsR_3_INTR| ColsR_4_INTR| ColsR_5_INTR| ColsR_6_INTR| ColsR_7_INTR))

#endif /* End Pins ColsR_ALIASES_H */


/* [] END OF FILE */
