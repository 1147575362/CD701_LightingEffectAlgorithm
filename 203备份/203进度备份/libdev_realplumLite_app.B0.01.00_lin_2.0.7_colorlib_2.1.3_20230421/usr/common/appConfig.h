#ifndef APP_CONFIG_H__
#define APP_CONFIG_H__

#include <clock_device.h>
#include <pwm_device.h>
#include <string.h>

#define PRAGMA(x) _Pragma(#x)
#define BEGIN_PACK PRAGMA(pack(push, 1))
#define END_PACK   PRAGMA(pack(pop))

#define SDK_APPL_VERSION        (0x0100U)

#define LIN_STACK_TYPE_LIN2_2A          (0U)
#define LIN_STACK_TYPE_SAEJ2602         (1U)
#define LIN_STACK_TYPE_PRIVATE          (2U)
#define LIN_STACK_TYPE                  LIN_STACK_TYPE_LIN2_2A

/* ****************************hardware config ********************************/
#define MAIN_CPU_CLOCK                          (16U)       /*MHz*/
#define PTAT_MEAS_USEING_RAW_ADC_VOLT           (1U)
#define LIN_SLEW_RATE_CTRL_EN                   (1U)
/* *****************************PWMs config ***********************************/
/* pwm resolution settings*/
#define PWM_VALUE_MAX_POS               (16U)
#define PWM_VALUE_MAX                   ((1UL << PWM_VALUE_MAX_POS) -1UL)

/* rgb channels settings*/
#define PHY_CHANNEL_R           PWM_CHANNEL_2
#define PHY_CHANNEL_G           PWM_CHANNEL_0
#define PHY_CHANNEL_B           PWM_CHANNEL_1

#define LED_NUM    (1U)

#define ROOM_TEMPERATURE        (24)  /*Celsius degree*/
#define WATCH_DOG_EN            (0U)
/******************************************************************************/



#if MAIN_CPU_CLOCK == 16U
    #define SYS_MAIN_CLOCK_DIV    CLOCK_DIV_1
#else
    #error MAIN_CPU_CLOCK MUST BE 16MHz!*/
#endif

#if LED_NUM == 0U || LED_NUM > 4U
    #error LED_NUM must be within 1-4 leds!*/
#endif

/* ********************************debug config *******************************/
#define ENABLE_FUNCTION_VALIDATION              (0U)
#define CODE_DEBUG_EN    0

#if CODE_DEBUG_EN == 1
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #define DEBUG_OUT(...) printf(__VA_ARGS__)
#else
  #define DEBUG_OUT(...)      
#endif



#endif