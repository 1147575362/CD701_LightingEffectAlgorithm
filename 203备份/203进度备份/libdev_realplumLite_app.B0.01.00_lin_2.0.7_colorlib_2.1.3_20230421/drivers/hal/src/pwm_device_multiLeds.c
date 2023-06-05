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

#if (LED_NUM > 1U)


#include <stdint.h>
#include <string.h>
#include <realplumLite.h>
#include <errno.h>
#include <pwm_device.h>
#include <appConfig.h>
#include <isrfuncs.h>
#include <adc_device.h>
#include <gpio_device.h>


#if defined PWM_SFRS

/* 128us for PN voltage measurement*/
#define MAX_MEASURE_TIME_TICKS          (MAIN_CPU_CLOCK * 64U)   /* us  */
#define MAX_DEBUNCE_TIME_TICKS          (MAIN_CPU_CLOCK * 128U) /* us  */

#define MAX_PWM_CHN_DELAY_ON_TIME_TICKS (MAIN_CPU_CLOCK * 4U)   /* us  */


uint16_t ADC_GetLedShortCode(uint8_t ledNo,uint16_t *pnCodeBattR,uint16_t *pnCodeBattG,uint16_t *pnCodeBattB);

static volatile uint16_t currPWMMatchValue[LED_NUM][3];

static uint32_t LedPowerSwitch = 0U;

static volatile uint8_t currLedNo  = 0U;
static volatile uint8_t ledConvert = 1U;
static  uint8_t ledChannel = 0U;


static uint16_t currPWM[LED_NUM][3];
static uint16_t lastPWM[LED_NUM][3];

static volatile uint8_t ledIsActive[LED_NUM];
static uint16_t ledPnCode[LED_NUM][3];
static uint16_t ledshortCode[LED_NUM][3];
static uint16_t battCode;
static uint16_t tempCode;

void LED_MULTI_ParamsInit(void)
{
    for(uint8_t i = 0U; i < LED_NUM; i++){
      currPWMMatchValue[i][PHY_CHANNEL_R] = PWM_VALUE_MAX;
      currPWMMatchValue[i][PHY_CHANNEL_G] = PWM_VALUE_MAX;   
      currPWMMatchValue[i][PHY_CHANNEL_B] = PWM_VALUE_MAX;
    }     
}
void PWM_Handler(void)
{
    uint8_t ledNo = currLedNo;
    if (ledIsActive[ledNo] == TRUE){
        SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1_CH2_CH3_CH4; 
 
       ledConvert = TRUE;
    }else{
        SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1_CH2;
        ledConvert = FALSE;
    }  
   
    SAR_CTRL_SFRS->SARCTRL.CONVERT = 1;
    PWM_SFRS->INTPWM.CLEAR.BYTE = 0x03U; /* Clear ISR flags */
}



void LED_MULTI_ADC_Handler(void)
{
#if LED_NUM == 1U
    LedPowerSwitch = 0U;
    PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[0][PHY_CHANNEL_R];
    PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[0][PHY_CHANNEL_G];
    PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[0][PHY_CHANNEL_B];
    GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_1));
    currLedNo = 0U;
