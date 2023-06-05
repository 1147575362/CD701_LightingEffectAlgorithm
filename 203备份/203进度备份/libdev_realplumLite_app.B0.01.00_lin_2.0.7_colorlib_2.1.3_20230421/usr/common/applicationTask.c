/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file applicationTask.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <applicationTask.h>
#include <measureTask.h>
#include <pdsTask.h>
#include <linStackTask.h>

/*static uint8_t ledNum = LED0;*/
static TaskState_t applState = TASK_STATE_INIT;

#define GROUP_NUMBERS (2U)					// copy
static ALU_PRAM_t LED_INFO[GROUP_NUMBERS]=
{
	[0] 	={0x03,0x0001},	//  左门
	[1] 	={0x04,0x0002},	//  右门

};



uint8_t GET_ALU_INFO_FID(uint8_t INDEX)	// copy
{
	return LED_INFO[INDEX].ALU_FID;
}


void ApplTimerExpired(SoftTimer_t *timer);


static uint8_t LED_State = 0U;

uint8_t APPL_GetLEDState(void)
{
    return LED_State;
}

static SoftTimer_t ApplTimer = {
    .mode     = TIMER_PERIODIC_MODE,
    .interval = 500U,
    .handler  = ApplTimerExpired
};


void ApplTimerExpired(SoftTimer_t *timer)
{
    ColorTemperature_t temp;
    temp.value[0] = ROOM_TEMPERATURE;
    temp.value[1] = ROOM_TEMPERATURE;
    temp.value[2] = ROOM_TEMPERATURE;
    
#if LED_NUM > 1U
        (void)CLM_SetRGBL(LED0,temp, 255U, 0U, 0U,512,0U);
        (void)CLM_SetRGBL(LED1,temp, 0U,  255U, 0U,512,0U);
        (void)CLM_SetRGBL(LED2,temp, 0U,  0U, 255U,512,0U);    
        (void)CLM_SetRGBL(LED3,temp, 255U,  255U, 255U,512,0U);     
#else    

    static uint8_t index = 0U;
    if (index == 0U){
        index = 1U;
        (void)CLM_SetRGBL(LED0,temp, 255U, 255U, 255U,1024U,0U);
    }else{
        index = 0;
        (void)CLM_SetRGBL(LED0,temp, 255U, 255U, 255U,0U, 0U);
    }
#endif
}

void APPL_TaskHandler(void)
{
    switch(applState){
    case TASK_STATE_INIT:
      //SoftTimer_Start(&ApplTimer);
      applState = TASK_STATE_ACTIVE;
      break;
    case TASK_STATE_ACTIVE:
      break;
    default:
      break;
    }
}


void APPL_HandleColorControlCommands(LIN_Device_Frame_t const *frame)
{
    ColorCtrlFrame_t const *color = (ColorCtrlFrame_t const*)((void const*)frame->data);
    if ( (color->nodeAddress == ls_read_nad()) || (color->nodeAddress == LIN_NAD_WILDCARD)){
        if (color->channel < LED_NUM){
            LedNum_t ledIndex = (LedNum_t)color->channel;
            switch(color->command){
            case CMD_COLOR_CTRL_XYY:
              (void)CLM_SetXYY(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorXY.x, color->ColorXY.y, (uint16_t)color->ColorXY.Y*10U, (uint16_t)color->ColorXY.transitionTime*100U);
              break;
            case CMD_COLOR_CTRL_XYY_EXT:
              (void)CLM_SetXYY(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorXY_EXT.x, color->ColorXY_EXT.y, (uint16_t)color->ColorXY_EXT.Y, 0U);
              break;
            case CMD_COLOR_CTRL_HSL:
              (void)CLM_SetHSL(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorHSL.hue, color->ColorHSL.saturation, (uint16_t)color->ColorHSL.level*10U, (uint16_t)color->ColorHSL.transitionTime*100U);
              break;
            case CMD_COLOR_CTRL_RGBL:
              if (color->ColorRGBL.level > 0U){
                  LED_State = 1U;
              }else{
                  LED_State = 0U;
              }
              (void)CLM_SetRGBL(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorRGBL.red, color->ColorRGBL.green, color->ColorRGBL.blue,(uint16_t)color->ColorRGBL.level*10U, (uint16_t)color->ColorRGBL.transitionTime*100U);
              break;
            case CMD_COLOR_CTRL_RGB:
              (void)CLM_SetRGB(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorRGB.red, color->ColorRGB.green, color->ColorRGB.blue, (uint16_t)color->ColorRGB.transitionTime*100U);
              break;
            case CMD_COLOR_CTRL_PWM:
              if (color->ColorPWM.red == 255U && color->ColorPWM.green == 255U && color->ColorPWM.blue == 255U){
                  (void)CLM_SetPWMs(ledIndex,65535U, 65535U, 65535U, (uint16_t)color->ColorPWM.transitionTime*100U);
              }else{
                  (void)CLM_SetPWMs(ledIndex,(uint16_t)color->ColorPWM.red*256U, (uint16_t)color->ColorPWM.green*256U, (uint16_t)color->ColorPWM.blue*256U, (uint16_t)color->ColorPWM.transitionTime*100U);
              }
              break;
            case CMD_COLOR_CTRL_LUV:
              (void)CLM_SetLUV(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorLUV.u, color->ColorLUV.v, (uint16_t)color->ColorLUV.level*10U, (uint16_t)color->ColorLUV.transitionTime*100U);
              break;
              
            case CMD_COLOR_CTRL_XY_ABS_Y:
              (void)CLM_SetXYAbsY(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorXY_Intensity.x, color->ColorXY_Intensity.y, color->ColorXY_Intensity.intensity, 0U);
              break;
            case CMD_COLOR_CTRL_ABS_L_UV:
              (void)CLM_SetAbsLUV(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorIntensity_UV.u, color->ColorIntensity_UV.v, color->ColorIntensity_UV.intensity, 0U);
              break;
            case CMD_COLOR_CTRL_sRGB_ABS_L:
              (void)CLM_SetSRGBAbsL(ledIndex,MES_GetLedTemperature(ledIndex), color->ColorSRGB_Intensity.red, color->ColorSRGB_Intensity.green, color->ColorSRGB_Intensity.blue,color->ColorSRGB_Intensity.intensity, 0U);
              break;
            default:
              break;
            }
        }
    }
}


