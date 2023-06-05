#ifndef SAFETY_MONITOR_TASK_H
#define SAFETY_MONITOR_TASK_H

#include <appConfig.h>
#include <hwProtection.h>
#include <softTimerTask.h>
#include <taskManager.h>
#include <ColorMixingTask.h>
#include <measureTask.h>
#include <wdt_device.h>
#include <pdsTask.h>


typedef enum {
  BATT_STATE_NORMAL = 0U,
  BATT_STATE_LOW,
  BATT_STATE_HIGH,
  BATT_STATE_RESVD,
}BatteryState_t;

typedef enum {
  CHIP_TEMPERATURE_STATE_NORMAL = 0U,
  CHIP_TEMPERATURE_STATE_LOW,
  CHIP_TEMPERATURE_STATE_HIGH,
}ChipTemperatureState_t;

typedef enum {
  LED_PN_NORMAL = 0U,
  LED_PN_SHORT,
  LED_PN_OPEN,
  LED_PN_SHORT_OPEN, 
}LedPNState_t;


BatteryState_t SAFM_GetBatteryState(void);
ChipTemperatureState_t SAFM_GetChipTemperatureState(void);
LedPNState_t SAFM_GetLedPNStateState(LedNum_t ledIndex);

void SAFM_TaskHandler(void);


#endif