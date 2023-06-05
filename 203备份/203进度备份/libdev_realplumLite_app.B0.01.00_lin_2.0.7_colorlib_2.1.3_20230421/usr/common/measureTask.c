/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file measureTask.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <appConfig.h>

#if (LED_NUM == 1U)


#include <crc32.h>
#include <measureTask.h>

#define AVERAGE_MEASURE_POS     (4U)
#define AVERAGE_MEASURE_GAIN    (1U << AVERAGE_MEASURE_POS)

#define MEASURE_GAIN_POS         12U
#define MEASURE_GAIN             4096 /*(1UL << MEASURE_GAIN_POS)*/

#define MEASURE_TEMP_GAIN_POS    14U
#define MEASURE_TEMP_GAIN        7208 /*16384*0.43994*/


#define RAW_PN_VOLT_SAMPLE_NUM_POS    4U
#define RAW_PN_VOLT_SAMPLE_NUM        (1U << RAW_PN_VOLT_SAMPLE_NUM_POS)

#if RAW_PN_VOLT_SAMPLE_NUM_POS <=0U
    #error RAW_PN_VOLT_SAMPLE_NUM_POS >= 1U
#endif

typedef struct{
  int32_t gain;
  int32_t offset;
}calibParam_t;

typedef struct{
  uint32_t startCode; /* means the measurement is started or not  */
  uint16_t buff[AVERAGE_MEASURE_GAIN];
  uint16_t count;
  calibParam_t calibP;
  int16_t target;
}AdcDatabuff_t;

typedef struct{
  AdcDatabuff_t vBatt;
  AdcDatabuff_t vLedPN[LED_NUM][3];
  AdcDatabuff_t vChipTemp;
  calibParam_t  tChip;
  int16_t       chipTemperature;
  int16_t       ledPNTemperature[LED_NUM][3];
}AdcResult_t;


static uint8_t dataIntegrityError = FALSE;

void add_data_to_buff(uint32_t *original, uint16_t newData, uint16_t *bufferIndex,uint16_t *buff);
static TaskState_t adcTaskState = TASK_STATE_INIT;
static AdcResult_t adcResult;

static uint8_t measLedColorNo = 0U;
static AdcTriggerSource_t measSyncMode = ADC_TRIGGER_SOURCE_SOFT_INPUT;
static AdcMeasureItem_t  measItem = ADC_MEASURE_ITEM_NONE;
static volatile uint32_t measStart = 0U;
static volatile uint16_t measGeneralAdcCode[4];
void measureDoneISR(ADCMeasureParam_t item, uint16_t *const result);
void measureParamStart(void);
void measureParamInit(void);

static uint32_t updateParamIndex = 0U;
static uint32_t updateParamStart = 0U;
void updateTimerExpired(SoftTimer_t *timer);

#if PTAT_MEAS_USEING_RAW_ADC_VOLT != 0U
int16_t get_chip_temp_volt(AdcDatabuff_t *const param);
#endif

int16_t get_volt(AdcDatabuff_t *const param);
int16_t get_chip_temperature(int16_t vTemp);
int8_t get_led_temperature(TempParams_t const table[],int16_t deltaPNVolt);
void updateSystemInfo(void);
void updateLedTemperature(LedNum_t index);
void updateParams(void);

static volatile uint32_t adcConvertDone = 0U;

static SoftTimer_t updateTimer = {
    .mode     = TIMER_PERIODIC_MODE,
    .interval = 50U,
    .handler  = updateTimerExpired
};


void add_data_to_buff(uint32_t *original, uint16_t newData, uint16_t *bufferIndex,uint16_t *buff)
{
    if (*original == INVALID_PARAM){
        *original = newData;
        *bufferIndex = 0;
        for (uint8_t i = 0U; i < AVERAGE_MEASURE_GAIN; i++){
            buff[i] = newData;
        }
    }else{
        buff[*bufferIndex] = newData;
    }
    (*bufferIndex) ++;
    if ((*bufferIndex) >= AVERAGE_MEASURE_GAIN){
        (*bufferIndex) = 0U;
    }
}