uint8_t APPL_HandleLedParamConfigRequest(uint8_t reqSid,uint8_t *const dataBuff,DiagRspInfo_t *const diagRsp)
{
    uint8_t response = FALSE;
    CommLedParamInfo_t *const info    = (CommLedParamInfo_t *)((void *)dataBuff);
    CommLedParamInfo_t *const infoRsp = (CommLedParamInfo_t *)((void *)diagRsp->payload);

    /* handle response data here  */
    diagRsp->type = PDU_TYPE_SINGLE_PDU;
    diagRsp->sid = reqSid;
    diagRsp->packLength = SF_MAX_DATA_LENGTH;   /* Valid data length not includes sid*/
    diagRsp->payload[0] = 0xFFU;                /* failed   */
    
    infoRsp->command = info->command;
    switch(info->command){
    case APP_DIAG_SET_LED_PHY_PARAM: 
      {
      LedcolorParam_t ledColorParam = info->ledPhysicalParam.colorParam;
      ColorTemperature_t temperature;
      temperature.value[0] = info->ledPhysicalParam.temperature[0];
      temperature.value[1] = info->ledPhysicalParam.temperature[1];
      temperature.value[2] = info->ledPhysicalParam.temperature[2];
      
      if (info->ledPhysicalParam.ledIndex <= (LedNum_t)(LED_NUM -1U)){
          (void)CLM_SetLedPhyParams((LedNum_t)info->ledPhysicalParam.ledIndex,temperature,&ledColorParam);
          diagRsp->payload[0] = 0x00U;
      }
      response = TRUE;/* response enable  */
      break;
      }
    case APP_DIAG_SET_LED_GENERAL_PARAM:
      {
      Coordinate_t    whitePoint = info->ledGeneralParam.whitePoint;
      uint16_t minIntensity[3];
      minIntensity[0] = info->ledGeneralParam.minIntensity[0];
      minIntensity[1] = info->ledGeneralParam.minIntensity[1];
      minIntensity[2] = info->ledGeneralParam.minIntensity[2];
      (void)CLM_SetWhitePointParams(&whitePoint);
      (void)CLM_SetMinimumIntensity(minIntensity[0],minIntensity[1],minIntensity[2]);
      diagRsp->payload[0] = 0x00U;
      response = TRUE;/* response enable  */
      break;
      }
    case APP_DIAG_SET_LED_TYPICAL_PN_VOLT: 
      {
      int16_t volt_R,volt_G,volt_B;
      ColorTemperature_t temperature;
      volt_R = info->ledTypicalPNVolt.ledTypicalPNVolt[0];
      volt_G = info->ledTypicalPNVolt.ledTypicalPNVolt[1];
      volt_B = info->ledTypicalPNVolt.ledTypicalPNVolt[2];
      temperature.value[0] = info->ledTypicalPNVolt.temperature;
      temperature.value[1] = info->ledTypicalPNVolt.temperature;
      temperature.value[2] = info->ledTypicalPNVolt.temperature;
      (void)CLM_SetLedPNVolts((LedNum_t)info->ledTypicalPNVolt.ledIndex,temperature, volt_R, volt_G, volt_B);
      diagRsp->payload[0] = 0x00U;
      response = TRUE;/* response enable  */
      break;
      }
    default:
      break;
    }
    return response;
}

