/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file safetyMonitorTask.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <safetyMonitorTask.h>

#define OVER_TEMPERATURE_HIGH           (130)   /*celsius degree */
#define OVER_TEMPERATURE_RELEASE        (OVER_TEMPERATURE_HIGH - 10)   /*celsius degree */


#define LED_PN_SHORT_VOLT                       (500U)   /* mV */
#define LED_PN_SHORT_OPEN                       (4000U)  /* mV */

#define INTENSITY_RATIO_TYPICAL                 (1024U)                                 /* 100% of max intensity */
#define INTENSITY_RATIO_HIGH_TEMPERATURE        (INTENSITY_RATIO_TYPICAL*4U/5U)         /* 80% of max intensity */
#define INTENSITY_RATIO_BATT_ISSUE              (0U)                                    /* 0% of max intensity */


static TaskState_t safeMonitorState = TASK_STATE_INIT;
static BatteryState_t battState = BATT_STATE_NORMAL;
static BatteryState_t battLastState = BATT_STATE_NORMAL;
static ChipTemperatureState_t chipTemperatureState = CHIP_TEMPERATURE_STATE_NORMAL;
static ChipTemperatureState_t chipTemperatureLastState = CHIP_TEMPERATURE_STATE_NORMAL;
static LedPNState_t ledPNState[LED_NUM];

static void UnderVoltageProtection_ISR(void);
static void OverVoltageProtection_ISR(void);
static void safetyHandle(void);
static void safetyInit(void);
static void safeMonitorTimerExpired(SoftTimer_t *timer);

static SoftTimer_t safeMonitorTimer = {
    .mode     = TIMER_PERIODIC_MODE,
    .interval = 200U,
    .handler  = safeMonitorTimerExpired
};


static void safeMonitorTimerExpired(SoftTimer_t *timer)
{
    TM_PostTask(TASK_ID_SAFETY_MONITOR);
}

void SAFM_TaskHandler(void)
{
    switch(safeMonitorState){
    case TASK_STATE_INIT:
      safetyInit();
      SoftTimer_Start(&safeMonitorTimer);
      safeMonitorState = TASK_STATE_ACTIVE;
      break;
    case TASK_STATE_ACTIVE:
      safetyHandle();
      break;
    default:
      break;
    }
}


static void safetyHandle(void)
{
  
    /*  Led PN Voltage Protection Handling     */
    uint16_t pnVoltR,pnVoltG,pnVoltB;
    uint8_t pnState;
    for (uint8_t ledIndex = 0U; ledIndex < LED_NUM; ledIndex++){
        pnState = 0U;
        MES_GetRunTimeLedPNVolt((LedNum_t)ledIndex,&pnVoltR, &pnVoltG,&pnVoltB);
        if ( (pnVoltR != INVALID_VOLT) && (pnVoltG != INVALID_VOLT)  && (pnVoltB != INVALID_VOLT) ){
            if ( (pnVoltR <= LED_PN_SHORT_VOLT) || (pnVoltG <= LED_PN_SHORT_VOLT) || (pnVoltB <= LED_PN_SHORT_VOLT)){
                pnState = 1U;           /* short  */
            }
            if ( (pnVoltR >= LED_PN_SHORT_OPEN) || (pnVoltG >= LED_PN_SHORT_OPEN) || (pnVoltB >= LED_PN_SHORT_OPEN)){
                pnState |= (1U << 1);   /* open  */
            }
        }
        ledPNState[ledIndex] = (LedPNState_t)pnState;
    }
    
    
  
    /*  Temperature Protection Handling     */
    int16_t chipTemperature;
    if (MES_GetChipTemperature(&chipTemperature) == 0){
        if (chipTemperature >= OVER_TEMPERATURE_HIGH){
            chipTemperatureState = CHIP_TEMPERATURE_STATE_HIGH;
        }else{
            if (chipTemperature <= OVER_TEMPERATURE_RELEASE){
                chipTemperatureState = CHIP_TEMPERATURE_STATE_NORMAL;
            }
        }
    }
    
    /*  Battery Voltage Protection Handling     */
    battState = (BatteryState_t)HW_PROT_GetBattVoltState();
    if ((chipTemperatureLastState != chipTemperatureState) || (battLastState != battState)){
        chipTemperatureLastState = chipTemperatureState;
        battLastState = battState;
        if (battState == BATT_STATE_LOW){
            /* Turn off LED when Battery voltage is low */
            (void)CLM_SetLimitedMaxIntensity(INTENSITY_RATIO_BATT_ISSUE);
            TM_PostTask(TASK_ID_COLOR_COMPENSATION);
            /* LED PN detection disbaled when Voltage is low */
            for (uint8_t ledIndex = 0U; ledIndex < LED_NUM; ledIndex++){
                ledPNState[ledIndex] = LED_PN_NORMAL;
            }
        }else{
            if (chipTemperatureState == CHIP_TEMPERATURE_STATE_HIGH){
                (void)CLM_SetLimitedMaxIntensity(INTENSITY_RATIO_HIGH_TEMPERATURE);
                TM_PostTask(TASK_ID_COLOR_COMPENSATION);
            }else{
                (void)CLM_SetLimitedMaxIntensity(INTENSITY_RATIO_TYPICAL);
                TM_PostTask(TASK_ID_COLOR_COMPENSATION);
            }
            /* Restart OV UV ISR   */
            PMUA_SFRS->PMUIRQ.CLEAR.OV  = 1U;
            PMUA_SFRS->PMUIRQ.CLEAR.UV  = 1U;
            NVIC_ClearPendingIRQ(UV_IRQn);
            NVIC_EnableIRQ(UV_IRQn);
            NVIC_ClearPendingIRQ(OV_IRQn);
            NVIC_EnableIRQ(OV_IRQn);
        }
    }

}


static void UnderVoltageProtection_ISR(void)
{
    TM_PostTask(TASK_ID_SAFETY_MONITOR);
}

static void OverVoltageProtection_ISR(void)
{
    TM_PostTask(TASK_ID_SAFETY_MONITOR);
}


static void safetyInit(void)
{
    for (uint8_t ledIndex = 0U; ledIndex < LED_NUM; ledIndex++){
        ledPNState[ledIndex] = LED_PN_NORMAL;
    }
    HW_PROT_SetUnderVoltage(UV_VOLT_5_135V,UV_HYS_08_PERCENT, VOLT_DEBOUNCE_TIME_10ms, VOLT_DEBOUNCE_TIME_10ms);
    HW_PROT_RegisterUnderVoltageIRQ(UnderVoltageProtection_ISR);
    
    HW_PROT_SetOverVoltage(OV_VOLT_20_0V, OV_HYS_12_PERCENT,   VOLT_DEBOUNCE_TIME_10ms, VOLT_DEBOUNCE_TIME_10ms);
    HW_PROT_RegisterOverVoltageIRQ(OverVoltageProtection_ISR);
    
}


BatteryState_t SAFM_GetBatteryState(void)
{
    return battState;
}


ChipTemperatureState_t SAFM_GetChipTemperatureState(void)
{
    return chipTemperatureState;
}


LedPNState_t SAFM_GetLedPNStateState(LedNum_t ledIndex)
{
    return ledPNState[(uint8_t)ledIndex];
}