void updateTimerExpired(SoftTimer_t *timer)
{
    static uint8_t measWaitCount = 0U;
    /*  update then start next measurement  */
    if (++measWaitCount >= 3U){
        measWaitCount = 0U;
        updateParamStart = 1U;
    }
    measStart = 1U; 
    measItem  = ADC_MEASURE_ITEM_NONE;
    TM_PostTask(TASK_ID_ADC_MEASURE);
}


void measureDoneISR(ADCMeasureParam_t param, uint16_t *const result)
{
    measGeneralAdcCode[0] = (result[0]>=0x800U)? 0U: result[0];
    measGeneralAdcCode[1] = (result[1]>=0x800U)? 0U: result[1];
    measGeneralAdcCode[2] = (result[2]>=0x800U)? 0U: result[2];
    measGeneralAdcCode[3] = (result[3]>=0x800U)? 0U: result[3];
    measStart = 1U;
    adcConvertDone = 1U;
    TM_PostTask(TASK_ID_ADC_MEASURE);
}

void measureParamStart(void)
{
    switch(measItem){
    case ADC_MEASURE_ITEM_NONE:
      measSyncMode = ADC_TRIGGER_SOURCE_SOFT_INPUT;
      measItem = ADC_MEASURE_ITEM_VBAT_VTEMP;
      break;
    case ADC_MEASURE_ITEM_VBAT_VTEMP:
      add_data_to_buff(&adcResult.vBatt.startCode,     measGeneralAdcCode[0], &adcResult.vBatt.count,adcResult.vBatt.buff);
      add_data_to_buff(&adcResult.vChipTemp.startCode, measGeneralAdcCode[1], &adcResult.vChipTemp.count,adcResult.vChipTemp.buff);
      if (PWM_LedIsOn(0U) != 0U){
          measLedColorNo = 0U;
          measSyncMode = ADC_TRIGGER_SOURCE_PWM_PERIOD;
          measItem = ADC_MEASURE_ITEM_LED_VBAT_VTEMP;
      }else{
          adcResult.vLedPN[0][0].startCode = INVALID_PARAM;
          adcResult.vLedPN[0][1].startCode = INVALID_PARAM;
          adcResult.vLedPN[0][2].startCode = INVALID_PARAM;
          measSyncMode = ADC_TRIGGER_SOURCE_SOFT_INPUT;
          measItem  = ADC_MEASURE_ITEM_NONE;
      }
      break;
    case ADC_MEASURE_ITEM_LED_VBAT_VTEMP:
      add_data_to_buff(&adcResult.vBatt.startCode,     measGeneralAdcCode[0], &adcResult.vBatt.count,adcResult.vBatt.buff);
      add_data_to_buff(&adcResult.vChipTemp.startCode, measGeneralAdcCode[1], &adcResult.vChipTemp.count,adcResult.vChipTemp.buff);
      if (PWM_LedIsOn(0U) != 0U){
          add_data_to_buff(&adcResult.vLedPN[0][measLedColorNo].startCode, measGeneralAdcCode[2], &adcResult.vLedPN[0][measLedColorNo].count,adcResult.vLedPN[0][measLedColorNo].buff);
      }else{
          adcResult.vLedPN[0][0].startCode = INVALID_PARAM;
          adcResult.vLedPN[0][1].startCode = INVALID_PARAM;
          adcResult.vLedPN[0][2].startCode = INVALID_PARAM;
      }
      measLedColorNo++;
      if (measLedColorNo >= 3U){
          measSyncMode = ADC_TRIGGER_SOURCE_SOFT_INPUT;
          measItem  = ADC_MEASURE_ITEM_NONE;
      }
      break;
    default:
      break;
    }
    if (measItem != ADC_MEASURE_ITEM_NONE){
        ADC_Init(measItem,0U,(LedType_t)measLedColorNo,measSyncMode);
        ADC_Start();
    }
}