uint8_t APPL_PrepareLedParamRequest(uint8_t reqSid,uint8_t *const dataBuff, DiagRspInfo_t *const diagRsp)
{
    uint8_t response = FALSE;
    CommLedParamInfo_t *const info    = (CommLedParamInfo_t *)((void *)dataBuff);
    CommLedParamInfo_t *const infoRsp = (CommLedParamInfo_t *)((void *)diagRsp->payload);

    diagRsp->sid = reqSid;
    infoRsp->command = info->command;
    switch(info->command){
    case APP_DIAG_GET_LED_PHY_PARAM:
      {
      LedcolorParam_t ledColorParam;
      ColorTemperature_t temperature;
      temperature.value[0] = info->ledPhysicalParam.temperature[0];
      temperature.value[1] = info->ledPhysicalParam.temperature[1];
      temperature.value[2] = info->ledPhysicalParam.temperature[2];
      if (info->ledPhysicalParam.ledIndex <= (LedNum_t)(LED_NUM -1U)){
          (void)CLM_GetLedPhyParams((LedNum_t)info->ledPhysicalParam.ledIndex,temperature,&ledColorParam);
          infoRsp->ledPhysicalParam.colorParam      = ledColorParam;
          infoRsp->ledPhysicalParam.ledIndex        = info->ledPhysicalParam.ledIndex;
          infoRsp->ledPhysicalParam.temperature[0]  = info->ledPhysicalParam.temperature[0];
          infoRsp->ledPhysicalParam.temperature[1]  = info->ledPhysicalParam.temperature[1];
          infoRsp->ledPhysicalParam.temperature[2]  = info->ledPhysicalParam.temperature[2];
          
          diagRsp->packLength = 2U + (uint16_t)sizeof(CommLedPhysicalParam_t);
          diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
          response = TRUE;/* response enable  */
      }
      break;
      }
    case APP_DIAG_GET_LED_GENERAL_PARAM:
      {
      Coordinate_t    whitePoint;
      uint16_t minIntensity[3];
      (void)CLM_GetWhitePointParams(&whitePoint);
      (void)CLM_GetMinimumIntensity(&minIntensity[0],&minIntensity[1],&minIntensity[2]);
      infoRsp->ledGeneralParam.minIntensity[0] = minIntensity[0];
      infoRsp->ledGeneralParam.minIntensity[1] = minIntensity[1];
      infoRsp->ledGeneralParam.minIntensity[2] = minIntensity[2];
      infoRsp->ledGeneralParam.whitePoint = whitePoint;
      diagRsp->packLength = 2U + (uint16_t)sizeof(CommLedGeneralParam_t);
      diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
      response = TRUE;/* response enable  */
      break;
      }
    case APP_DIAG_GET_LED_TYPICAL_PN_VOLT:
      {
      int16_t volt_R,volt_G,volt_B;
      ColorTemperature_t temperature;
      temperature.value[0] = info->ledTypicalPNVolt.temperature;
      temperature.value[1] = info->ledTypicalPNVolt.temperature;
      temperature.value[2] = info->ledTypicalPNVolt.temperature;
      (void)CLM_GetLedPNVolts((LedNum_t)info->ledTypicalPNVolt.ledIndex,temperature, &volt_R, &volt_G, &volt_B);
      infoRsp->ledTypicalPNVolt.ledIndex    = info->ledTypicalPNVolt.ledIndex;
      infoRsp->ledTypicalPNVolt.temperature = info->ledTypicalPNVolt.temperature;
      infoRsp->ledTypicalPNVolt.ledTypicalPNVolt[0] = volt_R;
      infoRsp->ledTypicalPNVolt.ledTypicalPNVolt[1] = volt_G;
      infoRsp->ledTypicalPNVolt.ledTypicalPNVolt[2] = volt_B;
      diagRsp->packLength = 2U + (uint16_t)sizeof(CommLedTypicalPNVolt_t);
      diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
      response = TRUE;/* response enable  */
      break;
      }
    case APP_DIAG_GET_LED_RUN_TIME_INFO:
      {
      uint8_t ledNo;
      uint16_t battVolt,buckVolt = 0;
      int16_t chipTemperature;
      ColorTemperature_t temperature;
      int16_t sum;
      (void)MES_GetBatteryVolt(&battVolt);
      (void)MES_GetChipTemperature(&chipTemperature);
      
      infoRsp->ledRunTimeParam.battVolt = battVolt;
      infoRsp->ledRunTimeParam.buckVolt = buckVolt;
      infoRsp->ledRunTimeParam.chipTemp = chipTemperature;
      infoRsp->ledRunTimeParam.ledNum   = LED_NUM;
      for (ledNo = 0U; ledNo < LED_NUM; ledNo++){
        temperature = MES_GetLedTemperature((LedNum_t)ledNo);
        sum = (int16_t)temperature.value[0] + (int16_t)temperature.value[1] + (int16_t)temperature.value[1] + (int16_t)temperature.value[2];
        infoRsp->ledRunTimeParam.ledTemperature[ledNo] = (int16_t)(sum/4);
      }
      diagRsp->packLength = 2U + (uint16_t)sizeof(CommLedRunTimeParam_t);
      diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
      response = TRUE;/* response enable  */
      break;
      }
    case APP_DIAG_GET_LED_RGB_RUN_TEMP:
      {
        int8_t rTemp,gTemp,bTemp;
        if ((uint8_t)info->LedRunTimePNTemp.ledIndex < LED_NUM ){
            (void)MES_GetLedRGBTemperature(info->LedRunTimePNTemp.ledIndex, &rTemp,&gTemp,&bTemp);
            infoRsp->LedRunTimePNTemp.ledIndex = info->LedRunTimePNTemp.ledIndex;
            infoRsp->LedRunTimePNTemp.ledTemp[0] = rTemp;
            infoRsp->LedRunTimePNTemp.ledTemp[1] = gTemp;
            infoRsp->LedRunTimePNTemp.ledTemp[2] = bTemp;
            diagRsp->packLength = 2U + (uint16_t)sizeof(CommLedRunTimePNTemp_t);
            diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
            response = TRUE;/* response enable  */
        }
        break;
      }
      
    case APP_DIAG_GET_LED_STATIC_PN_VOLT:
      {
      /* force to measure PN volt when Led is off for getting initialized PN voltage */
      uint16_t volt_R,volt_G,volt_B;
      MES_MnftGetLedInfo(info->ledStaticPNVolt.ledIndex, &volt_R,&volt_G,&volt_B);
      infoRsp->ledStaticPNVolt.ledIndex = info->ledStaticPNVolt.ledIndex;
      infoRsp->ledStaticPNVolt.ledPNVolt[0] = volt_R;
      infoRsp->ledStaticPNVolt.ledPNVolt[1] = volt_G;
      infoRsp->ledStaticPNVolt.ledPNVolt[2] = volt_B;
      diagRsp->packLength = 2U + (uint16_t)sizeof(CommLedStaticPNVolt_t);
      diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
      response = TRUE;/* response enable  */
      break;
      }
      
    case APP_DIAG_GET_LED_RGB_RUN_VOLT:
      {
        uint16_t volt_R,volt_G,volt_B;
        uint16_t battVolt;
        int16_t chipTemperature;
        if ((uint8_t)info->LedRunTimePNTemp.ledIndex < LED_NUM ){
          /* force to measure PN volt when Led is off for getting initialized PN voltage */
          MES_GetRunTimeLedPNVolt(info->ledRunTimePNVolt.ledIndex,&volt_R, &volt_G, &volt_B);
          infoRsp->ledRunTimePNVolt.ledIndex = info->ledRunTimePNVolt.ledIndex;
          infoRsp->ledRunTimePNVolt.ledPNVolt[0] = volt_R;
          infoRsp->ledRunTimePNVolt.ledPNVolt[1] = volt_G;
          infoRsp->ledRunTimePNVolt.ledPNVolt[2] = volt_B;
          
          (void)MES_GetBatteryVolt(&battVolt);
          (void)MES_GetChipTemperature(&chipTemperature);
          
          infoRsp->ledRunTimePNVolt.battVolt = battVolt;
          infoRsp->ledRunTimePNVolt.chipTemp = chipTemperature;
          
          diagRsp->packLength = 2U + (uint16_t)sizeof(CommLedRunTimePNVolt_t);
          diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
          response = TRUE;/* response enable  */
        }
        break;
      }
    case APP_DIAG_GET_SDK_VERSION:
      infoRsp->Version.sdkApplication   = SDK_APPL_VERSION;
      infoRsp->Version.linStack         = ls_get_lin_version();
      infoRsp->Version.ledLibrary       = CLM_GetColorLibraryVersion();
      diagRsp->packLength = 2U + (uint16_t)sizeof(CommVersion_t);
      diagRsp->type = (diagRsp->packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
      response = TRUE;
      break;
    default:
      response = FALSE;
      break;
    }
    return response;
}
