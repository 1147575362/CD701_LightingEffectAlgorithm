/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file pwmAux_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/03/18
 */

#include <isrfuncs.h>
#include <pwm_aux_device.h>
#include <gpio_device.h>

static PwmAuxIsrCallback_t pwmAuxReloadCallback[2] = {NULL,NULL};


void PWMAUX_Handler(void)
{
    uint8_t status = PWM_AUX_SFRS->INTPERIOD.STATUS.PERIOD;
    
    if ((status & (1U << (uint8_t)PWMAUX_BASE_0)) == (1U << (uint8_t)PWMAUX_BASE_0)){
        if (pwmAuxReloadCallback[(uint8_t)PWMAUX_BASE_0]!= NULL){
            pwmAuxReloadCallback[(uint8_t)PWMAUX_BASE_0]();
        }
        PWM_AUX_SFRS->INTPERIOD.CLEAR.PERIOD = (1U << (uint8_t)PWMAUX_BASE_0);
    }


    if ((status & (1U << (uint8_t)PWMAUX_BASE_1)) == (1U << (uint8_t)PWMAUX_BASE_1)){
        if (pwmAuxReloadCallback[(uint8_t)PWMAUX_BASE_1]!= NULL){
            pwmAuxReloadCallback[(uint8_t)PWMAUX_BASE_1]();
        }
        PWM_AUX_SFRS->INTPERIOD.CLEAR.PERIOD = (1U << (uint8_t)PWMAUX_BASE_1);
    }
}


void PWMAUX_Init(PWMAUX_Port_CH_t port,PWMAUX_ClockSource_t source,PWMAUX_Prescaler_t devider,uint16_t peroid)
{
    /* IO configuration */
    IOCTRLA_SFRS->GPIO[(uint8_t)port].HWMODE   = (uint8_t)GPIO_MUX_PWM;
    IOCTRLA_SFRS->GPIO[(uint8_t)port].PULLMODE = (uint8_t)GPIO_PULL_NONE;
    
    if((uint8_t)port > (uint8_t)GPIO_INIT_PORT_2){
        GPIO_SFRS->GPIO_CFG[(uint8_t)port + 1U].DIR = (uint8_t)GPIO_DIR_OUTPUT;
    }
    
    PWM_AUX_SFRS->PWM_AUX[(uint8_t)source].BASE.PRESCALESEL  = (uint8_t)devider;
    PWM_AUX_SFRS->PWM_AUX[(uint8_t)source].BASE.PERIOD       = peroid;
    uint8_t temp = PWM_AUX_SFRS->BASE_SEL;
    temp = temp & (~(1U << (uint8_t)port));
    temp |= (uint8_t)source << (uint8_t)port;                   /* channel connected to which clock source */
    PWM_AUX_SFRS->BASE_SEL       = temp;
    PWM_AUX_SFRS->ENAREQ.ENAREQ |= 1U << (uint8_t)port;       /* enable PWM channel:0-3   */
}


void PWMAUX_RegisterReloadIRQ(PWMAUX_ClockSource_t source, PwmAuxIsrCallback_t callback)
{
    PWM_AUX_SFRS->INTPERIOD.CLEAR.PERIOD   = 1U << (uint8_t)source;
    PWM_AUX_SFRS->INTPERIOD.ENABLE.PERIOD |= 1U << (uint8_t)source;
    pwmAuxReloadCallback[(uint8_t)source] = callback;
    NVIC_EnableIRQ(PWMAUX_IRQn);
}

void PWMAUX_UnRegisterReloadIRQ(PWMAUX_ClockSource_t source)
{
    PWM_AUX_SFRS->INTPERIOD.CLEAR.PERIOD   = 1U << (uint8_t)source;
    PWM_AUX_SFRS->INTPERIOD.ENABLE.PERIOD &= ~(1U << (uint8_t)source);
    pwmAuxReloadCallback[(uint8_t)source] = NULL;
    NVIC_DisableIRQ(PWMAUX_IRQn);
}


void PWMAUX_SetMatchValue(PWMAUX_Port_CH_t channel, uint16_t matchRisingValue, uint16_t matchFaillValue)
{
    PWM_AUX_REG_PULSE((uint32_t)channel).WORD = (uint32_t)((uint32_t)matchFaillValue + (uint32_t)((uint32_t)matchRisingValue << 16U));
    PWM_AUX_SFRS->UPDATE = 0x03U;
}


