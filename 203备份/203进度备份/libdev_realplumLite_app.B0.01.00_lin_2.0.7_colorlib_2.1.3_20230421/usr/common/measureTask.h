#ifndef _ADC_MEASUREMENT_TASK_H
#define _ADC_MEASUREMENT_TASK_H

#include <appConfig.h>
#include <softTimerTask.h>
#include "systick_device.h"
#include <taskManager.h>
#include <adc_device.h>
#include "hwcfg.h"
#include <colorMixingTask.h>
#include <ledParamTables.h>

#define INVALID_PARAM            (0x7FFFU)
#define INVALID_VOLT             (0x7FFFU)
#define INVALID_TEMP             (-127)

/*
 *  Get factory calibration data state
 *  @return 0: calibration data OK, 1 : calibration data error
 */
uint8_t MES_DataIntegrityError(void);

/*
 *  Get battery voltage in mV
 *  @param [out] battVolt: 0-32678
 *  @return 0: get voltage success, other : failed
 */
int8_t MES_GetBatteryVolt(uint16_t *battVolt);

/*
 *  Get chip temperature in 1C
 *  @param [out] chipTemperature: -127-127C
 *  @return 0: get voltage success, other : failed
 */
int16_t MES_GetChipTemperature(int16_t *chipTemperature);

/*
 *  Get Runtime Led PN voltage in mV
 *  @param [out] pnVoltR/pnVoltG/pnVoltB: 0-4000mV
 *  @return :none
 */
void MES_GetRunTimeLedPNVolt(LedNum_t ledIndex,uint16_t *pnVoltR, uint16_t *pnVoltG,uint16_t *pnVoltB);

/*
 *  Get Led temperature in 1C
 *  @return : temperature  -127-127C
 */
ColorTemperature_t MES_GetLedTemperature(LedNum_t ledNo);

/*
 *  Get Led R,G,B temperature in C
 *  @param [out] red temperature,green temperature,blue temperature
 *  @return 0
 */
int8_t MES_GetLedRGBTemperature(LedNum_t index, int8_t *rTemp,int8_t *gTemp,int8_t *bTemp);

/*
 *  measurement Task handle
 *  @return none
 */
void MES_TaskHandler(void);


/* ***************************Manufacturer test *****************************/
/*
 *  Get Led PN voltage in mV and temperature when all of leds are off for init pn voltage mesurement 
 *  @param [out] pnVoltG:     0-4000mV
 *  @return none
 */
void MES_MnftGetLedInfo(LedNum_t ledNo, uint16_t *pnRVolt,uint16_t *pnGVolt,uint16_t *pnBVolt);





#endif