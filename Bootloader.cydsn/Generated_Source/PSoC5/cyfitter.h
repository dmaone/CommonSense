/*******************************************************************************
* File Name: cyfitter.h
* 
* PSoC Creator  4.4
*
* Description:
* 
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2020 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#ifndef INCLUDED_CYFITTER_H
#define INCLUDED_CYFITTER_H
#include "cydevice.h"
#include "cydevice_trm.h"

/* USB */
#define USB_arb_int__INTC_CLR_EN_REG CYREG_NVIC_CLRENA0
#define USB_arb_int__INTC_CLR_PD_REG CYREG_NVIC_CLRPEND0
#define USB_arb_int__INTC_MASK 0x400000u
#define USB_arb_int__INTC_NUMBER 22u
#define USB_arb_int__INTC_PRIOR_NUM 7u
#define USB_arb_int__INTC_PRIOR_REG CYREG_NVIC_PRI_22
#define USB_arb_int__INTC_SET_EN_REG CYREG_NVIC_SETENA0
#define USB_arb_int__INTC_SET_PD_REG CYREG_NVIC_SETPEND0
#define USB_bus_reset__INTC_CLR_EN_REG CYREG_NVIC_CLRENA0
#define USB_bus_reset__INTC_CLR_PD_REG CYREG_NVIC_CLRPEND0
#define USB_bus_reset__INTC_MASK 0x800000u
#define USB_bus_reset__INTC_NUMBER 23u
#define USB_bus_reset__INTC_PRIOR_NUM 7u
#define USB_bus_reset__INTC_PRIOR_REG CYREG_NVIC_PRI_23
#define USB_bus_reset__INTC_SET_EN_REG CYREG_NVIC_SETENA0
#define USB_bus_reset__INTC_SET_PD_REG CYREG_NVIC_SETPEND0
#define USB_Dm__0__INTTYPE CYREG_PICU15_INTTYPE7
#define USB_Dm__0__MASK 0x80u
#define USB_Dm__0__PC CYREG_IO_PC_PRT15_7_6_PC1
#define USB_Dm__0__PORT 15u
#define USB_Dm__0__SHIFT 7u
#define USB_Dm__AG CYREG_PRT15_AG
#define USB_Dm__AMUX CYREG_PRT15_AMUX
#define USB_Dm__BIE CYREG_PRT15_BIE
#define USB_Dm__BIT_MASK CYREG_PRT15_BIT_MASK
#define USB_Dm__BYP CYREG_PRT15_BYP
#define USB_Dm__CTL CYREG_PRT15_CTL
#define USB_Dm__DM0 CYREG_PRT15_DM0
#define USB_Dm__DM1 CYREG_PRT15_DM1
#define USB_Dm__DM2 CYREG_PRT15_DM2
#define USB_Dm__DR CYREG_PRT15_DR
#define USB_Dm__INP_DIS CYREG_PRT15_INP_DIS
#define USB_Dm__INTTYPE_BASE CYDEV_PICU_INTTYPE_PICU15_BASE
#define USB_Dm__LCD_COM_SEG CYREG_PRT15_LCD_COM_SEG
#define USB_Dm__LCD_EN CYREG_PRT15_LCD_EN
#define USB_Dm__MASK 0x80u
#define USB_Dm__PORT 15u
#define USB_Dm__PRT CYREG_PRT15_PRT
#define USB_Dm__PRTDSI__CAPS_SEL CYREG_PRT15_CAPS_SEL
#define USB_Dm__PRTDSI__DBL_SYNC_IN CYREG_PRT15_DBL_SYNC_IN
#define USB_Dm__PRTDSI__OE_SEL0 CYREG_PRT15_OE_SEL0
#define USB_Dm__PRTDSI__OE_SEL1 CYREG_PRT15_OE_SEL1
#define USB_Dm__PRTDSI__OUT_SEL0 CYREG_PRT15_OUT_SEL0
#define USB_Dm__PRTDSI__OUT_SEL1 CYREG_PRT15_OUT_SEL1
#define USB_Dm__PRTDSI__SYNC_OUT CYREG_PRT15_SYNC_OUT
#define USB_Dm__PS CYREG_PRT15_PS
#define USB_Dm__SHIFT 7u
#define USB_Dm__SLW CYREG_PRT15_SLW
#define USB_Dp__0__INTTYPE CYREG_PICU15_INTTYPE6
#define USB_Dp__0__MASK 0x40u
#define USB_Dp__0__PC CYREG_IO_PC_PRT15_7_6_PC0
#define USB_Dp__0__PORT 15u
#define USB_Dp__0__SHIFT 6u
#define USB_Dp__AG CYREG_PRT15_AG
#define USB_Dp__AMUX CYREG_PRT15_AMUX
#define USB_Dp__BIE CYREG_PRT15_BIE
#define USB_Dp__BIT_MASK CYREG_PRT15_BIT_MASK
#define USB_Dp__BYP CYREG_PRT15_BYP
#define USB_Dp__CTL CYREG_PRT15_CTL
#define USB_Dp__DM0 CYREG_PRT15_DM0
#define USB_Dp__DM1 CYREG_PRT15_DM1
#define USB_Dp__DM2 CYREG_PRT15_DM2
#define USB_Dp__DR CYREG_PRT15_DR
#define USB_Dp__INP_DIS CYREG_PRT15_INP_DIS
#define USB_Dp__INTSTAT CYREG_PICU15_INTSTAT
#define USB_Dp__INTTYPE_BASE CYDEV_PICU_INTTYPE_PICU15_BASE
#define USB_Dp__LCD_COM_SEG CYREG_PRT15_LCD_COM_SEG
#define USB_Dp__LCD_EN CYREG_PRT15_LCD_EN
#define USB_Dp__MASK 0x40u
#define USB_Dp__PORT 15u
#define USB_Dp__PRT CYREG_PRT15_PRT
#define USB_Dp__PRTDSI__CAPS_SEL CYREG_PRT15_CAPS_SEL
#define USB_Dp__PRTDSI__DBL_SYNC_IN CYREG_PRT15_DBL_SYNC_IN
#define USB_Dp__PRTDSI__OE_SEL0 CYREG_PRT15_OE_SEL0
#define USB_Dp__PRTDSI__OE_SEL1 CYREG_PRT15_OE_SEL1
#define USB_Dp__PRTDSI__OUT_SEL0 CYREG_PRT15_OUT_SEL0
#define USB_Dp__PRTDSI__OUT_SEL1 CYREG_PRT15_OUT_SEL1
#define USB_Dp__PRTDSI__SYNC_OUT CYREG_PRT15_SYNC_OUT
#define USB_Dp__PS CYREG_PRT15_PS
#define USB_Dp__SHIFT 6u
#define USB_Dp__SLW CYREG_PRT15_SLW
#define USB_Dp__SNAP CYREG_PICU_15_SNAP_15
#define USB_dp_int__INTC_CLR_EN_REG CYREG_NVIC_CLRENA0
#define USB_dp_int__INTC_CLR_PD_REG CYREG_NVIC_CLRPEND0
#define USB_dp_int__INTC_MASK 0x1000u
#define USB_dp_int__INTC_NUMBER 12u
#define USB_dp_int__INTC_PRIOR_NUM 7u
#define USB_dp_int__INTC_PRIOR_REG CYREG_NVIC_PRI_12
#define USB_dp_int__INTC_SET_EN_REG CYREG_NVIC_SETENA0
#define USB_dp_int__INTC_SET_PD_REG CYREG_NVIC_SETPEND0
#define USB_ep_0__INTC_CLR_EN_REG CYREG_NVIC_CLRENA0
#define USB_ep_0__INTC_CLR_PD_REG CYREG_NVIC_CLRPEND0
#define USB_ep_0__INTC_MASK 0x1000000u
#define USB_ep_0__INTC_NUMBER 24u
#define USB_ep_0__INTC_PRIOR_NUM 7u
#define USB_ep_0__INTC_PRIOR_REG CYREG_NVIC_PRI_24
#define USB_ep_0__INTC_SET_EN_REG CYREG_NVIC_SETENA0
#define USB_ep_0__INTC_SET_PD_REG CYREG_NVIC_SETPEND0
#define USB_ep_1__INTC_CLR_EN_REG CYREG_NVIC_CLRENA0
#define USB_ep_1__INTC_CLR_PD_REG CYREG_NVIC_CLRPEND0
#define USB_ep_1__INTC_MASK 0x01u
#define USB_ep_1__INTC_NUMBER 0u
#define USB_ep_1__INTC_PRIOR_NUM 7u
#define USB_ep_1__INTC_PRIOR_REG CYREG_NVIC_PRI_0
#define USB_ep_1__INTC_SET_EN_REG CYREG_NVIC_SETENA0
#define USB_ep_1__INTC_SET_PD_REG CYREG_NVIC_SETPEND0
#define USB_ep_2__INTC_CLR_EN_REG CYREG_NVIC_CLRENA0
#define USB_ep_2__INTC_CLR_PD_REG CYREG_NVIC_CLRPEND0
#define USB_ep_2__INTC_MASK 0x02u
#define USB_ep_2__INTC_NUMBER 1u
#define USB_ep_2__INTC_PRIOR_NUM 7u
#define USB_ep_2__INTC_PRIOR_REG CYREG_NVIC_PRI_1
#define USB_ep_2__INTC_SET_EN_REG CYREG_NVIC_SETENA0
#define USB_ep_2__INTC_SET_PD_REG CYREG_NVIC_SETPEND0
#define USB_USB__ARB_CFG CYREG_USB_ARB_CFG
#define USB_USB__ARB_EP1_CFG CYREG_USB_ARB_EP1_CFG
#define USB_USB__ARB_EP1_INT_EN CYREG_USB_ARB_EP1_INT_EN
#define USB_USB__ARB_EP1_SR CYREG_USB_ARB_EP1_SR
#define USB_USB__ARB_EP2_CFG CYREG_USB_ARB_EP2_CFG
#define USB_USB__ARB_EP2_INT_EN CYREG_USB_ARB_EP2_INT_EN
#define USB_USB__ARB_EP2_SR CYREG_USB_ARB_EP2_SR
#define USB_USB__ARB_EP3_CFG CYREG_USB_ARB_EP3_CFG
#define USB_USB__ARB_EP3_INT_EN CYREG_USB_ARB_EP3_INT_EN
#define USB_USB__ARB_EP3_SR CYREG_USB_ARB_EP3_SR
#define USB_USB__ARB_EP4_CFG CYREG_USB_ARB_EP4_CFG
#define USB_USB__ARB_EP4_INT_EN CYREG_USB_ARB_EP4_INT_EN
#define USB_USB__ARB_EP4_SR CYREG_USB_ARB_EP4_SR
#define USB_USB__ARB_EP5_CFG CYREG_USB_ARB_EP5_CFG
#define USB_USB__ARB_EP5_INT_EN CYREG_USB_ARB_EP5_INT_EN
#define USB_USB__ARB_EP5_SR CYREG_USB_ARB_EP5_SR
#define USB_USB__ARB_EP6_CFG CYREG_USB_ARB_EP6_CFG
#define USB_USB__ARB_EP6_INT_EN CYREG_USB_ARB_EP6_INT_EN
#define USB_USB__ARB_EP6_SR CYREG_USB_ARB_EP6_SR
#define USB_USB__ARB_EP7_CFG CYREG_USB_ARB_EP7_CFG
#define USB_USB__ARB_EP7_INT_EN CYREG_USB_ARB_EP7_INT_EN
#define USB_USB__ARB_EP7_SR CYREG_USB_ARB_EP7_SR
#define USB_USB__ARB_EP8_CFG CYREG_USB_ARB_EP8_CFG
#define USB_USB__ARB_EP8_INT_EN CYREG_USB_ARB_EP8_INT_EN
#define USB_USB__ARB_EP8_SR CYREG_USB_ARB_EP8_SR
#define USB_USB__ARB_INT_EN CYREG_USB_ARB_INT_EN
#define USB_USB__ARB_INT_SR CYREG_USB_ARB_INT_SR
#define USB_USB__ARB_RW1_DR CYREG_USB_ARB_RW1_DR
#define USB_USB__ARB_RW1_RA CYREG_USB_ARB_RW1_RA
#define USB_USB__ARB_RW1_RA_MSB CYREG_USB_ARB_RW1_RA_MSB
#define USB_USB__ARB_RW1_WA CYREG_USB_ARB_RW1_WA
#define USB_USB__ARB_RW1_WA_MSB CYREG_USB_ARB_RW1_WA_MSB
#define USB_USB__ARB_RW2_DR CYREG_USB_ARB_RW2_DR
#define USB_USB__ARB_RW2_RA CYREG_USB_ARB_RW2_RA
#define USB_USB__ARB_RW2_RA_MSB CYREG_USB_ARB_RW2_RA_MSB
#define USB_USB__ARB_RW2_WA CYREG_USB_ARB_RW2_WA
#define USB_USB__ARB_RW2_WA_MSB CYREG_USB_ARB_RW2_WA_MSB
#define USB_USB__ARB_RW3_DR CYREG_USB_ARB_RW3_DR
#define USB_USB__ARB_RW3_RA CYREG_USB_ARB_RW3_RA
#define USB_USB__ARB_RW3_RA_MSB CYREG_USB_ARB_RW3_RA_MSB
#define USB_USB__ARB_RW3_WA CYREG_USB_ARB_RW3_WA
#define USB_USB__ARB_RW3_WA_MSB CYREG_USB_ARB_RW3_WA_MSB
#define USB_USB__ARB_RW4_DR CYREG_USB_ARB_RW4_DR
#define USB_USB__ARB_RW4_RA CYREG_USB_ARB_RW4_RA
#define USB_USB__ARB_RW4_RA_MSB CYREG_USB_ARB_RW4_RA_MSB
#define USB_USB__ARB_RW4_WA CYREG_USB_ARB_RW4_WA
#define USB_USB__ARB_RW4_WA_MSB CYREG_USB_ARB_RW4_WA_MSB
#define USB_USB__ARB_RW5_DR CYREG_USB_ARB_RW5_DR
#define USB_USB__ARB_RW5_RA CYREG_USB_ARB_RW5_RA
#define USB_USB__ARB_RW5_RA_MSB CYREG_USB_ARB_RW5_RA_MSB
#define USB_USB__ARB_RW5_WA CYREG_USB_ARB_RW5_WA
#define USB_USB__ARB_RW5_WA_MSB CYREG_USB_ARB_RW5_WA_MSB
#define USB_USB__ARB_RW6_DR CYREG_USB_ARB_RW6_DR
#define USB_USB__ARB_RW6_RA CYREG_USB_ARB_RW6_RA
#define USB_USB__ARB_RW6_RA_MSB CYREG_USB_ARB_RW6_RA_MSB
#define USB_USB__ARB_RW6_WA CYREG_USB_ARB_RW6_WA
#define USB_USB__ARB_RW6_WA_MSB CYREG_USB_ARB_RW6_WA_MSB
#define USB_USB__ARB_RW7_DR CYREG_USB_ARB_RW7_DR
#define USB_USB__ARB_RW7_RA CYREG_USB_ARB_RW7_RA
#define USB_USB__ARB_RW7_RA_MSB CYREG_USB_ARB_RW7_RA_MSB
#define USB_USB__ARB_RW7_WA CYREG_USB_ARB_RW7_WA
#define USB_USB__ARB_RW7_WA_MSB CYREG_USB_ARB_RW7_WA_MSB
#define USB_USB__ARB_RW8_DR CYREG_USB_ARB_RW8_DR
#define USB_USB__ARB_RW8_RA CYREG_USB_ARB_RW8_RA
#define USB_USB__ARB_RW8_RA_MSB CYREG_USB_ARB_RW8_RA_MSB
#define USB_USB__ARB_RW8_WA CYREG_USB_ARB_RW8_WA
#define USB_USB__ARB_RW8_WA_MSB CYREG_USB_ARB_RW8_WA_MSB
#define USB_USB__BUF_SIZE CYREG_USB_BUF_SIZE
#define USB_USB__BUS_RST_CNT CYREG_USB_BUS_RST_CNT
#define USB_USB__CR0 CYREG_USB_CR0
#define USB_USB__CR1 CYREG_USB_CR1
#define USB_USB__CWA CYREG_USB_CWA
#define USB_USB__CWA_MSB CYREG_USB_CWA_MSB
#define USB_USB__DMA_THRES CYREG_USB_DMA_THRES
#define USB_USB__DMA_THRES_MSB CYREG_USB_DMA_THRES_MSB
#define USB_USB__DYN_RECONFIG CYREG_USB_DYN_RECONFIG
#define USB_USB__EP_ACTIVE CYREG_USB_EP_ACTIVE
#define USB_USB__EP_TYPE CYREG_USB_EP_TYPE
#define USB_USB__EP0_CNT CYREG_USB_EP0_CNT
#define USB_USB__EP0_CR CYREG_USB_EP0_CR
#define USB_USB__EP0_DR0 CYREG_USB_EP0_DR0
#define USB_USB__EP0_DR1 CYREG_USB_EP0_DR1
#define USB_USB__EP0_DR2 CYREG_USB_EP0_DR2
#define USB_USB__EP0_DR3 CYREG_USB_EP0_DR3
#define USB_USB__EP0_DR4 CYREG_USB_EP0_DR4
#define USB_USB__EP0_DR5 CYREG_USB_EP0_DR5
#define USB_USB__EP0_DR6 CYREG_USB_EP0_DR6
#define USB_USB__EP0_DR7 CYREG_USB_EP0_DR7
#define USB_USB__MEM_DATA CYREG_USB_MEM_DATA_MBASE
#define USB_USB__PM_ACT_CFG CYREG_PM_ACT_CFG5
#define USB_USB__PM_ACT_MSK 0x01u
#define USB_USB__PM_STBY_CFG CYREG_PM_STBY_CFG5
#define USB_USB__PM_STBY_MSK 0x01u
#define USB_USB__SIE_EP_INT_EN CYREG_USB_SIE_EP_INT_EN
#define USB_USB__SIE_EP_INT_SR CYREG_USB_SIE_EP_INT_SR
#define USB_USB__SIE_EP1_CNT0 CYREG_USB_SIE_EP1_CNT0
#define USB_USB__SIE_EP1_CNT1 CYREG_USB_SIE_EP1_CNT1
#define USB_USB__SIE_EP1_CR0 CYREG_USB_SIE_EP1_CR0
#define USB_USB__SIE_EP2_CNT0 CYREG_USB_SIE_EP2_CNT0
#define USB_USB__SIE_EP2_CNT1 CYREG_USB_SIE_EP2_CNT1
#define USB_USB__SIE_EP2_CR0 CYREG_USB_SIE_EP2_CR0
#define USB_USB__SIE_EP3_CNT0 CYREG_USB_SIE_EP3_CNT0
#define USB_USB__SIE_EP3_CNT1 CYREG_USB_SIE_EP3_CNT1
#define USB_USB__SIE_EP3_CR0 CYREG_USB_SIE_EP3_CR0
#define USB_USB__SIE_EP4_CNT0 CYREG_USB_SIE_EP4_CNT0
#define USB_USB__SIE_EP4_CNT1 CYREG_USB_SIE_EP4_CNT1
#define USB_USB__SIE_EP4_CR0 CYREG_USB_SIE_EP4_CR0
#define USB_USB__SIE_EP5_CNT0 CYREG_USB_SIE_EP5_CNT0
#define USB_USB__SIE_EP5_CNT1 CYREG_USB_SIE_EP5_CNT1
#define USB_USB__SIE_EP5_CR0 CYREG_USB_SIE_EP5_CR0
#define USB_USB__SIE_EP6_CNT0 CYREG_USB_SIE_EP6_CNT0
#define USB_USB__SIE_EP6_CNT1 CYREG_USB_SIE_EP6_CNT1
#define USB_USB__SIE_EP6_CR0 CYREG_USB_SIE_EP6_CR0
#define USB_USB__SIE_EP7_CNT0 CYREG_USB_SIE_EP7_CNT0
#define USB_USB__SIE_EP7_CNT1 CYREG_USB_SIE_EP7_CNT1
#define USB_USB__SIE_EP7_CR0 CYREG_USB_SIE_EP7_CR0
#define USB_USB__SIE_EP8_CNT0 CYREG_USB_SIE_EP8_CNT0
#define USB_USB__SIE_EP8_CNT1 CYREG_USB_SIE_EP8_CNT1
#define USB_USB__SIE_EP8_CR0 CYREG_USB_SIE_EP8_CR0
#define USB_USB__SOF0 CYREG_USB_SOF0
#define USB_USB__SOF1 CYREG_USB_SOF1
#define USB_USB__USB_CLK_EN CYREG_USB_USB_CLK_EN
#define USB_USB__USBIO_CR0 CYREG_USB_USBIO_CR0
#define USB_USB__USBIO_CR1 CYREG_USB_USBIO_CR1
#define USB_VBUS__0__INTTYPE CYREG_PICU12_INTTYPE5
#define USB_VBUS__0__MASK 0x20u
#define USB_VBUS__0__PC CYREG_PRT12_PC5
#define USB_VBUS__0__PORT 12u
#define USB_VBUS__0__SHIFT 5u
#define USB_VBUS__AG CYREG_PRT12_AG
#define USB_VBUS__BIE CYREG_PRT12_BIE
#define USB_VBUS__BIT_MASK CYREG_PRT12_BIT_MASK
#define USB_VBUS__BYP CYREG_PRT12_BYP
#define USB_VBUS__DM0 CYREG_PRT12_DM0
#define USB_VBUS__DM1 CYREG_PRT12_DM1
#define USB_VBUS__DM2 CYREG_PRT12_DM2
#define USB_VBUS__DR CYREG_PRT12_DR
#define USB_VBUS__INP_DIS CYREG_PRT12_INP_DIS
#define USB_VBUS__INTTYPE_BASE CYDEV_PICU_INTTYPE_PICU12_BASE
#define USB_VBUS__MASK 0x20u
#define USB_VBUS__PORT 12u
#define USB_VBUS__PRT CYREG_PRT12_PRT
#define USB_VBUS__PRTDSI__DBL_SYNC_IN CYREG_PRT12_DBL_SYNC_IN
#define USB_VBUS__PRTDSI__OE_SEL0 CYREG_PRT12_OE_SEL0
#define USB_VBUS__PRTDSI__OE_SEL1 CYREG_PRT12_OE_SEL1
#define USB_VBUS__PRTDSI__OUT_SEL0 CYREG_PRT12_OUT_SEL0
#define USB_VBUS__PRTDSI__OUT_SEL1 CYREG_PRT12_OUT_SEL1
#define USB_VBUS__PRTDSI__SYNC_OUT CYREG_PRT12_SYNC_OUT
#define USB_VBUS__PS CYREG_PRT12_PS
#define USB_VBUS__SHIFT 5u
#define USB_VBUS__SIO_CFG CYREG_PRT12_SIO_CFG
#define USB_VBUS__SIO_DIFF CYREG_PRT12_SIO_DIFF
#define USB_VBUS__SIO_HYST_EN CYREG_PRT12_SIO_HYST_EN
#define USB_VBUS__SIO_REG_HIFREQ CYREG_PRT12_SIO_REG_HIFREQ
#define USB_VBUS__SLW CYREG_PRT12_SLW