#elif LED_NUM == 2U
    if (LedPowerSwitch == 0U){
        LedPowerSwitch = 1U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[0][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[0][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[0][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_2));
        currLedNo = 1U;
    }else{
        LedPowerSwitch = 0U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[1][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[1][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[1][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_1));
        currLedNo = 0U;
    }
#elif LED_NUM == 3U   
    if (LedPowerSwitch == 0U){
        LedPowerSwitch = 1U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[0][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[0][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[0][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_3));
        currLedNo = 2U;
    }else if (LedPowerSwitch == 1U){
        LedPowerSwitch = 2U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[1][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[1][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[1][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_1));
        currLedNo = 0U;
    }else{
        LedPowerSwitch = 0U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[2][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[2][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[2][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_2));
        currLedNo = 1U;
    }
#elif LED_NUM == 4U
  
  if (LedPowerSwitch == 0U){
        LedPowerSwitch = 1U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[0][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[0][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[0][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_4));
        currLedNo = 3U;
    }else if (LedPowerSwitch == 1U){
        LedPowerSwitch = 2U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[1][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[1][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[1][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_1));
        currLedNo = 0U;
    }else if(LedPowerSwitch == 2U){
        LedPowerSwitch = 3U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[2][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[2][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[2][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_2));
        currLedNo = 1U;
    }else{
        LedPowerSwitch = 0U;
        PWM_SFRS->PULSE[PHY_CHANNEL_R].PRISE   = currPWMMatchValue[3][PHY_CHANNEL_R];
        PWM_SFRS->PULSE[PHY_CHANNEL_G].PRISE = currPWMMatchValue[3][PHY_CHANNEL_G];
        PWM_SFRS->PULSE[PHY_CHANNEL_B].PRISE  = currPWMMatchValue[3][PHY_CHANNEL_B];
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << GPIO_PORT_3));
        currLedNo = 2U;
    }  
#else
  #error LED_NUM must be in the range [1,4]
#endif    
    PWM_StartAndUpdate();
     
}

void ADC_Handler(void)
{
    uint8_t ledNo = currLedNo;
    uint8_t lastLedChannel = ledChannel;
    
#if LED_NUM == 1U
    if (ledNo == 0U){
        ledChannel++;
    }
#elif LED_NUM == 2U
    if (ledNo >= 1U){
        ledChannel++;
    }
#elif LED_NUM == 3U
    if (currLedNo >= 2U){
        ledChannel++;
    }
#elif LED_NUM == 4U
    if (ledNo >= 3U){
        ledChannel++;
    }
#else
#error LED_NUM must be in the range [1,4]
#endif  
    if (ledChannel >= 3U){
        ledChannel = 0U;
    }

    SAR_CTRL_SFRS->ADCCHCONF.CH3SEL = ((uint8_t)ADC_CH_SEL_VBAT_LED0 + ledChannel);
    SAR_CTRL_SFRS->ADCCHCONF.CH4SEL = ((uint8_t)ADC_CH_SEL_LED0_GND + ledChannel); 
    
    if (ledConvert == TRUE){   
        battCode = (uint16_t)(SAR_CTRL_SFRS->DATA[0].DATA & 0x0FFFU); /* VBAT       */
        tempCode = (uint16_t)(SAR_CTRL_SFRS->DATA[1].DATA & 0x0FFFU); /* PTAT       */
        ledPnCode[ledNo][lastLedChannel] = (uint16_t)(SAR_CTRL_SFRS->DATA[2].DATA & 0x0FFFU); /* VBAT-VLEDx      */
        ledshortCode[ledNo][lastLedChannel]  = (uint16_t)(SAR_CTRL_SFRS->DATA[3].DATA & 0x0FFFU); /* VLEDx-GND       */

    }else{
        battCode = (uint16_t)(SAR_CTRL_SFRS->DATA[0].DATA & 0x0FFFU); /* VBAT       */
        tempCode = (uint16_t)(SAR_CTRL_SFRS->DATA[1].DATA & 0x0FFFU); /* PTAT       */        
    }


    LED_MULTI_ADC_Handler();    

    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR  = 1U;

}


uint16_t ADC_GetLedPNCode(uint8_t ledNo, uint16_t *pnCodeR,uint16_t *pnCodeG,uint16_t *pnCodeB)
{   
__atomic_enter()
    *pnCodeR = ledPnCode[ledNo][PHY_CHANNEL_R];
    *pnCodeG = ledPnCode[ledNo][PHY_CHANNEL_G];
    *pnCodeB = ledPnCode[ledNo][PHY_CHANNEL_B];
__atomic_exit()
    return 0U;
}

uint16_t ADC_GetLedShortCode(uint8_t ledNo,uint16_t *pnCodeBattR,uint16_t *pnCodeBattG,uint16_t *pnCodeBattB)
{
__atomic_enter()
    *pnCodeBattR = ledshortCode[ledNo][PHY_CHANNEL_R];
    *pnCodeBattG = ledshortCode[ledNo][PHY_CHANNEL_G];
    *pnCodeBattB = ledshortCode[ledNo][PHY_CHANNEL_B];
__atomic_exit()
    return 0U;
}

uint16_t ADC_GetVbatCode(void)
{   
    uint16_t code = battCode;
    return code;
}

uint16_t ADC_GetVTempCode(void)
{   
    uint16_t code = tempCode;
    return code;
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


    PWM_SFRS->INTPWM.CLEAR.PERIOD = 1U;
    PWM_SFRS->INTPWM.ENABLE.PERIOD = 1U;
    PWM_SFRS->UPDATE = 1U;
    NVIC_EnableIRQ(PWM_IRQn);


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
void PWM_SetRGBValue(uint8_t ledNo,uint16_t red, uint16_t green, uint16_t blue)
{
    if (ledNo < LED_NUM){
        uint16_t r,g,b;
        r= (uint16_t)(((uint32_t)red*31U   >> 5U) >> (16U-PWM_VALUE_MAX_POS));
        g= (uint16_t)(((uint32_t)green*31U >> 5U) >> (16U-PWM_VALUE_MAX_POS));
        b= (uint16_t)(((uint32_t)blue*31U  >> 5U) >> (16U-PWM_VALUE_MAX_POS));
        
        currPWM[ledNo][PHY_CHANNEL_R] = r;
        currPWM[ledNo][PHY_CHANNEL_G] = g;
        currPWM[ledNo][PHY_CHANNEL_B] = b;
        if (red >= (65535U/256U) || green >= (65535U/256U) || blue >= (65535U/256U)){
            ledIsActive[ledNo] = TRUE;
        }else{
            ledIsActive[ledNo] = FALSE;
            ledPnCode[ledNo][0] = INVALID_ADC_CODE;
            ledPnCode[ledNo][1] = INVALID_ADC_CODE;
            ledPnCode[ledNo][2] = INVALID_ADC_CODE;
        }
        currPWMMatchValue[ledNo][PHY_CHANNEL_R]   = (uint16_t)(PWM_VALUE_MAX - r);
        currPWMMatchValue[ledNo][PHY_CHANNEL_G]   = (uint16_t)(PWM_VALUE_MAX - g);
        currPWMMatchValue[ledNo][PHY_CHANNEL_B]   = (uint16_t)(PWM_VALUE_MAX - b);
        PWM_SFRS->UPDATE = 1U; 
    }
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
    SYSCTRLA_SFRS->LED[0].TRIM = HWCFG_GetLEDTrimValue(0U);
    SYSCTRLA_SFRS->LED[1].TRIM = HWCFG_GetLEDTrimValue(1U);
    SYSCTRLA_SFRS->LED[2].TRIM = HWCFG_GetLEDTrimValue(2U);
    SYSCTRLA_SFRS->TRIMVFW = HWCFG_GetOffLEDTrimValue();
    return 0;
}


#endif



#endif