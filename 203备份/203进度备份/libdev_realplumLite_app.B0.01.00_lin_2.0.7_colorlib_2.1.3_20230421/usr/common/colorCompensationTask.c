/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file colorCompensationTask.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <colorCompensationTask.h>

#define COLOR_COMP_INTERVAL 500U
#define COLOR_COMP_DIMMING_INTERVAL 1000U
static TaskState_t ccpState = TASK_STATE_INIT;
void doColorCompensationHandle(void);
void colorCompTimerExpired(SoftTimer_t *timer);
static uint8_t ledNo = LED0;

static SoftTimer_t colorCompTimer = {
    .mode     = TIMER_PERIODIC_MODE,
    .interval = COLOR_COMP_INTERVAL, /* color compensation detect time interval */
    .handler  = colorCompTimerExpired
};


void colorCompTimerExpired(SoftTimer_t *timer)
{
    /* color compensation only starts until the diming operation has been finished */
    if ( CLM_FadingIsFinished((LedNum_t)ledNo) == TRUE){
        TM_PostTask(TASK_ID_COLOR_COMPENSATION);
    }
}

void CCP_TaskHandler(void)
{
    switch(ccpState){
    case TASK_STATE_INIT:
    {
        SoftTimer_Start(&colorCompTimer);
        ccpState = TASK_STATE_ACTIVE;
        break;
    }
    case TASK_STATE_ACTIVE:
      doColorCompensationHandle(); /* do color compensation operation if temperature has changed */
      break;
    default:
      break;
    }
}


void doColorCompensationHandle(void)
{
    ColorTemperature_t currTemperature[LED_NUM];
    /* Get current LED temperature */
    currTemperature[ledNo] = MES_GetLedTemperature((LedNum_t)ledNo);
    ColorParam_t const *color = CLM_GetCurrentColorParams((LedNum_t)ledNo);
    if(color != NULL){
        switch(color->colorMode){
        case COLOR_MODE_XYY:
          (void)CLM_SetXYY((LedNum_t)ledNo,currTemperature[ledNo], color->XYY.x, color->XYY.y, color->XYY.Y, COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_LUV:
          (void)CLM_SetLUV((LedNum_t)ledNo,currTemperature[ledNo], color->LUV.u, color->LUV.v, color->LUV.L, COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_sRGBL:
          (void)CLM_SetSRGBL((LedNum_t)ledNo,currTemperature[ledNo], color->RGBL.red, color->RGBL.green, color->RGBL.blue,color->RGBL.level,COLOR_COMP_DIMMING_INTERVAL);
          break;
          
        case COLOR_MODE_XY_ABS_Y:
          (void)CLM_SetXYAbsY((LedNum_t)ledNo,currTemperature[ledNo], color->XYY.x, color->XYY.y, color->XYY.Y, COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_ABS_LUV:
          (void)CLM_SetAbsLUV((LedNum_t)ledNo,currTemperature[ledNo], color->LUV.u, color->LUV.v, color->LUV.L, COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_sRGB_ABS_L:
          (void)CLM_SetSRGBAbsL((LedNum_t)ledNo,currTemperature[ledNo], color->RGBL.red, color->RGBL.green, color->RGBL.blue,color->RGBL.level,COLOR_COMP_DIMMING_INTERVAL);
          break;
          
          
        case COLOR_MODE_ACCURATE_XYY:
          (void)CLM_SetAccurateXYY((LedNum_t)ledNo,color->iRatio, currTemperature[ledNo], color->XYY.x, color->XYY.y, (uint8_t)color->XYY.Y, COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_RGBL:
          (void)CLM_SetRGBL((LedNum_t)ledNo,currTemperature[ledNo], color->RGBL.red, color->RGBL.green, color->RGBL.blue,color->RGBL.level,COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_ACCURATE_RGBL:
          (void)CLM_SetAccurateRGBL((LedNum_t)ledNo,color->iRatio,currTemperature[ledNo], color->RGBL.red, color->RGBL.green, color->RGBL.blue,(uint8_t)color->RGBL.level,COLOR_COMP_DIMMING_INTERVAL);
          break;

        case COLOR_MODE_ACCURATE_sRGBL:
          (void)CLM_SetAccurateSRGBL((LedNum_t)ledNo,color->iRatio,currTemperature[ledNo], color->RGBL.red, color->RGBL.green, color->RGBL.blue,(uint8_t)color->RGBL.level,COLOR_COMP_DIMMING_INTERVAL);
          break;

        case COLOR_MODE_ACCURATE_LUV:
          (void)CLM_SetAccurateLUV((LedNum_t)ledNo,color->iRatio,currTemperature[ledNo], color->LUV.u, color->LUV.v, (uint8_t)color->LUV.L, COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_RGB:
          (void)CLM_SetRGB((LedNum_t)ledNo,currTemperature[ledNo], color->RGB.red, color->RGB.green, color->RGB.blue,COLOR_COMP_DIMMING_INTERVAL);
          break;
        case COLOR_MODE_HSL:
          (void)CLM_SetHSL((LedNum_t)ledNo,currTemperature[ledNo], color->HSL.hue, color->HSL.saturation, color->HSL.level, COLOR_COMP_DIMMING_INTERVAL);
          break;
        default:
          break;
        }
    }
    ledNo++;
    if (ledNo >= (uint8_t)LED_NUM){
        ledNo = (uint8_t)LED0;
    }
}

