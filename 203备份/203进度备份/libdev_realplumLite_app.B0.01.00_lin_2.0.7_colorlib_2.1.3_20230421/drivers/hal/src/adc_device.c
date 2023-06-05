/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file adc_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */
#include <appConfig.h>

#if (LED_NUM == 1U)


#include <adc_device.h>
#include <isrfuncs.h>

static ADCMeasureParam_t adcMeasParamm = {ADC_MEASURE_ITEM_NONE, 0};
static adc_cb_func_t adcCallback = NULL;
static uint16_t adcResult[4];


void ADC_Handler(void)
{
    if (adcCallback !=NULL){
        if (adcMeasParamm.item == ADC_MEASURE_ITEM_LED_VBAT_VTEMP){
            adcResult[0] = (uint16_t)(SAR_CTRL_SFRS->DATA[1].DATA & 0x0FFFU); /* VBAT       */
            adcResult[1] = (uint16_t)(SAR_CTRL_SFRS->DATA[3].DATA & 0x0FFFU); /* PTAT       */
            adcResult[2] = (uint16_t)(SAR_CTRL_SFRS->DATA[0].DATA & 0x0FFFU); /* LED PN     */
            adcResult[3] = (uint16_t)(SAR_CTRL_SFRS->DATA[2].DATA & 0x0FFFU); /* LED Short  */
        }else{
            adcResult[0] = (uint16_t)(SAR_CTRL_SFRS->DATA[0].DATA & 0x0FFFU); /* VBAT       */
            adcResult[1] = (uint16_t)(SAR_CTRL_SFRS->DATA[1].DATA & 0x0FFFU); /* PTAT       */
        }
        adcCallback(adcMeasParamm, adcResult);
    }
    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR  = 1U;
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ = 0U;
}

void ADC_GeneralInit(void)
{
#if SYS_MAIN_CLOCK_DIV == CLOCK_DIV_1
    SAR_CTRL_SFRS->SARCLKDIV = 4U;
#elif SYS_MAIN_CLOCK_DIV == CLOCK_DIV_2
    SAR_CTRL_SFRS->SARCLKDIV = 2U;
#else
    SAR_CTRL_SFRS->SARCLKDIV = 1U;
#endif
  
    SAR_CTRL_SFRS->SARANACFG.ADCVREFSEL = (uint8_t)ADC_REF_VBG_2TIMES;
    SAR_CTRL_SFRS->SARCFG.TRIGSEL       = (uint8_t)ADC_TRIGGER_SOURCE_SOFT_INPUT;
    SAR_CTRL_SFRS->SARCFG.TRIGDLY       = 8U;         /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->SARCFG.SAMPCYC       = 5U;         /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->AFECTRL.SARINPUTGAIN = (uint8_t)ADC_AFE_GAIN_31_OF_32;
    SAR_CTRL_SFRS->AFECTRL.SARINPUTMODE = (uint8_t)ADC_AFE_INPUT_INP_BUFFERED_INN_EXT;
    SAR_CTRL_SFRS->AFECTRL.SARAFEEN     = 1U;
    SAR_CTRL_SFRS->AFECTRL.ADCSELVINVCMEXT = 0U; /* set to 1 if only test PN voltage and enable low voltage mode when SAR_CTRL_SFRS->AFECTRL.SARAFEEN = 0;*/
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ       = 1U;       /* SAR ADC Enable. Set to enable the SAR analog & digital part */
    IOCTRLA_SFRS->LEDPIN.GAIN_SEL          = (uint8_t)ADC_LED_GAIN_1_OF_2;
    SAR_CTRL_SFRS->SARANACFG.SARPREAMPEN   = 1U;
    
}


