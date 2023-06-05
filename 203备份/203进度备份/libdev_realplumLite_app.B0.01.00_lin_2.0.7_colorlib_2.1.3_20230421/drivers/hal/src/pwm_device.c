/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file pwm_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <appConfig.h>

#if (LED_NUM == 1U)

#include <stdint.h>
#include <string.h>
#include <realplumLite.h>
#include <errno.h>
#include <pwm_device.h>
#include <isrfuncs.h>
#include <adc_device.h>

#if defined PWM_SFRS

/* 128us for PN voltage measurement*/
#define MAX_MEASURE_TIME_TICKS          (MAIN_CPU_CLOCK * 64U)   /* us  */
#define MAX_DEBUNCE_TIME_TICKS          (MAIN_CPU_CLOCK * 128U) /* us  */

#define MAX_PWM_CHN_DELAY_ON_TIME_TICKS (MAIN_CPU_CLOCK * 4U)   /* us  */

static uint16_t currPWM[LED_NUM][3];
static uint16_t lastPWM[LED_NUM][3];
static PwmIsrCallback_t pwmReloadCallback = NULL;

void PWM_Handler(void)
{
    if (pwmReloadCallback != NULL){
        pwmReloadCallback();
    }
}


/**
 * @brief The instance of PMW device
 */
void PWM_Init(PwmPrescaler_t divide, uint16_t period)
{
    IOCTRLA_SFRS->LEDPIN.HWMODE   = 0x07U;
    IOCTRLA_SFRS->LEDPIN.GAIN_SEL = 0U;  /* 0x0: GAIN=4 ,0x1: GAIN=8   */
    /*
    PWM_SFRS->BASE.PRESCALESEL = (uint8_t)divide;
    PWM_SFRS->BASE.PERIOD = period;
    */
    PWM_SFRS->BASE.WORD = ((uint32_t)divide << 8) + ((uint32_t)period << 16);
    
    /* set duty cycle to 0*/
    for (uint8_t i = 0U; i < 3U; i++){
        PWM_SFRS->PULSE[i].WORD = (PWM_VALUE_MAX << 16) + PWM_VALUE_MAX;
    }
    for (uint8_t i = 0; i < LED_NUM; i++){
        currPWM[i][0] = 0U;
        currPWM[i][1] = 0U;
        currPWM[i][2] = 0U;
    }
    PWM_StartAndUpdate();
}

/**
 * @brief The instance of PMW device
 */
void PWM_SetMatchValue(PwmChannel_t channel, uint16_t matchRisingValue, uint16_t matchFaillValue)
{
    PWM_SFRS->PULSE[channel].WORD = (uint32_t)matchFaillValue + ((uint32_t)matchRisingValue << 16);
}

/**
 * @brief The instance of PMW device
 */
void PWM_StartAndUpdate(void)
{
    PWM_SFRS->ENAREQ.ENAREQALL = 1U;
    PWM_SFRS->UPDATE = 1U;
}



/**
 * @brief The instance of PWM_SetRGBValue
 */
void PWM_SetRGBValue(uint8_t ledNo, uint16_t red, uint16_t green, uint16_t blue)
{
    uint32_t maxDUTY;
    uint16_t r,g,b;
    maxDUTY = (1U << PWM_VALUE_MAX_POS) - MAX_MEASURE_TIME_TICKS - MAX_DEBUNCE_TIME_TICKS - 1U;
    
    r = (uint16_t)((red  *maxDUTY) >> 16);
    g = (uint16_t)((green*maxDUTY) >> 16);
    b = (uint16_t)((blue *maxDUTY) >> 16);
   
    lastPWM[0][PHY_CHANNEL_R] = currPWM[0][PHY_CHANNEL_R];
    lastPWM[0][PHY_CHANNEL_G] = currPWM[0][PHY_CHANNEL_G];
    lastPWM[0][PHY_CHANNEL_B] = currPWM[0][PHY_CHANNEL_B];
    
    currPWM[0][PHY_CHANNEL_R] = r;
    currPWM[0][PHY_CHANNEL_G] = g;
    currPWM[0][PHY_CHANNEL_B] = b;
    
    PWM_SetMatchValue(PHY_CHANNEL_R,MAX_MEASURE_TIME_TICKS, r + MAX_MEASURE_TIME_TICKS);
    PWM_SetMatchValue(PHY_CHANNEL_G,(((uint16_t)PWM_VALUE_MAX - g)>>1U), (uint16_t)((PWM_VALUE_MAX + (uint32_t)g) >> 1U));
    PWM_SetMatchValue(PHY_CHANNEL_B,((uint16_t)PWM_VALUE_MAX - MAX_DEBUNCE_TIME_TICKS - b), (uint16_t)PWM_VALUE_MAX - MAX_DEBUNCE_TIME_TICKS);
    
    PWM_StartAndUpdate();
}

