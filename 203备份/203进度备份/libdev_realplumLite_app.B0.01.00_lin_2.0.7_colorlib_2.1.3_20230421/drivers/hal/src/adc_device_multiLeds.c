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

#if (LED_NUM > 1U)


#include <adc_device.h>
#include <isrfuncs.h>



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
    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1U;
    SAR_CTRL_SFRS->SARINT.ENABLE.INT_CONV_DONE_ENA = 0U;
    NVIC_DisableIRQ(ADC_IRQn);
}

void ADC_Start(void)
{
    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR  = 1U;
//    SAR_CTRL_SFRS->SARINT.ENABLE.INT_CONV_DONE_ENA = 1U;
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ = 1U;
    SAR_CTRL_SFRS->SARCTRL.CONVERT = 1U;
}

#endif