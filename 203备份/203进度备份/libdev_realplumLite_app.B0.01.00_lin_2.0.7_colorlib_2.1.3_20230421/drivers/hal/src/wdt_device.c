/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file wdt_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <string.h>
#include <realplumLite.h>
#include "wdt_device.h"
#include "errno.h"
#include <isrfuncs.h>
/**
 * @brief The instance of WDT device
 */
static wdt_cb_func_t  wdt1Callback = NULL;


void Watchdog_Handler(void)
{
    if (wdt1Callback != NULL){
        wdt1Callback();
    }
}

void WDT1_Enable(WDT1Mode_t mode, WDT1Interval_t interval, wdt_cb_func_t callback)
{
    WDT1_SFRS->CFG.PRESET           = (uint8_t)interval;
    if (mode == WDT1_MODE_RESET){
        WDT1_SFRS->CFG.RSTEN        = 1U;
    }else{
        wdt1Callback = callback;
        NVIC_EnableIRQ(Watchdog_IRQn);
    }
    WDT1_SFRS->CFG.ENA              = 1U;
}

#pragma optimize=none
void WDT1_Clear(void)
{
    __disable_irq();
    WDT1_SFRS->KEY       = 0x3C570001U;
    WDT1_SFRS->KEY       = 0x007F4AD6U; 
    __enable_irq();
}


void WatchdogA_Handler(void)
{

}

void WDTA_Enable(WDTAInterval_t interval)
{
    WDTA_SFRS->CTRL.TIMEOUTSEL = (uint8_t)interval;
    CRGA_SFRS->WDTACTION.WDTBARKCNT = 5;
    WDTA_SFRS->CTRL.UPDATE = 1U;
    WDTA_SFRS->CLEAR = 0x3c574ad6U;
    WDTA_SFRS->STOP.STOP = 0U;/* start wdt*/

}

void WDTA_Disable(void)
{
    WDTA_SFRS->STOP.STOP = 0xC3U;/* stop wdt*/
}

void WDTA_Clear(void)
{
    WDTA_SFRS->CLEAR = 0x3c574ad6U;
}






























