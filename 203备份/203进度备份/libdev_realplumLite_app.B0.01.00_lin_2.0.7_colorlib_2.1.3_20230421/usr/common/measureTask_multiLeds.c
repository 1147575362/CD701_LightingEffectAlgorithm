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

#if (LED_NUM > 1U)

#include <crc32.h>
#include <measureTask.h>
#include <gpio_device.h>




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

#define MES_FLAGS_UPDATE_LED_TEMP       (1U << 0)
#define MES_FLAGS_UPDATE_BATT           (1U << 1)
#define MES_FLAGS_UPDATE_CHIP_TEMP      (1U << 2)

typedef struct{
  int32_t gain;
  int32_t offset;
}calibParam_t;

typedef struct{
  uint32_t startCode;
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

static uint8_t meausureFlags = 0U;
static uint8_t paramUpdateCount = 0U;
static uint8_t dataIntegrityError = FALSE;

void add_data_to_buff(uint32_t *original, uint16_t newData, uint16_t *bufferIndex,uint16_t *buff);
static TaskState_t adcTaskState = TASK_STATE_INIT;
static AdcResult_t adcResult;




static volatile uint32_t measStart = 0U;
static volatile uint16_t measGeneralAdcCode[4];
void measureDoneISR(ADCMeasureParam_t item, uint16_t *const result);
void measureParamStart(void);
void measureParamInit(void);

static uint32_t updateParamIndex = 0U;

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
void measureAdcCodeUpdate(void);
void updateChipTemperature(void);
void updateBatteryVolt(void);
void updateLedsTemperature(uint8_t ledNo,uint16_t voltR,uint16_t voltG,uint16_t voltB);
void get_runtime_led_pn_volt(LedNum_t ledNo,uint16_t *pnRVolt,uint16_t *pnGVolt,uint16_t *pnBVolt);


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
    paramUpdateCount ++;
    if (paramUpdateCount >= (AVERAGE_MEASURE_GAIN*4U/8U)){
        paramUpdateCount = 0U;
    }else if (paramUpdateCount == (AVERAGE_MEASURE_GAIN*3U/8U)){
        meausureFlags |= MES_FLAGS_UPDATE_CHIP_TEMP;    /* update chip temperature parameters */
    }else if (paramUpdateCount == (AVERAGE_MEASURE_GAIN*2U/8U)){
        meausureFlags |= MES_FLAGS_UPDATE_BATT;         /* update battery parameters */
    }else if (paramUpdateCount == (AVERAGE_MEASURE_GAIN*1U/8U) ){
        meausureFlags |= MES_FLAGS_UPDATE_LED_TEMP;     /* update led temperature parameters */
    }else{
        /* intent to empty   */
    }
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


void measureAdcCodeUpdate(void)
{
    uint16_t codeR,codeG,codeB;
    uint16_t code = ADC_GetVTempCode();
    if (code != INVALID_ADC_CODE){
        add_data_to_buff(&adcResult.vChipTemp.startCode, code, &adcResult.vChipTemp.count,adcResult.vChipTemp.buff);
    }else{
        adcResult.vChipTemp.startCode = INVALID_PARAM;
    }
    code = ADC_GetVbatCode();
    if (code != INVALID_ADC_CODE){
        add_data_to_buff(&adcResult.vBatt.startCode, code, &adcResult.vBatt.count,adcResult.vBatt.buff);
    }else{
        adcResult.vBatt.startCode = INVALID_PARAM;
    }
    
    for (uint8_t i = 0U; i < LED_NUM; i++){
        (void)ADC_GetLedPNCode(i,&codeR,&codeG,&codeB);
        if ((codeR != INVALID_ADC_CODE) && (codeG != INVALID_ADC_CODE) && (codeB != INVALID_ADC_CODE)){
            add_data_to_buff(&adcResult.vLedPN[i][PHY_CHANNEL_R].startCode, codeR, &adcResult.vLedPN[i][PHY_CHANNEL_R].count,adcResult.vLedPN[i][PHY_CHANNEL_R].buff);
            add_data_to_buff(&adcResult.vLedPN[i][PHY_CHANNEL_G].startCode, codeG, &adcResult.vLedPN[i][PHY_CHANNEL_G].count,adcResult.vLedPN[i][PHY_CHANNEL_G].buff);
            add_data_to_buff(&adcResult.vLedPN[i][PHY_CHANNEL_B].startCode, codeB, &adcResult.vLedPN[i][PHY_CHANNEL_B].count,adcResult.vLedPN[i][PHY_CHANNEL_B].buff);
        }else{
            adcResult.vLedPN[i][PHY_CHANNEL_R].startCode = INVALID_PARAM;
            adcResult.vLedPN[i][PHY_CHANNEL_G].startCode = INVALID_PARAM;
            adcResult.vLedPN[i][PHY_CHANNEL_B].startCode = INVALID_PARAM;
        }
    }
}

void updateChipTemperature(void)
{
    /* calculate chip temperature */
#if PTAT_MEAS_USEING_RAW_ADC_VOLT != 0U
    adcResult.vChipTemp.target  = get_chip_temp_volt(&adcResult.vChipTemp);
#else
    adcResult.vChipTemp.target  = get_volt(&adcResult.vChipTemp);
#endif
    adcResult.chipTemperature   = get_chip_temperature(adcResult.vChipTemp.target);

}


void updateBatteryVolt(void)
{
    adcResult.vBatt.target= get_volt(&adcResult.vBatt);
}


void updateLedsTemperature(uint8_t ledNo,uint16_t voltR,uint16_t voltG,uint16_t voltB)
{
    if (ledNo < LED_NUM){
        int16_t typicalVolt_R, typicalVolt_G, typicalVolt_B;
        ColorTemperature_t temperature;
        temperature.value[0] = ROOM_TEMPERATURE;
        temperature.value[1] = ROOM_TEMPERATURE;
        temperature.value[2] = ROOM_TEMPERATURE;
        /* get typical LED PN volt @24C*/
        (void)CLM_GetLedPNVolts((LedNum_t)ledNo,temperature, &typicalVolt_R, &typicalVolt_G, &typicalVolt_B); /* get init PN volt @24C */
        /* get LED RGB volt and temperature */
        if (voltR != INVALID_VOLT && voltG != INVALID_VOLT && voltB != INVALID_VOLT){
            adcResult.ledPNTemperature[ledNo][0] = (int16_t)get_led_temperature(LP_GetTempTableRed((LedNum_t)ledNo),  ((int16_t)voltR - typicalVolt_R));
            adcResult.ledPNTemperature[ledNo][1] = (int16_t)get_led_temperature(LP_GetTempTableGreen((LedNum_t)ledNo),((int16_t)voltG - typicalVolt_G));
            adcResult.ledPNTemperature[ledNo][2] = (int16_t)get_led_temperature(LP_GetTempTableBlue((LedNum_t)ledNo), ((int16_t)voltB - typicalVolt_B));
        }else{
            adcResult.ledPNTemperature[ledNo][0] = ROOM_TEMPERATURE;
            adcResult.ledPNTemperature[ledNo][1] = ROOM_TEMPERATURE;
            adcResult.ledPNTemperature[ledNo][2] = ROOM_TEMPERATURE;
        }
    }
}

void MES_TaskHandler(void)
{
    switch(adcTaskState){
    case TASK_STATE_INIT:
      measureParamInit();
      SoftTimer_Start(&updateTimer);
      adcTaskState = TASK_STATE_ACTIVE;
      break;
    case TASK_STATE_ACTIVE:
      if( (meausureFlags & MES_FLAGS_UPDATE_LED_TEMP) == MES_FLAGS_UPDATE_LED_TEMP){
          meausureFlags &=(~MES_FLAGS_UPDATE_LED_TEMP);
          for (uint8_t i = 0U; i < LED_NUM; i++){
              /* calculate led temperture*/
              adcResult.vLedPN[i][0].target = get_volt(&adcResult.vLedPN[i][PHY_CHANNEL_R]);
              adcResult.vLedPN[i][1].target = get_volt(&adcResult.vLedPN[i][PHY_CHANNEL_G]);
              adcResult.vLedPN[i][2].target = get_volt(&adcResult.vLedPN[i][PHY_CHANNEL_B]);
              updateLedsTemperature(i, (uint16_t)adcResult.vLedPN[i][0].target,(uint16_t)adcResult.vLedPN[i][1].target,(uint16_t)adcResult.vLedPN[i][2].target);
          }
      }else if( (meausureFlags & MES_FLAGS_UPDATE_BATT) == MES_FLAGS_UPDATE_BATT){
          meausureFlags &=(~MES_FLAGS_UPDATE_BATT);
          /* calculate battery volt */
          updateBatteryVolt();
      }else if( (meausureFlags & MES_FLAGS_UPDATE_CHIP_TEMP) == MES_FLAGS_UPDATE_CHIP_TEMP){
          meausureFlags &=(~MES_FLAGS_UPDATE_CHIP_TEMP);
          /* calculate chip temperature */
          updateChipTemperature();
      }else{
          /* intent to empty   */
      }
      measureAdcCodeUpdate();
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
    adcResult.tChip.offset      = 30*(int32_t)(1UL << MEASURE_TEMP_GAIN_POS) - MEASURE_TEMP_GAIN*tSensor_mV; /* code measured from 30C */
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
        adcResult.vLedPN[i][0].calibP.gain = ((int32_t)codeHVolt - (int32_t)codeLVolt)*MEASURE_GAIN/((int32_t)codeH - (int32_t)codeL);
        adcResult.vLedPN[i][0].calibP.offset      = (int32_t)codeLVolt*MEASURE_GAIN - adcResult.vLedPN[i][0].calibP.gain*(int32_t)codeL;
        adcResult.vLedPN[i][0].startCode = INVALID_PARAM;
        adcResult.vLedPN[i][0].target = (int16_t)INVALID_VOLT;
        
        codeH = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN4P0V_CODE;
        codeL = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN1P5V_CODE;
        
        codeHVolt = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN4P0V_VOLT;
        codeLVolt = HWCFG_SFRS->LED[PHY_CHANNEL_G].LED_PN1P5V_VOLT;
        
        adcResult.vLedPN[i][1].calibP.gain = ((int32_t)codeHVolt - (int32_t)codeLVolt)*MEASURE_GAIN/((int32_t)codeH - (int32_t)codeL);
        adcResult.vLedPN[i][1].calibP.offset      = (int32_t)codeLVolt*MEASURE_GAIN - adcResult.vLedPN[i][1].calibP.gain*(int32_t)codeL;
        adcResult.vLedPN[i][1].startCode = INVALID_PARAM;
        adcResult.vLedPN[i][1].target   = (int16_t)INVALID_VOLT;
        
        codeH = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN4P0V_CODE;
        codeL = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN1P5V_CODE;
        
        codeHVolt = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN4P0V_VOLT;
        codeLVolt = HWCFG_SFRS->LED[PHY_CHANNEL_B].LED_PN1P5V_VOLT;
        
        adcResult.vLedPN[i][2].calibP.gain = ((int32_t)codeHVolt - (int32_t)codeLVolt)*MEASURE_GAIN/((int32_t)codeH - (int32_t)codeL);
        adcResult.vLedPN[i][2].calibP.offset      = (int32_t)codeLVolt*MEASURE_GAIN - adcResult.vLedPN[i][2].calibP.gain*(int32_t)codeL;
        adcResult.vLedPN[i][2].startCode = INVALID_PARAM;
        adcResult.vLedPN[i][2].target   = (int16_t)INVALID_VOLT;
    }
    
    //add ADC Init For LED Multi
    
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


void get_runtime_led_pn_volt(LedNum_t ledNo,uint16_t *pnRVolt,uint16_t *pnGVolt,uint16_t *pnBVolt)
{ 

    int32_t sLedCode ;
    uint32_t uLedCode[3] = {0,0,0};

    
    SAR_CTRL_SFRS->ADCCHCONF.SEQCNT     = (uint8_t)ADC_SQ_CH1;
    SAR_CTRL_SFRS->SARCFG.TRIGSEL       = (uint8_t)ADC_TRIGGER_SOURCE_SOFT_INPUT;
  
    for (uint8_t channel = 0U; channel < 3U; channel++){
        SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = (uint8_t)ADC_CH_SEL_VBAT_LED0+channel;
        for (uint8_t i = 0U; i < RAW_PN_VOLT_SAMPLE_NUM; i++){
            SAR_CTRL_SFRS->SARCTRL.CONVERT = 1U;
            while(SAR_CTRL_SFRS->SARINT.STATUS.CONV_DONE == 0U){};
            uLedCode[channel] += (uint16_t)(SAR_CTRL_SFRS->DATA[0].DATA);
            SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1U;
        }
        uLedCode[channel] = (uLedCode[channel] >> (RAW_PN_VOLT_SAMPLE_NUM_POS -1U));
        sLedCode = (int32_t)uLedCode[channel];
        sLedCode = sLedCode* adcResult.vLedPN[ledNo][channel].calibP.gain + adcResult.vLedPN[ledNo][channel].calibP.offset;
        uLedCode[channel] = (uint32_t)sLedCode;
    }
    
    *pnRVolt = (uint16_t)(uLedCode[0] >> MEASURE_GAIN_POS);
    *pnGVolt = (uint16_t)(uLedCode[1] >> MEASURE_GAIN_POS);
    *pnBVolt = (uint16_t)(uLedCode[2] >> MEASURE_GAIN_POS);
}





void MES_MnftGetLedInfo(LedNum_t ledNo, uint16_t *pnRVolt,uint16_t *pnGVolt,uint16_t *pnBVolt)
{
    if ((uint8_t)ledNo < LED_NUM){
        ADC_UnregisterIRQ();
        PWM_UnRegisterPWMReloadIRQ();
        
#if LED_NUM <= 2U
        GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << (uint8_t)ledNo));
#else
        if ((uint8_t)ledNo <= (uint8_t)LED1){
            GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << (uint8_t)ledNo));
        }else{
            GPIO_SetGPIOs((1U << GPIO_PORT_1) | (1U << GPIO_PORT_2) | (1U << GPIO_PORT_3) | (1U << GPIO_PORT_4), (1U << ((uint8_t)ledNo + 1U)));
        }
