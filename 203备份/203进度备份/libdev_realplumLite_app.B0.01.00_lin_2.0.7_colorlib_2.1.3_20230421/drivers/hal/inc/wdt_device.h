/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file wdt_device.h
 */

#ifndef WDT_DEVICE_H__
#define WDT_DEVICE_H__

#include <stdint.h>
#include <wdt1_sfr.h>
#include <wdta_sfr.h>
#include "errno.h"


typedef void (*wdt_cb_func_t)(void);

typedef enum{
  WDT1_INTERVAL_E13 = 0U,/*0x0: 2^13 / System Clock  512us @16MHz*/
  WDT1_INTERVAL_E19,    /*0x1: 2^19 / System Clock  32768us @16MHz*/
  WDT1_INTERVAL_E22,    /*0x2: 2^22 / System Clock  262144us @16MHz*/
  WDT1_INTERVAL_E32,    /*0x3: 2^32 / System Clock  268seconds @@16MHz*/
}WDT1Interval_t;

typedef enum{
  WDT1_MODE_RESET = 0U,
  WDT1_MODE_INTERRUPT
}WDT1Mode_t;

void WDT1_Enable(WDT1Mode_t mode, WDT1Interval_t interval, wdt_cb_func_t callback);
void WDT1_Clear(void);

typedef enum{
  WDTA_INTERVAL_144MS = 0U,/*0x0: 2^11 * 62.5us ~= 128*(1+1/8) ms*/
  WDTA_INTERVAL_288MS,     /*0x2: 2^12 * 62.5us ~= 256*(1+1/8) ms*/
  WDTA_INTERVAL_576MS,     /*0x3: 2^13 * 62.5us ~= 512*(1+1/8) ms*/
  WDTA_INTERVAL_1152MS,    /*0x4: 2^14 * 62.5us ~= 1.0*(1+1/8) s*/
  WDTA_INTERVAL_2304MS,    /*0x5: 2^15 * 62.5us ~= 2.0*(1+1/8) s*/
  WDTA_INTERVAL_4608MS,    /*0x6: 2^16 * 62.5us ~= 4.0*(1+1/8) s*/
  WDTA_INTERVAL_9216MS,    /*0x7: 2^17 * 62.5us ~= 8.0*(1+1/8) s*/
  WDTA_INTERVAL_18432MS,   /*0x7: 2^18 * 62.5us ~= 16.0*(1+1/8)s*/
}WDTAInterval_t;

typedef enum{
  WDTA_MODE_RESET = 0U,
}WDTAMode_t;


void WDTA_Enable(WDTAInterval_t interval);
void WDTA_Disable(void);
void WDTA_Clear(void);




#endif /* __WDT_DEVICE_H__ */