#if PTAT_MEAS_USEING_RAW_ADC_VOLT != 0U
int16_t get_chip_temp_volt(AdcDatabuff_t *const param)
{
    uint32_t bandgap_mV  = HWCFG_GetVBG();
    
    int16_t volt = (int16_t)INVALID_VOLT;
    uint32_t sum = 0;
    if (param->startCode != INVALID_PARAM){
        for (uint8_t i = 0U; i < AVERAGE_MEASURE_GAIN; i++){
            sum += param->buff[i];
        }
        sum  = sum >> (AVERAGE_MEASURE_POS - 1U);
        sum = (bandgap_mV*sum) >> 12U;
        volt = (int16_t)sum;
    }
    return volt;
}
#endif

int16_t get_volt(AdcDatabuff_t *const param)
{
    int16_t volt = (int16_t)INVALID_VOLT;
    int32_t sVolt;
    uint32_t uVolt;
    uint32_t sum = 0;
    if (param->startCode != INVALID_PARAM){
        for (uint8_t i = 0U; i < AVERAGE_MEASURE_GAIN; i++){
            sum += param->buff[i];
        }
        sum  = sum >> (AVERAGE_MEASURE_POS -1U);
        sVolt = param->calibP.gain * (int32_t)sum + param->calibP.offset;
        uVolt = (uint32_t)sVolt;
        uVolt = uVolt >> MEASURE_GAIN_POS;
        volt = (int16_t)uVolt;
    }
    return volt;
}


int16_t get_chip_temperature(int16_t vTemp)
{
    int32_t  stemp;
    uint32_t uTemp;
    uint8_t  negative = 0U;
    stemp = adcResult.tChip.gain*vTemp + adcResult.tChip.offset;
    if (stemp < 0){
        stemp = -stemp;
        negative = 1U;
    }
    uTemp = (uint32_t)stemp;
    uTemp = uTemp >> MEASURE_TEMP_GAIN_POS;
    stemp = (int32_t)uTemp;
    if (negative != 0U){
        stemp = -stemp;
    }
    return (int16_t)stemp;
}


void updateSystemInfo(void)
{
    /* calculate chip temperature */
#if PTAT_MEAS_USEING_RAW_ADC_VOLT != 0U
    adcResult.vChipTemp.target  = get_chip_temp_volt(&adcResult.vChipTemp);
#else
    adcResult.vChipTemp.target  = get_volt(&adcResult.vChipTemp);
#endif
    adcResult.chipTemperature   = get_chip_temperature(adcResult.vChipTemp.target);
    adcResult.vBatt.target      = get_volt(&adcResult.vBatt);
}


int8_t get_led_temperature(TempParams_t const table[],int16_t deltaPNVolt)
{
    int8_t temperature;
    uint8_t minPos = 0U;
    uint8_t maxPos = TEMP_CMP_NUM_MAX-1U;
    uint8_t tempPos = ((TEMP_CMP_NUM_MAX-1U)>> 1U);
    for(;;){
        if (deltaPNVolt > table[tempPos].deltaVoltPN){
            maxPos = tempPos;
        }else if (deltaPNVolt < table[tempPos].deltaVoltPN){
            minPos = tempPos;
        }else{
            break;
        }
        tempPos = (minPos + maxPos) >> 1U;
        if ((minPos + 1U)>= maxPos){
            break;
        }
    }
    if (deltaPNVolt <= table[TEMP_CMP_NUM_MAX-1U].deltaVoltPN){
        tempPos = TEMP_CMP_NUM_MAX-1U;
    }
    tempPos = tempPos << DELTA_TEMP_POS;
    temperature = (int8_t)tempPos;
    temperature = temperature + MINIMUM_TEMP;
    return temperature;
}

