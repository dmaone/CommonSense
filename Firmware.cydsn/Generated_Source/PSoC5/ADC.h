/*******************************************************************************
* File Name: ADC.h
* Version 2.0
*
* Description:
*  Contains the function prototypes, constants and register definition of the
*  ADC SAR Sequencer Component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_ADC_H)
    #define CY_ADC_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"
#include "ADC_TempBuf_dma.h"
#include "ADC_FinalBuf_dma.h"
#include "ADC_SAR.h"

#define ADC_NUMBER_OF_CHANNELS    (8u)
#define ADC_SAMPLE_MODE           (1u)
#define ADC_CLOCK_SOURCE          (0u)

extern int16  ADC_finalArray[ADC_NUMBER_OF_CHANNELS];
extern uint32 ADC_initVar;

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component ADC_SAR_SEQ_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */


/***************************************
*        Function Prototypes
***************************************/
void ADC_Init(void);
void ADC_Enable(void);
void ADC_Disable(void);
void ADC_Start(void);
void ADC_Stop(void);

uint32 ADC_IsEndConversion(uint8 retMode);
int16 ADC_GetResult16(uint16 chan);
int16 ADC_GetAdcResult(void);
void ADC_SetOffset(int32 offset);
void ADC_SetResolution(uint8 resolution);
void ADC_SetScaledGain(int32 adcGain);
int32 ADC_CountsTo_mVolts(int16 adcCounts);
int32 ADC_CountsTo_uVolts(int16 adcCounts);
float32 ADC_CountsTo_Volts(int16 adcCounts);
void ADC_Sleep(void);
void ADC_Wakeup(void);
void ADC_SaveConfig(void);
void ADC_RestoreConfig(void);

CY_ISR_PROTO( ADC_ISR );

/* Obsolete API for backward compatibility.
*  Should not be used in new designs.
*/
void ADC_SetGain(int32 adcGain);


/**************************************
*    Initial Parameter Constants
**************************************/
#define ADC_IRQ_REMOVE             (0u)                /* Removes internal interrupt */


/***************************************
*             Registers
***************************************/
#define ADC_CYCLE_COUNTER_AUX_CONTROL_REG \
                                               (*(reg8 *) ADC_bSAR_SEQ_ChannelCounter__CONTROL_AUX_CTL_REG)
#define ADC_CYCLE_COUNTER_AUX_CONTROL_PTR \
                                               ( (reg8 *) ADC_bSAR_SEQ_ChannelCounter__CONTROL_AUX_CTL_REG)
#define ADC_CONTROL_REG    (*(reg8 *) \
                                             ADC_bSAR_SEQ_CtrlReg__CONTROL_REG)
#define ADC_CONTROL_PTR    ( (reg8 *) \
                                             ADC_bSAR_SEQ_CtrlReg__CONTROL_REG)
#define ADC_COUNT_REG      (*(reg8 *) \
                                             ADC_bSAR_SEQ_ChannelCounter__COUNT_REG)
#define ADC_COUNT_PTR      ( (reg8 *) \
                                             ADC_bSAR_SEQ_ChannelCounter__COUNT_REG)
#define ADC_STATUS_REG     (*(reg8 *) ADC_bSAR_SEQ_EOCSts__STATUS_REG)
#define ADC_STATUS_PTR     ( (reg8 *) ADC_bSAR_SEQ_EOCSts__STATUS_REG)

#define ADC_SAR_DATA_ADDR_0 (ADC_SAR_ADC_SAR__WRK0)
#define ADC_SAR_DATA_ADDR_1 (ADC_SAR_ADC_SAR__WRK1)
#define ADC_SAR_DATA_ADDR_0_REG (*(reg8 *) \
                                              ADC_SAR_ADC_SAR__WRK0)
#define ADC_SAR_DATA_ADDR_1_REG (*(reg8 *) \
                                              ADC_SAR_ADC_SAR__WRK1)


/**************************************
*       Register Constants
**************************************/

#if(ADC_IRQ_REMOVE == 0u)

    /* Priority of the ADC_SAR_IRQ interrupt. */
    #define ADC_INTC_PRIOR_NUMBER          (uint8)(ADC_IRQ__INTC_PRIOR_NUM)

    /* ADC_SAR_IRQ interrupt number */
    #define ADC_INTC_NUMBER                (uint8)(ADC_IRQ__INTC_NUMBER)

#endif   /* End ADC_IRQ_REMOVE */


/***************************************
*       API Constants
***************************************/

/* Constants for IsEndConversion() "retMode" parameter */
#define ADC_RETURN_STATUS              (0x01u)
#define ADC_WAIT_FOR_RESULT            (0x00u)

/* Defines for the Resolution parameter */
#define ADC_BITS_12    ADC_SAR__BITS_12
#define ADC_BITS_10    ADC_SAR__BITS_10
#define ADC_BITS_8     ADC_SAR__BITS_8

#define ADC_CYCLE_COUNTER_ENABLE    (0x20u)
#define ADC_BASE_COMPONENT_ENABLE   (0x01u)
#define ADC_LOAD_COUNTER_PERIOD     (0x02u)
#define ADC_SOFTWARE_SOC_PULSE      (0x04u)

/* Generic DMA Configuration parameters */
#define ADC_TEMP_BYTES_PER_BURST     (uint8)(2u)
#define ADC_TEMP_TRANSFER_COUNT      ((uint16)ADC_NUMBER_OF_CHANNELS << 1u)
#define ADC_FINAL_BYTES_PER_BURST    ((uint16)ADC_NUMBER_OF_CHANNELS << 1u)
#define ADC_REQUEST_PER_BURST        (uint8)(1u)

#define ADC_GET_RESULT_INDEX_OFFSET    ((uint8)ADC_NUMBER_OF_CHANNELS - 1u)

/* Define for Sample Mode  */
#define ADC_SAMPLE_MODE_FREE_RUNNING    (0x00u)
#define ADC_SAMPLE_MODE_SW_TRIGGERED    (0x01u)
#define ADC_SAMPLE_MODE_HW_TRIGGERED    (0x02u)

/* Define for Clock Source  */
#define ADC_CLOCK_INTERNAL              (0x00u)
#define ADC_CLOCK_EXTERNAL              (0x01u)


/***************************************
*        Optional Function Prototypes
***************************************/
#if(ADC_SAMPLE_MODE != ADC_SAMPLE_MODE_HW_TRIGGERED)
    void ADC_StartConvert(void);
    void ADC_StopConvert(void);
#endif /* ADC_SAMPLE_MODE != ADC_SAMPLE_MODE_HW_TRIGGERED */

#endif  /* !defined(CY_ADC_H) */

/* [] END OF FILE */