void ADC_Init(AdcMeasureItem_t item, uint8_t channel, LedType_t type, AdcTriggerSource_t triggerSource)
{
    adcMeasParamm.item       = item;
    adcMeasParamm.channel    = channel;
    switch(item){
    case ADC_MEASURE_ITEM_LED_VBAT_VTEMP:               /* LED: 1/2x, VBAT:1/14x*/
      IOCTRLA_SFRS->LEDPIN.VFW_ENA = 1U;                /* Enable 2mA led current source */
      IOCTRLA_SFRS->LEDPIN.SENSE_ENA = 1U;              /* LED Forward Voltage Sense Enable. Set to enable LED forward voltage sense module.*/
      if (type == LED_RED){
          SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = (uint8_t)((uint8_t)ADC_CH_SEL_VBAT_LED0 + (uint8_t)PHY_CHANNEL_R);      /*  measure red PN volt */
          SAR_CTRL_SFRS->ADCCHCONF.CH3SEL = (uint8_t)((uint8_t)ADC_CH_SEL_LED0_GND  + (uint8_t)PHY_CHANNEL_R);
      }else if (type == LED_GREEN){
          SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = (uint8_t)((uint8_t)ADC_CH_SEL_VBAT_LED0 + (uint8_t)PHY_CHANNEL_G);      /*  measure green PN volt */
          SAR_CTRL_SFRS->ADCCHCONF.CH3SEL = (uint8_t)((uint8_t)ADC_CH_SEL_LED0_GND  + (uint8_t)PHY_CHANNEL_G);
      }else{
          SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = (uint8_t)((uint8_t)ADC_CH_SEL_VBAT_LED0 + (uint8_t)PHY_CHANNEL_B);      /*  measure blue PN volt */
          SAR_CTRL_SFRS->ADCCHCONF.CH3SEL = (uint8_t)((uint8_t)ADC_CH_SEL_LED0_GND  + (uint8_t)PHY_CHANNEL_B);
      }
      SAR_CTRL_SFRS->ADCCHCONF.CH2SEL    = (uint8_t)ADC_CH_SEL_VBAT_GND;
      SAR_CTRL_SFRS->ADCCHCONF.CH4SEL    = (uint8_t)ADC_CH_SEL_TSENSOR_GND;
      SAR_CTRL_SFRS->ADCCHCONF.SEQCNT    = (uint8_t)ADC_SQ_CH1_CH2_CH3_CH4;
      break;
      
    case ADC_MEASURE_ITEM_VBAT_VTEMP:/* 1x */
      SAR_CTRL_SFRS->ADCCHCONF.CH1SEL  = (uint8_t)ADC_CH_SEL_VBAT_GND;
      SAR_CTRL_SFRS->ADCCHCONF.CH2SEL  = (uint8_t)ADC_CH_SEL_TSENSOR_GND;
      SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1_CH2;
      break;
      
    case ADC_MEASURE_ITEM_LED:
      IOCTRLA_SFRS->LEDPIN.VFW_ENA = 1U;        /* Enable 2mA led current source */
      IOCTRLA_SFRS->LEDPIN.SENSE_ENA = 1U;      /* LED Forward Voltage Sense Enable. Set to enable LED forward voltage sense module.*/
      if (type == LED_RED){
          SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = (uint8_t)((uint8_t)ADC_CH_SEL_VBAT_LED0 + (uint8_t)PHY_CHANNEL_R);      /*  measure red PN volt */
      }else if (type == LED_GREEN){
          SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = (uint8_t)((uint8_t)ADC_CH_SEL_VBAT_LED0 + (uint8_t)PHY_CHANNEL_G);      /*  measure green PN volt */
      }else{
          SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = (uint8_t)((uint8_t)ADC_CH_SEL_VBAT_LED0 + (uint8_t)PHY_CHANNEL_B);      /*  measure blue PN volt */
      }

      SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
      break;
    case ADC_MEASURE_ITEM_VBG:  /*1x */
      SAR_CTRL_SFRS->ADCCHCONF.CH1SEL  = (uint8_t)ADC_CH_SEL_REF_GND;
      SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
      break; 
      
    case ADC_MEASURE_ITEM_VBAT: /*1/16x */
      SAR_CTRL_SFRS->ADCCHCONF.CH1SEL  = (uint8_t)ADC_CH_SEL_VBAT_GND;
      SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
      break;
      
    case ADC_MEASURE_ITEM_VDD1V5:
      SAR_CTRL_SFRS->ADCCHCONF.CH1SEL  = (uint8_t)ADC_CH_SEL_1V5_GND;
      SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
      break;
      
    case ADC_MEASURE_ITEM_GPIO:
      SAR_CTRL_SFRS->ADCCHCONF.CH1SEL  = (uint8_t)((uint8_t)ADC_CH_SEL_GPIO1_GND + (uint8_t)channel);
      SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
      break;

    default:
      break;
    }
    SAR_CTRL_SFRS->SARCFG.TRIGSEL = (uint8_t)triggerSource;
}

void ADC_UnInit(AdcMeasureItem_t item)
{
    
  
}

/**
 * @brief Register a callback function to ADC interrupt handler.
 *
 * @param dev Pointer to the ADC device.
 * @param cb The callback function to be registered.
 * @return 0 for success or error code upon a failure.
 */
void ADC_RegisterIRQ(adc_cb_func_t callback)
{
    adcCallback = callback;
    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1U;
    SAR_CTRL_SFRS->SARINT.ENABLE.INT_CONV_DONE_ENA = 1U;
    NVIC_EnableIRQ(ADC_IRQn);
    
}

/**
 * @brief Unregister a callback function to ADC interrupt handler.
 *
 * @param dev Pointer to the ADC device.
 * @param cb The callback function to be unregistered.
 * @return 0 for success or error code upon a failure.
 */
void ADC_UnregisterIRQ(void)
{
    adcCallback = NULL;
    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1U;
    SAR_CTRL_SFRS->SARINT.ENABLE.INT_CONV_DONE_ENA = 0U;
    NVIC_DisableIRQ(ADC_IRQn);
}

void ADC_Start(void)
{
    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR  = 1U;
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ = 1U;
    SAR_CTRL_SFRS->SARCTRL.CONVERT = 1U;
}

#endif