void updateLedTemperature(LedNum_t ledIndex)
{
    int16_t typicalVolt[3];
    ColorTemperature_t temperature;
    temperature.value[0] = ROOM_TEMPERATURE;
    temperature.value[1] = ROOM_TEMPERATURE;
    temperature.value[2] = ROOM_TEMPERATURE;
    
    (void)CLM_GetLedPNVolts(LED0,temperature, &typicalVolt[0], &typicalVolt[1], &typicalVolt[2]);
    for (uint8_t i = 0U; i < 3U; i++){
        adcResult.vLedPN[0][i].target = get_volt(&adcResult.vLedPN[0][i]);
        if (adcResult.vLedPN[0][i].target != (int16_t)INVALID_VOLT){
            if (i == (uint8_t)LED_RED){
                adcResult.ledPNTemperature[0][i] = get_led_temperature(LP_GetTempTableRed(LED0),  (adcResult.vLedPN[0][i].target - typicalVolt[i]) );
            }else if (i == (uint8_t)LED_GREEN){
                adcResult.ledPNTemperature[0][i] = get_led_temperature(LP_GetTempTableGreen(LED0),(adcResult.vLedPN[0][i].target - typicalVolt[i]) );
            }else{
                adcResult.ledPNTemperature[0][i] = get_led_temperature(LP_GetTempTableBlue(LED0), (adcResult.vLedPN[0][i].target - typicalVolt[i]) );
            }
        }else{
            adcResult.ledPNTemperature[0][i] = ROOM_TEMPERATURE;
        }
    }
}


void updateParams(void)
{
    if( updateParamIndex == 0U){
        /* update system information */
        updateSystemInfo();
    }else{
        /* update led temperature information LED0-LED3*/
        for (uint8_t i = 0U; i < LED_NUM; i++){
            updateLedTemperature((LedNum_t)i);
        }
    }
    if (++updateParamIndex > 2U){
        updateParamIndex = 0U;
    }
}


void MES_TaskHandler(void)
{
    switch(adcTaskState){
    case TASK_STATE_INIT:
      measureParamInit();
      ADC_GeneralInit();/*  init ADC */
      ADC_RegisterIRQ(measureDoneISR);
      /* start updating timer */
      SoftTimer_Start(&updateTimer);
      adcTaskState = TASK_STATE_ACTIVE;
      break;
    case TASK_STATE_ACTIVE:
      /* start parameters updating  */
      if (updateParamStart != 0U){
          updateParamStart = 0U;
          updateParams();
      }
      /*  Start parameters measuring */
      if (measStart != 0U){
          measStart = 0U;
          measureParamStart();
      }
      break;
    default:
      break;
    }
}

