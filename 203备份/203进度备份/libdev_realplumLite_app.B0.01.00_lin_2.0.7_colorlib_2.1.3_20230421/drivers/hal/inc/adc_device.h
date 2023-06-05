/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file adc_device.h
 */

#ifndef ADC_DEVICE_H__
#define ADC_DEVICE_H__

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <realplumLite.h>
#include <pdsTask.h>

#define INVALID_ADC_CODE        (0x7FFFU)

typedef enum {
  ADC_REF_VBG = 0U,     /* VBG = 1.22V  */
  ADC_REF_VBG_2TIMES,        /* 2* VBG   */
  ADC_REF_VDD_3V3,
}AdcReference_t;

typedef enum {
  ADC_TRIGGER_SOURCE_SOFT_INPUT  = 0x01U,
  ADC_TRIGGER_SOURCE_PWM_RISING  = 0x02U,
  ADC_TRIGGER_SOURCE_PWM_FALLING = 0x04U,
  ADC_TRIGGER_SOURCE_PWM_PERIOD  = 0x08U,
}AdcTriggerSource_t;


typedef enum {
  ADC_AFE_GAIN_22_OF_32 = 0U,
  ADC_AFE_GAIN_31_OF_32,
}AdcAfeGain_t;

typedef enum {
  ADC_AFE_INPUT_ALL_EXT = 0U,
  ADC_AFE_INPUT_INP_BUFFERED_INN_EXT,
  ADC_AFE_INPUT_INP_EXT_INN_BUFFERED,
  ADC_AFE_INPUT_INP_BUFFERED_INN_BUFFERED,
}AdcAfeInputMode_t;

typedef enum {
  ADC_CH_SEL_NONE = 0U,
  ADC_CH_SEL_REF_REF,
  ADC_CH_SEL_REF_GND,
  ADC_CH_SEL_GND_REF,
  ADC_CH_SEL_TSENSOR_GND,
  ADC_CH_SEL_1V5_GND,
  ADC_CH_SEL_3V3_GND,
  ADC_CH_SEL_5V0_GND,   /*  1/4  gain  */
  ADC_CH_SEL_VBAT_GND,  /*  1/14 gain  */
  ADC_CH_SEL_LED0_GND,
  ADC_CH_SEL_LED1_GND,
  ADC_CH_SEL_LED2_GND,
  ADC_CH_SEL_GPIO1_GND,
  ADC_CH_SEL_GPIO2_GND,
  ADC_CH_SEL_GPIO3_GND,
  ADC_CH_SEL_GPIO4_GND,
  ADC_CH_SEL_VBAT_LED0,
  ADC_CH_SEL_VBAT_LED1,
  ADC_CH_SEL_VBAT_LED2,
}AdcChannelSelect_t;


typedef enum {
  ADC_LED_GAIN_1_OF_2 = 0U,
  ADC_LED_GAIN_1_OF_4,
}AdcLedGain_t;


typedef enum {
  ADC_SQ_CH1 = 1U,
  ADC_SQ_CH1_CH2 = 2U,
  ADC_SQ_CH1_CH2_CH3 = 3U,
  ADC_SQ_CH1_CH2_CH3_CH4 = 4U,
}AdcSequence_t;


typedef enum {
  ADC_MEASURE_ITEM_NONE = 0U,
  ADC_MEASURE_ITEM_VBG,
  ADC_MEASURE_ITEM_VBAT,                
  ADC_MEASURE_ITEM_VDD1V5,
  ADC_MEASURE_ITEM_GPIO,
  ADC_MEASURE_ITEM_VBAT_VTEMP,        
  ADC_MEASURE_ITEM_LED_VBAT_VTEMP,
  ADC_MEASURE_ITEM_LED,                   
}AdcMeasureItem_t;

typedef struct{
  AdcMeasureItem_t item;
  uint8_t channel;
}ADCMeasureParam_t;


typedef void (*adc_cb_func_t)(ADCMeasureParam_t item, uint16_t *const result);

void ADC_GeneralInit(void);
void ADC_Init(AdcMeasureItem_t item, uint8_t channel, LedType_t type, AdcTriggerSource_t triggerSource);
void ADC_UnInit(AdcMeasureItem_t item);
void ADC_RegisterIRQ(adc_cb_func_t callback);
void ADC_UnregisterIRQ(void);
void ADC_Start(void);


#endif /* __ADC_DEVICE_H__ */