/**
 * @copyright 2016 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file clock_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <clock_device.h>

#define SYSTEM_CLOCK_HIGH_FREQ  (16000U)/* KHz */
#define SYSTEM_CLOCK_LOW_FREQ   (256U)  /* KHz */
static uint32_t systemClock = SYSTEM_CLOCK_HIGH_FREQ;

void Clock_SystemMainClockInit(uint8_t divider)
{
    CRGA_SFRS->SYSCLKCTRL.HFRCENA= 1U;
    while(CRGA_SFRS->SYSCLKCTRL.HFRCSTS == 0U){}
    /* Enable trim access write enable */
    SYSCTRLA_SFRS->TRIM_ACCESS_KEY.TRIM_ACCESS_KEY = 0x0EU;
    SYSCTRLA_SFRS->HF_OSC_TRIM.TRIM_HF_RC= HWCFG_GetRCHFClockCalibValue();
    SYSCTRLA_SFRS->LEDBIASTRIM=  HWCFG_GetLedBiasTrimValue();
    
    CRGA_SFRS->LFCLKCTRL.CLKLFSEL = CLOCK_LF_RC_16KHz;
    SYSCTRLA_SFRS->TRIM_LF_RC = HWCFG_GetRCLFClockCalibValue();
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U){}
    
    CRGA_SFRS->SYSCLKCTRL.SYSCLKSEL = (uint8_t)CLOCK_RC_16MHz;
    CRGA_SFRS->SYSCLKCTRL.DIVSYSCLK = (uint8_t)divider;
    /*  frequency spread    */
    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCDEEP  = 7U;
    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCDIV   = 15U;
    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCENA   = 1U;
    
    systemClock = SYSTEM_CLOCK_HIGH_FREQ;
    systemClock = systemClock >> (uint8_t)divider;
    
}

uint32_t Clock_GetSystemClock(void)
{
    return systemClock;/* KHz */
}