/* Miscellaneous */
#define BCLK__BUS_CLK__HZ 24000000U
#define BCLK__BUS_CLK__KHZ 24000U
#define BCLK__BUS_CLK__MHZ 24U
#define CY_PROJECT_NAME "Bootloader"
#define CY_VERSION "PSoC Creator  4.4"
#define CYDEV_BOOTLOADER_APPLICATIONS 1u
#define CYDEV_BOOTLOADER_CHECKSUM_BASIC 0
#define CYDEV_BOOTLOADER_CHECKSUM_CRC 1
#define CYDEV_BOOTLOADER_IO_COMP_CUSTOM_IO 0
#define CyBtldr_Custom_Interface CYDEV_BOOTLOADER_IO_COMP_CUSTOM_IO
#define CYDEV_BOOTLOADER_IO_COMP_LAUNCHER_ONLY 1
#define CyBtldr_LAUNCHER_ONLY CYDEV_BOOTLOADER_IO_COMP_LAUNCHER_ONLY
#define CYDEV_BOOTLOADER_IO_COMP_USB 2
#define CyBtldr_USB CYDEV_BOOTLOADER_IO_COMP_USB
#define CYDEV_BOOTLOADER_IO_COMP CYDEV_BOOTLOADER_IO_COMP_USB
#define CYDEV_CHIP_DIE_LEOPARD 1u
#define CYDEV_CHIP_DIE_PSOC4A 26u
#define CYDEV_CHIP_DIE_PSOC5LP 2u
#define CYDEV_CHIP_DIE_PSOC5TM 3u
#define CYDEV_CHIP_DIE_TMA4 4u
#define CYDEV_CHIP_DIE_UNKNOWN 0u
#define CYDEV_CHIP_FAMILY_FM0P 5u
#define CYDEV_CHIP_FAMILY_FM3 6u
#define CYDEV_CHIP_FAMILY_FM4 7u
#define CYDEV_CHIP_FAMILY_PSOC3 1u
#define CYDEV_CHIP_FAMILY_PSOC4 2u
#define CYDEV_CHIP_FAMILY_PSOC5 3u
#define CYDEV_CHIP_FAMILY_PSOC6 4u
#define CYDEV_CHIP_FAMILY_UNKNOWN 0u
#define CYDEV_CHIP_FAMILY_USED CYDEV_CHIP_FAMILY_PSOC5
#define CYDEV_CHIP_JTAG_ID 0x2E161069u
#define CYDEV_CHIP_MEMBER_3A 1u
#define CYDEV_CHIP_MEMBER_4A 26u
#define CYDEV_CHIP_MEMBER_4AA 25u
#define CYDEV_CHIP_MEMBER_4AB 30u
#define CYDEV_CHIP_MEMBER_4AC 14u
#define CYDEV_CHIP_MEMBER_4AD 15u
#define CYDEV_CHIP_MEMBER_4AE 16u
#define CYDEV_CHIP_MEMBER_4D 20u
#define CYDEV_CHIP_MEMBER_4E 6u
#define CYDEV_CHIP_MEMBER_4F 27u
#define CYDEV_CHIP_MEMBER_4G 4u
#define CYDEV_CHIP_MEMBER_4H 24u
#define CYDEV_CHIP_MEMBER_4I 32u
#define CYDEV_CHIP_MEMBER_4J 21u
#define CYDEV_CHIP_MEMBER_4K 22u
#define CYDEV_CHIP_MEMBER_4L 31u
#define CYDEV_CHIP_MEMBER_4M 29u
#define CYDEV_CHIP_MEMBER_4N 11u
#define CYDEV_CHIP_MEMBER_4O 8u
#define CYDEV_CHIP_MEMBER_4P 28u
#define CYDEV_CHIP_MEMBER_4Q 17u
#define CYDEV_CHIP_MEMBER_4R 9u
#define CYDEV_CHIP_MEMBER_4S 12u
#define CYDEV_CHIP_MEMBER_4T 10u
#define CYDEV_CHIP_MEMBER_4U 5u
#define CYDEV_CHIP_MEMBER_4V 23u
#define CYDEV_CHIP_MEMBER_4W 13u
#define CYDEV_CHIP_MEMBER_4X 7u
#define CYDEV_CHIP_MEMBER_4Y 18u
#define CYDEV_CHIP_MEMBER_4Z 19u
#define CYDEV_CHIP_MEMBER_5A 3u
#define CYDEV_CHIP_MEMBER_5B 2u
#define CYDEV_CHIP_MEMBER_6A 33u
#define CYDEV_CHIP_MEMBER_FM3 37u
#define CYDEV_CHIP_MEMBER_FM4 38u
#define CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE1 34u
#define CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE2 35u
#define CYDEV_CHIP_MEMBER_PDL_FM0P_TYPE3 36u
#define CYDEV_CHIP_MEMBER_UNKNOWN 0u
#define CYDEV_CHIP_MEMBER_USED CYDEV_CHIP_MEMBER_5B
#define CYDEV_CHIP_DIE_EXPECT CYDEV_CHIP_MEMBER_USED
#define CYDEV_CHIP_DIE_ACTUAL CYDEV_CHIP_DIE_EXPECT
#define CYDEV_CHIP_REV_LEOPARD_ES1 0u
#define CYDEV_CHIP_REV_LEOPARD_ES2 1u
#define CYDEV_CHIP_REV_LEOPARD_ES3 3u
#define CYDEV_CHIP_REV_LEOPARD_PRODUCTION 3u
#define CYDEV_CHIP_REV_PSOC4A_ES0 17u
#define CYDEV_CHIP_REV_PSOC4A_PRODUCTION 17u
#define CYDEV_CHIP_REV_PSOC5LP_ES0 0u
#define CYDEV_CHIP_REV_PSOC5LP_PRODUCTION 0u
#define CYDEV_CHIP_REV_PSOC5TM_ES0 0u
#define CYDEV_CHIP_REV_PSOC5TM_ES1 1u
#define CYDEV_CHIP_REV_PSOC5TM_PRODUCTION 1u
#define CYDEV_CHIP_REV_TMA4_ES 17u
#define CYDEV_CHIP_REV_TMA4_ES2 33u
#define CYDEV_CHIP_REV_TMA4_PRODUCTION 17u
#define CYDEV_CHIP_REVISION_3A_ES1 0u
#define CYDEV_CHIP_REVISION_3A_ES2 1u
#define CYDEV_CHIP_REVISION_3A_ES3 3u
#define CYDEV_CHIP_REVISION_3A_PRODUCTION 3u
#define CYDEV_CHIP_REVISION_4A_ES0 17u
#define CYDEV_CHIP_REVISION_4A_PRODUCTION 17u
#define CYDEV_CHIP_REVISION_4AA_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AB_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AC_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AD_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4AE_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4D_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4E_CCG2_NO_USBPD 0u
#define CYDEV_CHIP_REVISION_4E_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4F_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4F_PRODUCTION_256DMA 0u
#define CYDEV_CHIP_REVISION_4F_PRODUCTION_256K 0u
#define CYDEV_CHIP_REVISION_4G_ES 17u
#define CYDEV_CHIP_REVISION_4G_ES2 33u
#define CYDEV_CHIP_REVISION_4G_PRODUCTION 17u
#define CYDEV_CHIP_REVISION_4H_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4I_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4J_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4K_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4L_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4M_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4N_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4O_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4P_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4Q_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4R_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4S_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4T_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4U_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4V_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4W_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4X_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4Y_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_4Z_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_5A_ES0 0u
#define CYDEV_CHIP_REVISION_5A_ES1 1u
#define CYDEV_CHIP_REVISION_5A_PRODUCTION 1u
#define CYDEV_CHIP_REVISION_5B_ES0 0u
#define CYDEV_CHIP_REVISION_5B_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_6A_ES 17u
#define CYDEV_CHIP_REVISION_6A_NO_UDB 33u
#define CYDEV_CHIP_REVISION_6A_PRODUCTION 33u
#define CYDEV_CHIP_REVISION_FM3_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_FM4_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_PDL_FM0P_TYPE1_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_PDL_FM0P_TYPE2_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_PDL_FM0P_TYPE3_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_USED CYDEV_CHIP_REVISION_5B_PRODUCTION
#define CYDEV_CHIP_REV_EXPECT CYDEV_CHIP_REVISION_USED
#define CYDEV_CONFIG_FASTBOOT_ENABLED 0
#define CYDEV_CONFIG_UNUSED_IO_AllowButWarn 0
#define CYDEV_CONFIG_UNUSED_IO CYDEV_CONFIG_UNUSED_IO_AllowButWarn
#define CYDEV_CONFIG_UNUSED_IO_AllowWithInfo 1
#define CYDEV_CONFIG_UNUSED_IO_Disallowed 2
#define CYDEV_CONFIGURATION_COMPRESSED 1
#define CYDEV_CONFIGURATION_DMA 0
#define CYDEV_CONFIGURATION_ECC 1
#define CYDEV_CONFIGURATION_IMOENABLED CYDEV_CONFIG_FASTBOOT_ENABLED
#define CYDEV_CONFIGURATION_MODE_COMPRESSED 0
#define CYDEV_CONFIGURATION_MODE CYDEV_CONFIGURATION_MODE_COMPRESSED
#define CYDEV_CONFIGURATION_MODE_DMA 2
#define CYDEV_CONFIGURATION_MODE_UNCOMPRESSED 1
#define CYDEV_DEBUG_ENABLE_MASK 0x20u
#define CYDEV_DEBUG_ENABLE_REGISTER CYREG_MLOGIC_DEBUG
#define CYDEV_DEBUGGING_DPS_Disable 3
#define CYDEV_DEBUGGING_DPS CYDEV_DEBUGGING_DPS_Disable
#define CYDEV_DEBUGGING_DPS_JTAG_4 1
#define CYDEV_DEBUGGING_DPS_JTAG_5 0
#define CYDEV_DEBUGGING_DPS_SWD 2
#define CYDEV_DEBUGGING_DPS_SWD_SWV 6
#define CYDEV_DEBUGGING_ENABLE 1
#define CYDEV_DEBUGGING_XRES 0
#define CYDEV_DMA_CHANNELS_AVAILABLE 24u
#define CYDEV_ECC_ENABLE 0
#define CYDEV_HEAP_SIZE 0x0400
#define CYDEV_INSTRUCT_CACHE_ENABLED 1
#define CYDEV_INTR_RISING 0x00000000u
#define CYDEV_IS_EXPORTING_CODE 0
#define CYDEV_IS_IMPORTING_CODE 0
#define CYDEV_PROJ_TYPE 1
#define CYDEV_PROJ_TYPE_BOOTLOADER 1
#define CYDEV_PROJ_TYPE_LAUNCHER 5
#define CYDEV_PROJ_TYPE_LOADABLE 2
#define CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER 4
#define CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER 3
#define CYDEV_PROJ_TYPE_STANDARD 0
#define CYDEV_PROTECTION_ENABLE 0
#define CYDEV_STACK_SIZE 0x0400
#define CYDEV_USB_CLK_OSC_LOCKING_ENABLED_AT_PWR_UP 
#define CYDEV_USE_BUNDLED_CMSIS 1
#define CYDEV_VARIABLE_VDDA 0
#define CYDEV_VDDA 5.0
#define CYDEV_VDDA_MV 5000
#define CYDEV_VDDD 5.0
#define CYDEV_VDDD_MV 5000
#define CYDEV_VDDIO0 5.0
#define CYDEV_VDDIO0_MV 5000
#define CYDEV_VDDIO1 5.0
#define CYDEV_VDDIO1_MV 5000
#define CYDEV_VDDIO2 5.0
#define CYDEV_VDDIO2_MV 5000
#define CYDEV_VDDIO3 5.0
#define CYDEV_VDDIO3_MV 5000
#define CYDEV_VIO0 5.0
#define CYDEV_VIO0_MV 5000
#define CYDEV_VIO1 5.0
#define CYDEV_VIO1_MV 5000
#define CYDEV_VIO2 5.0
#define CYDEV_VIO2_MV 5000
#define CYDEV_VIO3 5.0
#define CYDEV_VIO3_MV 5000
#define CYIPBLOCK_ARM_CM3_VERSION 0
#define CYIPBLOCK_P3_ANAIF_VERSION 0
#define CYIPBLOCK_P3_CAN_VERSION 0
#define CYIPBLOCK_P3_CAPSENSE_VERSION 0
#define CYIPBLOCK_P3_COMP_VERSION 0
#define CYIPBLOCK_P3_DECIMATOR_VERSION 0
#define CYIPBLOCK_P3_DFB_VERSION 0
#define CYIPBLOCK_P3_DMA_VERSION 0
#define CYIPBLOCK_P3_DRQ_VERSION 0
#define CYIPBLOCK_P3_DSM_VERSION 0
#define CYIPBLOCK_P3_EMIF_VERSION 0
#define CYIPBLOCK_P3_I2C_VERSION 0
#define CYIPBLOCK_P3_LCD_VERSION 0
#define CYIPBLOCK_P3_LPF_VERSION 0
#define CYIPBLOCK_P3_OPAMP_VERSION 0
#define CYIPBLOCK_P3_PM_VERSION 0
#define CYIPBLOCK_P3_SCCT_VERSION 0
#define CYIPBLOCK_P3_TIMER_VERSION 0
#define CYIPBLOCK_P3_USB_VERSION 0
#define CYIPBLOCK_P3_VIDAC_VERSION 0
#define CYIPBLOCK_P3_VREF_VERSION 0
#define CYIPBLOCK_S8_GPIO_VERSION 0
#define CYIPBLOCK_S8_IRQ_VERSION 0
#define CYIPBLOCK_S8_SAR_VERSION 0
#define CYIPBLOCK_S8_SIO_VERSION 0
#define CYIPBLOCK_S8_UDB_VERSION 0
#define DMA_CHANNELS_USED__MASK0 0x00000000u
#define CYDEV_BOOTLOADER_ENABLE 1

#endif /* INCLUDED_CYFITTER_H */