#endif
        
        for (uint8_t i = 0U; i < LED_NUM; i++){
            (void)CLM_SetPWMs((LedNum_t)i,0U, 0U, 0U, 0U);  /* turn off all of channels */
        }

        for (uint8_t i = 0U; i < 3U; i++){
            PWM_SFRS->PULSE[i].WORD = (PWM_VALUE_MAX << 16) + PWM_VALUE_MAX;
        }
        
        PWM_StartAndUpdate(); 

        while(PWM_UpdateFinished()!= TRUE){}   /* wait until taking effect */

        get_runtime_led_pn_volt(ledNo,pnRVolt,pnGVolt,pnBVolt);
       
        SAR_CTRL_SFRS->ADCCHCONF.CH1SEL  = (uint8_t)ADC_CH_SEL_VBAT_GND;
        ADC_RegisterIRQ(NULL);
        PWM_RegisterPWMReloadIRQ(NULL);

    }
}
/*
 *  Get Runtime Led PN voltage in mV
 *  @param [out] pnVoltR/pnVoltG/pnVoltB: 0-4000mV
 *  @return :none
 */
void MES_GetRunTimeLedPNVolt(LedNum_t ledIndex,uint16_t *pnVoltR, uint16_t *pnVoltG,uint16_t *pnVoltB)
{     
    *pnVoltR = (uint16_t)adcResult.vLedPN[ledIndex][0].target;
    *pnVoltG = (uint16_t)adcResult.vLedPN[ledIndex][1].target;
    *pnVoltB = (uint16_t)adcResult.vLedPN[ledIndex][2].target;
}


#endif