uint8_t PWM_UpdateFinished(void)
{
    uint8_t result = FALSE;
    uint8_t update = PWM_SFRS->UPDATE;
    if (update == 0U){
        result = TRUE;
    }
    return result;
}


uint8_t PWM_LedIsOn(uint8_t ledNo)
{
    uint8_t result = 1U;
    if ( (currPWM[0][PHY_CHANNEL_R] <= (PWM_VALUE_MAX/100U)) &&
         (currPWM[0][PHY_CHANNEL_G] <= (PWM_VALUE_MAX/100U)) &&
         (currPWM[0][PHY_CHANNEL_B] <= (PWM_VALUE_MAX/100U)) ){
        result = 0U;
     }else{
        if ( (lastPWM[0][PHY_CHANNEL_R] <= (PWM_VALUE_MAX/100U)) &&
             (lastPWM[0][PHY_CHANNEL_G] <= (PWM_VALUE_MAX/100U)) &&
             (lastPWM[0][PHY_CHANNEL_B] <= (PWM_VALUE_MAX/100U)) ){
               if (PWM_SFRS->UPDATE != 0U){
                  result = 0U;
               }
         }
     }
    return result;
}

/**
 * @brief The instance of PMW device
 */
void PWM_RegisterPWMReloadIRQ(PwmIsrCallback_t callback)
{
    pwmReloadCallback = callback;
    PWM_SFRS->INTPWM.CLEAR.PERIOD = 1U;
    PWM_SFRS->INTPWM.ENABLE.PERIOD = 1U;
    PWM_SFRS->UPDATE = 1U;
    NVIC_EnableIRQ(PWM_IRQn);
}

/**
 * @brief The instance of PMW device
 */
void PWM_UnRegisterPWMReloadIRQ(void)
{
    pwmReloadCallback = NULL;
    PWM_SFRS->INTPWM.CLEAR.PERIOD = 1U;
    PWM_SFRS->INTPWM.ENABLE.PERIOD   = 0U;
    PWM_SFRS->UPDATE = 1U;
    NVIC_DisableIRQ(PWM_IRQn);
}


/**
 * @brief The instance of PMW device
 */
void PWM_EnableAllChannels(void)
{
    PWM_REG_ENAREQ.ENAREQALL = 0x01U;
    PWM_SFRS->UPDATE = 1U;
}

/**
 * @brief The instance of PMW device
 */
void PWM_DisableAllChannels(void)
{
    PWM_SFRS->ENAREQ.CLRREQALL   = 0x1U;
    PWM_SFRS->ENAREQ.FORCEINACTIVE = 1U;
    PWM_SFRS->UPDATE = 1U;
}

/**
 * @brief The instance of PMW device
 */
void PWM_Disable(PwmChannel_t channel)
{
    PWM_SFRS->ENAREQ.ENAREQ &= ~(1U << channel);
    PWM_SFRS->UPDATE = 1U;
}


/**
 * @brief The instance of PMW device
 */
void PWM_SetPrescaler(PwmPrescaler_t divide)
{
    PWM_SFRS->BASE.PRESCALESEL = (uint8_t)divide;
    PWM_SFRS->UPDATE = 1U;
}

/**
 * @brief The instance of PMW device
 */
void PWM_SetPeriod(uint16_t period)
{
    PWM_SFRS->BASE.PERIOD = period;
    PWM_SFRS->UPDATE = 1U;
}

/**
 * @brief The instance of PMW device
 */
void PWM_SetInvert(PwmChannel_t channel, uint8_t invertEn)
{
    PWM_SFRS->INVERT |= (uint8_t)((uint32_t)invertEn << (uint8_t)channel);
    PWM_SFRS->UPDATE = 1U;
}

/**
 * @brief The instance of PMW device
 */
int8_t PWM_SetLedCurrent(void)
{
    /* Enable trim access write enable */
    SYSCTRLA_SFRS->TRIM_ACCESS_KEY.TRIM_ACCESS_KEY = 0x0EU;
    SYSCTRLA_SFRS->LED[0].TRIM  = HWCFG_GetLEDTrimValue(0U);
    SYSCTRLA_SFRS->LED[1].TRIM  = HWCFG_GetLEDTrimValue(1U);
    SYSCTRLA_SFRS->LED[2].TRIM  = HWCFG_GetLEDTrimValue(2U);
    SYSCTRLA_SFRS->TRIMVFW      = HWCFG_GetOffLEDTrimValue();
    return 0;
}


#endif

#endif