void measureParamInit(void)
{
    uint32_t crc = CRC32_GetRunTimeCRC32((uint8_t *)HWCFG_SFRS_START->payload+4U ,508U);
    if (crc != HWCFG_SFRS->crc32){
        dataIntegrityError = TRUE;
    }

#if PTAT_MEAS_USEING_RAW_ADC_VOLT != 0U
    uint32_t bandgap_mV  = HWCFG_GetVBG();
    uint16_t tSensorCode = HWCFG_SFRS->TSENSOR_CODE_25C;
    int32_t tSensor_mV;
    bandgap_mV = (bandgap_mV*tSensorCode) >> 12U;
    tSensor_mV = (int32_t)bandgap_mV;
    adcResult.vChipTemp.startCode = INVALID_PARAM;
    /* v = a*t + b;  a = 2.273,  t = v/a - b/a;   gain = 1/a, offset = - b/a*/
    adcResult.tChip.gain = MEASURE_TEMP_GAIN;
    adcResult.tChip.offset      = 30*(int32_t)(1UL << MEASURE_TEMP_GAIN_POS) - MEASURE_TEMP_GAIN*tSensor_mV;/* 30 degree centigrade*/
#else
    uint16_t adc0V5 = HWCFG_SFRS->ADC_0V5_CODE;
    uint16_t adc1V0 = HWCFG_SFRS->ADC_1V0_CODE;
    adcResult.vChipTemp.calibP.gain = (1000 -500)*MEASURE_GAIN/((int32_t)adc1V0 - (int32_t)adc0V5);
    adcResult.vChipTemp.calibP.offset =  500*MEASURE_GAIN - adcResult.vChipTemp.calibP.gain*(int32_t)adc0V5;
    adcResult.vChipTemp.startCode = INVALID_PARAM;
    /* v = a*t + b;  a = 2.273,  t = v/a - b/a;   gain = 1/a, offset = - b/a*/
    adcResult.tChip.gain = MEASURE_TEMP_GAIN;
    adcResult.tChip.offset      = 25*(int32_t)(1UL << MEASURE_TEMP_GAIN_POS) - MEASURE_TEMP_GAIN*(int32_t)HWCFG_SFRS->TSENSOR_mV_25C;
#endif
    /* battery volt*/
    uint16_t batt13V5 = HWCFG_SFRS->VBAT_CODE_13V5;
    uint16_t batt8V0 = HWCFG_SFRS->VBAT_CODE_8V0;
    adcResult.vBatt.calibP.gain = (int32_t)(13500 - 8000)*MEASURE_GAIN/((int32_t)batt13V5 - (int32_t)batt8V0);
    adcResult.vBatt.calibP.offset      = (int32_t)(8000*MEASURE_GAIN) - adcResult.vBatt.calibP.gain*(int32_t)batt8V0;
    adcResult.vBatt.startCode    = INVALID_PARAM;
    
    /* led parameters init */
    uint16_t codeH,codeL;
    uint16_t codeHVolt,codeLVolt;
    for (uint8_t i = 0; i < LED_NUM; i++){
        codeH = HWCFG_SFRS->LED[PHY_CHANNEL_R].LED_PN4P0V_CODE;
        codeL = HWCFG_SFRS->LED[PHY_CHANNEL_R].LED_PN1P5V_CODE;
        codeHVolt = HWCFG_SFRS->LED[PHY_CHANNEL_R].LED_PN4P0V_VOLT;
        codeLVolt = HWCFG_SFRS->LED[PHY_CHANNEL_R].LED_PN1P5V_VOLT;
        adcResult.vLedPN[0][0].calibP.gain = ((int32_t)codeHVolt - (int32_t)codeLVolt)*MEASURE_GAIN/((int32_t)codeH - (int32_t)codeL);
        adcResult.vLedPN[0][0].calibP.offset      = (int32_t)codeLVolt*MEASURE_GAIN - adcResult.vLedPN[i][0].calibP.gain*(int32_t)codeL;
        adcResult.vLedPN[0][0].startCode = INVALID_PARAM;
        adcResult.vLedPN[0][0].target = (int16_t)INVALID_VOLT;
        
        codeH = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN4P0V_CODE;
        codeL = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN1P5V_CODE;
        
        codeHVolt = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN4P0V_VOLT;
        codeLVolt = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN1P5V_VOLT;
        
        adcResult.vLedPN[0][1].calibP.gain = ((int32_t)codeHVolt - (int32_t)codeLVolt)*MEASURE_GAIN/((int32_t)codeH - (int32_t)codeL);
        adcResult.vLedPN[0][1].calibP.offset      = (int32_t)codeLVolt*MEASURE_GAIN - adcResult.vLedPN[i][1].calibP.gain*(int32_t)codeL;
        adcResult.vLedPN[0][1].startCode = INVALID_PARAM;
        adcResult.vLedPN[0][1].target   = (int16_t)INVALID_VOLT;
        
        codeH = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN4P0V_CODE;
        codeL = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN1P5V_CODE;
        
        codeHVolt = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN4P0V_VOLT;
        codeLVolt = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN1P5V_VOLT;
        
        adcResult.vLedPN[0][2].calibP.gain = ((int32_t)codeHVolt - (int32_t)codeLVolt)*MEASURE_GAIN/((int32_t)codeH - (int32_t)codeL);
        adcResult.vLedPN[0][2].calibP.offset      = (int32_t)codeLVolt*MEASURE_GAIN - adcResult.vLedPN[i][2].calibP.gain*(int32_t)codeL;
        adcResult.vLedPN[0][2].startCode = INVALID_PARAM;
        adcResult.vLedPN[0][2].target   = (int16_t)INVALID_VOLT;
    }
}

/*
 *  Get factory calibration data state
 *  @return 0: calibration data OK, 1 : calibration data error
 */
uint8_t MES_DataIntegrityError(void)
{
    return dataIntegrityError;
  
}

/*
 *  Get battery voltage in mV
 *  @param [out] battVolt: 0-65535
 *  @return 0: get voltage success, other : failed
 */
