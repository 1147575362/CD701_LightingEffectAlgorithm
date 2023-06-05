/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file pwm_device.h
  */

#ifndef PWM_DEVICE_H__
#define PWM_DEVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include <realplumLite.h>
#include <hwcfg.h>

typedef void (*PwmIsrCallback_t)(void);

typedef enum{
  PWM_PRESCALER_DIVIDE_1 = 0U,
  PWM_PRESCALER_DIVIDE_2,
  PWM_PRESCALER_DIVIDE_4,
  PWM_PRESCALER_DIVIDE_8,
  PWM_PRESCALER_DIVIDE_16,
  PWM_PRESCALER_DIVIDE_64,
  PWM_PRESCALER_DIVIDE_256,
  PWM_PRESCALER_DIVIDE_1024,
}PwmPrescaler_t;

typedef enum{
  PWM_CHANNEL_0 = 0U,
  PWM_CHANNEL_1,
  PWM_CHANNEL_2,
}PwmChannel_t;

typedef enum{
  PWM_TRIGGER_POS_EDGE = 0U,
  PWM_TRIGGER_NEG_EDGE,
}PWMTriggerEdge_t;

typedef struct{
  PwmPrescaler_t divide;
  uint16_t       period;
  uint8_t        invertEn;
  uint8_t        enable;
}PwmConfig_t;


void LED_MULTI_ParamsInit(void);
void LED_MULTI_ADC_Handler(void);

void PWM_Init(PwmPrescaler_t divide, uint16_t period);
void PWM_SetMatchValue(PwmChannel_t channel, uint16_t matchRisingValue, uint16_t matchFaillValue);
void PWM_StartAndUpdate(void);
void PWM_SetRGBValue(uint8_t channel, uint16_t red, uint16_t green, uint16_t blue);
void PWM_RegisterPWMReloadIRQ(PwmIsrCallback_t callback);
void PWM_UnRegisterPWMReloadIRQ(void);
void PWM_EnableAllChannels(void);
void PWM_Disable(PwmChannel_t channel);
void PWM_DisableAllChannels(void);
void PWM_SetPrescaler(PwmPrescaler_t divide);
void PWM_SetPeriod(uint16_t period);
void PWM_SetInvert(PwmChannel_t channel, uint8_t invertEn);
  
int8_t PWM_SetLedCurrent(void);


uint8_t PWM_UpdateFinished(void);
uint8_t PWM_LedIsOn(uint8_t ledNo);

void PWM_LedsInit(void);
uint16_t ADC_GetVTempCode(void);
uint16_t ADC_GetVbatCode(void);
uint16_t ADC_GetLedPNCode(uint8_t ledNo, uint16_t *pnCodeR,uint16_t *pnCodeG,uint16_t *pnCodeB);

#endif /* __PWM_DEVICE_H__  */