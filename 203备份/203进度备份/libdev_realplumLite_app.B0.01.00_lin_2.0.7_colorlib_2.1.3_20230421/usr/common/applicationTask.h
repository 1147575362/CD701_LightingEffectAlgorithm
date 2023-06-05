#ifndef _APPLICATION_H
#define _APPLICATION_H
#include <appConfig.h>
#include <softTimerTask.h>
#include <systick_device.h>
#include <taskManager.h>

#include <ColorMixingTask.h>
#include <lin_device.h>
#include <adc_device.h>
#include <gpio_device.h>
#include <ledParamTables.h>
#include <timer_device.h>
#include <pmu_device.h>
#include <linStackTask.h>


typedef enum{
    CMD_COLOR_CTRL_XYY  = 0x00,
    CMD_COLOR_CTRL_HSL  = 0x01,
    CMD_COLOR_CTRL_RGBL = 0x02,
    CMD_COLOR_CTRL_RGB  = 0x03,
    CMD_COLOR_CTRL_PWM  = 0x04,
    CMD_COLOR_CTRL_LUV  = 0x05,
    CMD_COLOR_CTRL_XYY_EXT  = 0x06,
    CMD_COLOR_CTRL_XY_ABS_Y    = 0x07,
    CMD_COLOR_CTRL_ABS_L_UV    = 0x08,
    CMD_COLOR_CTRL_sRGB_ABS_L  = 0x09,
}ColorCtrlCommand_t;


typedef struct
{
	uint32_t RGB;
	float Cx;
	float Cy;
}RGB_xy_t;	//copy

typedef struct
{
	uint8_t ALU_FID;
	uint16_t ALU_LED;
}ALU_PRAM_t;	//copy

typedef union{
    struct{
        uint8_t nodeAddress;
        uint8_t channel         :4;
        uint8_t command         :4;
    };
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint16_t x;
        uint16_t y;
        uint8_t  Y;
        uint8_t transitionTime;/* 100ms/count */
    }ColorXY;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint16_t hue;
        uint8_t  saturation;
        uint8_t  level;
        uint8_t  transitionTime;/* 100ms/count */
    }ColorHSL;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint8_t  red;
        uint8_t  green;
        uint8_t  blue;
        uint8_t  level;
        uint8_t  transitionTime;/* 100ms/count */
    }ColorRGBL;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint8_t  red;
        uint8_t  green;
        uint8_t  blue;
        uint8_t  transitionTime;/* 100ms/count */
    }ColorRGB;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint8_t  red;
        uint8_t  green;
        uint8_t  blue;
        uint8_t transitionTime;/* 100ms/count */
    }ColorPWM;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint16_t u;
        uint16_t v;
        uint8_t  level;
        uint8_t  transitionTime;/* 100ms/count */
    }ColorLUV;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint16_t x;
        uint16_t y;
        uint16_t Y;
    }ColorXY_EXT;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint16_t x;
        uint16_t y;
        uint16_t intensity;
    }ColorXY_Intensity;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint16_t u;
        uint16_t v;
        uint16_t intensity;
    }ColorIntensity_UV;
    struct{
        uint8_t  nodeAddress;
        uint8_t  channel         :4;
        uint8_t  command         :4;
        uint8_t  red;
        uint8_t  green;
        uint8_t  blue;
        uint8_t  reserved;
        uint16_t intensity;
    }ColorSRGB_Intensity;
    
    uint8_t  data[LIN_BUFF_SIZE];       /*!< data buffer. */
}ColorCtrlFrame_t;


#define APP_DIAG_GET_LED_PHY_PARAM       (0x00U)
#define APP_DIAG_SET_LED_PHY_PARAM       (0x01U)
#define APP_DIAG_GET_LED_GENERAL_PARAM   (0x02U)
#define APP_DIAG_SET_LED_GENERAL_PARAM   (0x03U)
#define APP_DIAG_GET_LED_TYPICAL_PN_VOLT (0x04U)
#define APP_DIAG_SET_LED_TYPICAL_PN_VOLT (0x05U)
#define APP_DIAG_GET_LED_RUN_TIME_INFO   (0x06U)
#define APP_DIAG_GET_LED_STATIC_PN_VOLT  (0x07U)
#define APP_DIAG_GET_LED_RGB_RUN_TEMP    (0x08U)
#define APP_DIAG_GET_LED_RGB_RUN_VOLT    (0x09U)

#define APP_DIAG_GET_SDK_VERSION         (0x0FU)

BEGIN_PACK

typedef struct{
  LedNum_t         ledIndex;
  int8_t           temperature[3];
  LedcolorParam_t  colorParam;
}CommLedPhysicalParam_t;

typedef struct{
  LedNum_t         ledIndex;
  int8_t           temperature;
  int16_t          ledTypicalPNVolt[3];   /* @25C*/
}CommLedTypicalPNVolt_t;

typedef struct{
  Coordinate_t     whitePoint;
  uint16_t         minIntensity[3];       /* size: 6bytes min rgb Intensity, red has more degration */
}CommLedGeneralParam_t;

typedef struct{
  uint16_t battVolt;
  uint16_t buckVolt;
  int16_t  chipTemp; 
  uint16_t ledNum;
  int16_t  ledTemperature[LED_NUM];
}CommLedRunTimeParam_t;

typedef struct{
  LedNum_t         ledIndex;
  uint8_t          reserved;
  uint16_t         ledPNVolt[3];
}CommLedStaticPNVolt_t;

typedef struct{
  LedNum_t         ledIndex;
  uint8_t          reserved;
  uint16_t         ledPNVolt[3];
  uint16_t battVolt;
  uint16_t resvd;
  int16_t  chipTemp;
}CommLedRunTimePNVolt_t;

typedef struct{
  LedNum_t         ledIndex;
  uint8_t          reserved;
  int8_t           ledTemp[3];
}CommLedRunTimePNTemp_t;

typedef struct{
  uint16_t              sdkApplication;
  lin_stack_version_t   linStack;
  LibVersionInfo_t      ledLibrary;
}CommVersion_t;

typedef struct{
  uint16_t command;
  union{
      CommLedPhysicalParam_t    ledPhysicalParam;
      CommLedTypicalPNVolt_t    ledTypicalPNVolt;
      CommLedGeneralParam_t     ledGeneralParam;
      CommLedRunTimeParam_t     ledRunTimeParam;
      CommLedStaticPNVolt_t     ledStaticPNVolt;
      CommLedRunTimePNVolt_t    ledRunTimePNVolt;
      CommLedRunTimePNTemp_t    LedRunTimePNTemp;
      CommVersion_t             Version;
  };
}CommLedParamInfo_t;

END_PACK


uint8_t APPL_GetLEDState(void);

void APPL_TaskHandler(void);
void APPL_HandleColorControlCommands(LIN_Device_Frame_t const *frame);
/* Response enable  */
uint8_t APPL_PrepareLedParamRequest(uint8_t reqSid,uint8_t *const dataBuff, DiagRspInfo_t *const diagRsp);
uint8_t APPL_HandleLedParamConfigRequest(uint8_t reqSid,uint8_t *const dataBuff,DiagRspInfo_t *const diagRsp);


#endif