int8_t MES_GetBatteryVolt(uint16_t *battVolt)
{
    int8_t result = -1;
    if (adcResult.vBatt.startCode != INVALID_PARAM){
        *battVolt = (uint16_t)adcResult.vBatt.target;
        result = 0;
    }
    return result;
}


int16_t MES_GetChipTemperature(int16_t *chipTemperature)
{
    int16_t result = -1;
    if (adcResult.vChipTemp.startCode != INVALID_PARAM){
        *chipTemperature = adcResult.chipTemperature;
        result = 0;
    }
    return result;
}


/*  */
ColorTemperature_t MES_GetLedTemperature(LedNum_t ledNo)
{
    ColorTemperature_t temperature;
    temperature.value[0] = (int8_t)adcResult.ledPNTemperature[ledNo][0];
    temperature.value[1] = (int8_t)adcResult.ledPNTemperature[ledNo][1];
    temperature.value[2] = (int8_t)adcResult.ledPNTemperature[ledNo][2];
    return temperature;
}

 int8_t MES_GetLedRGBTemperature(LedNum_t index, int8_t *rTemp,int8_t *gTemp,int8_t *bTemp)
 {
    *rTemp = (int8_t)adcResult.ledPNTemperature[index][0];
    *gTemp = (int8_t)adcResult.ledPNTemperature[index][1];
    *bTemp = (int8_t)adcResult.ledPNTemperature[index][2];
    return 0;
 }


void MES_MnftGetLedInfo(LedNum_t ledNo, uint16_t *pnRVolt,uint16_t *pnGVolt,uint16_t *pnBVolt)
{
    int32_t sLedCode = 0;
    uint32_t uLedCode[3];
    for(uint8_t i = 0U; i < LED_NUM; i++){
        (void)CLM_SetPWMs((LedNum_t)i,0U, 0U, 0U, 0U);  /* turn off all of channels */
    }
    while(PWM_UpdateFinished() != TRUE){}
    SAR_CTRL_SFRS->SARCFG.TRIGSEL = 1; /* soft trigger for adc conversion   */
    for (uint8_t led = 0U; led < 3U; led++){
        uLedCode[led] = 0U;
        ADC_Init(ADC_MEASURE_ITEM_LED,(uint8_t)0,(LedType_t)led,ADC_TRIGGER_SOURCE_SOFT_INPUT);
        for (uint16_t i = 0U; i < RAW_PN_VOLT_SAMPLE_NUM; i++){
            ADC_Start();
            adcConvertDone = 0U;
            while(adcConvertDone == 0U){}
            uLedCode[led] += measGeneralAdcCode[0];
        }
        uLedCode[led] = (uLedCode[led] >> (RAW_PN_VOLT_SAMPLE_NUM_POS -1U));
        sLedCode = (int32_t)uLedCode[led];
        sLedCode = sLedCode* adcResult.vLedPN[0][led].calibP.gain + adcResult.vLedPN[0][led].calibP.offset;
        uLedCode[led] = (uint32_t)sLedCode;
    }
    *pnRVolt = (uint16_t)(uLedCode[0] >> MEASURE_GAIN_POS);
    *pnGVolt = (uint16_t)(uLedCode[1] >> MEASURE_GAIN_POS);
    *pnBVolt = (uint16_t)(uLedCode[2] >> MEASURE_GAIN_POS);
    (void)sLedCode;
}


/*
 *  Get Runtime Led PN voltage in mV
 *  @param [out] pnVoltR/pnVoltG/pnVoltB: 0-4000mV
 *  @return :none
 */
void MES_GetRunTimeLedPNVolt(LedNum_t ledIndex,uint16_t *pnVoltR, uint16_t *pnVoltG,uint16_t *pnVoltB)
{     
    *pnVoltR = (uint16_t)adcResult.vLedPN[0][0].target;
    *pnVoltG = (uint16_t)adcResult.vLedPN[0][1].target;
    *pnVoltB = (uint16_t)adcResult.vLedPN[0][2].target;
}




#endif