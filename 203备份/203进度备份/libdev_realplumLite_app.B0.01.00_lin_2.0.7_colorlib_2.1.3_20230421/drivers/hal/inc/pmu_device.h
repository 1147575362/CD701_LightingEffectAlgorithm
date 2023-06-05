/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file pmu_device.h
 */

#ifndef PMU_DEVICE_H__
#define PMU_DEVICE_H__

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <realplumLite.h>

typedef enum{
    POLARITY_NATIVE = 0,
    POLARITY_FLIP,
}BattMonitorPolarity_t;


typedef enum{
  BOR_1V5_THRS_1218mV = 0,
  BOR_1V5_THRS_1243mV,
  BOR_1V5_THRS_1273mV,
  BOR_1V5_THRS_1298mV,
  BOR_1V5_THRS_1328mV,
  BOR_1V5_THRS_1363mV,
  BOR_1V5_THRS_1393mV,
  BOR_1V5_THRS_1428mV,
}Bor1V5Thres_t;


typedef enum{
  BOR_3V3_THRS_2163mV = 0,
  BOR_3V3_THRS_2228mV,
  BOR_3V3_THRS_2293mV,
  BOR_3V3_THRS_2363mV,
  BOR_3V3_THRS_2433mV,
  BOR_3V3_THRS_2513mV,
  BOR_3V3_THRS_2598mV,
  BOR_3V3_THRS_2688mV,
  BOR_3V3_THRS_2783mV,
  BOR_3V3_THRS_2883mV,
  BOR_3V3_THRS_2998mV,
  BOR_3V3_THRS_3118mV,
  BOR_3V3_THRS_3248mV,
}Bor3V3Thres_t;

/**
 * @brief The enumeration of brownout mode.
 */
typedef enum {
    PMU_BROWNOUT_RESET = 0U,
    PMU_BROWNOUT_INTERRUPT,
} PMU_BROWNOUT_MODE_t;

/**
 * @brief The enumeration of wake up timer mode.
 */
typedef enum {
    WAKEUP_TIMEER_DISABLE = 0U,
    WAKEUP_TIMEER_ENABLE,
} PMU_WAKEUP_TIMEER_MODE_t;

/**
 * @brief The enumeration of wake up timer mode.
 */
typedef enum {
    WAKEUP_TIMEER_INTERVAL_100us = 0U,
    WAKEUP_TIMEER_INTERVAL_200us,
    WAKEUP_TIMEER_INTERVAL_400us,
    WAKEUP_TIMEER_INTERVAL_800us,
    WAKEUP_TIMEER_INTERVAL_16ms,
    WAKEUP_TIMEER_INTERVAL_32ms,
    WAKEUP_TIMEER_INTERVAL_64ms,
    WAKEUP_TIMEER_INTERVAL_128ms,
    WAKEUP_TIMEER_INTERVAL_256ms,
    WAKEUP_TIMEER_INTERVAL_512ms,
    WAKEUP_TIMEER_INTERVAL_1024ms,
    WAKEUP_TIMEER_INTERVAL_2048ms,
    WAKEUP_TIMEER_INTERVAL_4096ms,
    WAKEUP_TIMEER_INTERVAL_8192ms,
    WAKEUP_TIMEER_INTERVAL_16384ms,
    WAKEUP_TIMEER_INTERVAL_32768ms
}PMU_WAKEUP_TIMEER_Interval_t;
void PMU_BORInit(Bor1V5Thres_t lowThreshold, Bor3V3Thres_t highThreshold);
void PMU_WakeTimerInit(PMU_WAKEUP_TIMEER_MODE_t mode, PMU_WAKEUP_TIMEER_Interval_t interval);
void PMU_EnterDeepSleepMode(void);




#endif /* __PMU_DEVICE_H__ */
