/*******************************************************************************
* File Name: ExpHdr.h  
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

#if !defined(CY_PINS_ExpHdr_ALIASES_H) /* Pins ExpHdr_ALIASES_H */
#define CY_PINS_ExpHdr_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define ExpHdr_0			(ExpHdr__0__PC)
#define ExpHdr_0_INTR	((uint16)((uint16)0x0001u << ExpHdr__0__SHIFT))

#define ExpHdr_1			(ExpHdr__1__PC)
#define ExpHdr_1_INTR	((uint16)((uint16)0x0001u << ExpHdr__1__SHIFT))

#define ExpHdr_2			(ExpHdr__2__PC)
#define ExpHdr_2_INTR	((uint16)((uint16)0x0001u << ExpHdr__2__SHIFT))

#define ExpHdr_3			(ExpHdr__3__PC)
#define ExpHdr_3_INTR	((uint16)((uint16)0x0001u << ExpHdr__3__SHIFT))

#define ExpHdr_4			(ExpHdr__4__PC)
#define ExpHdr_4_INTR	((uint16)((uint16)0x0001u << ExpHdr__4__SHIFT))

#define ExpHdr_INTR_ALL	 ((uint16)(ExpHdr_0_INTR| ExpHdr_1_INTR| ExpHdr_2_INTR| ExpHdr_3_INTR| ExpHdr_4_INTR))

#endif /* End Pins ExpHdr_ALIASES_H */


/* [] END OF FILE */
