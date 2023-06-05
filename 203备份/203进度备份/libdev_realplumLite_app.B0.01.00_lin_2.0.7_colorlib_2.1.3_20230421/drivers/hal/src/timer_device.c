/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file timer_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <realplumLite.h>
#include <timer_device.h>
#include "errno.h"
#include <isrfuncs.h>


static HwGpTimer_t hwGpTimer[HW_GP_TIMER_NUM] = {
  {.timeReloadCnt = 0xFFFFFFFFU, .periodCount = 0U, .handler = NULL},
  {.timeReloadCnt = 0xFFFFFFFFU, .periodCount = 0U, .handler = NULL},
};

void Timer0_Handler( void )
{
    if (hwGpTimer[TIMER0].mode == HW_TIMER_ONE_SHOT_MODE){
        TIMER0_SFRS->ENA = 0U;
        NVIC_DisableIRQ(Timer0_IRQn);
    }else{
        TIMER0_SFRS->COUNT = hwGpTimer[TIMER0].timeReloadCnt;
    }
    if (hwGpTimer[TIMER0].handler != NULL){
        hwGpTimer[TIMER0].handler();
    }else{
        /*  intent to empty  */  
    }
}

void Timer1_Handler( void )
{
    if (hwGpTimer[TIMER1].mode == HW_TIMER_ONE_SHOT_MODE){
        TIMER1_SFRS->ENA = 0U;
        NVIC_DisableIRQ(Timer1_IRQn);
    }else{
        TIMER1_SFRS->COUNT = hwGpTimer[TIMER1].timeReloadCnt;
    }
    if (hwGpTimer[TIMER1].handler != NULL){
        hwGpTimer[TIMER1].handler();
    }else{
        /*  intent to empty  */ 
    }
}




void TIMER_Init(HwGpTimerId_t timerId, HwGpTimeMode_t mode, uint32_t timeoutTicks, TimeoutCallbackFunc_t callback)
{
    hwGpTimer[timerId].timeReloadCnt = 0xFFFFFFFFU - timeoutTicks;
    hwGpTimer[timerId].handler = callback;
    hwGpTimer[timerId].mode = mode;
    hwGpTimer[timerId].periodCount = 0U;
    if (timerId == TIMER0){
        TIMER0_SFRS->COUNT = hwGpTimer[timerId].timeReloadCnt;
        TIMER0_SFRS->ENA = 1U;
        NVIC_EnableIRQ(Timer0_IRQn);
    }else{
        TIMER1_SFRS->COUNT = hwGpTimer[timerId].timeReloadCnt;
        TIMER1_SFRS->ENA = 1U;
        NVIC_EnableIRQ(Timer1_IRQn);
    }
}


void TIMER_Disable(HwGpTimerId_t timerId)
{
    hwGpTimer[timerId].handler = NULL;
    if (timerId == TIMER0){
        TIMER0_SFRS->ENA = 0U;
        NVIC_DisableIRQ(Timer0_IRQn);
    }else{
        TIMER1_SFRS->ENA = 0U;
        NVIC_DisableIRQ(Timer1_IRQn);
    }
}




















