#ifndef PWM_AUX_DEVICE_H__
#define PWM_AUX_DEVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include <realplumLite.h>

typedef void (*PwmAuxIsrCallback_t)(void);

typedef enum{
  PWMAUX_PORT1_CH0 = 0U,
  PWMAUX_PORT2_CH1,
  PWMAUX_PORT3_CH2,
  PWMAUX_PORT4_CH3,
}PWMAUX_Port_CH_t;


typedef enum{
  PWMAUX_BASE_0 = 0U,
  PWMAUX_BASE_1,
}PWMAUX_ClockSource_t;


typedef enum{
  PWMAUX_CLK_DIV_1 = 0U,
  PWMAUX_CLK_DIV_2,
  PWMAUX_CLK_DIV_4,
  PWMAUX_CLK_DIV_8,
  PWMAUX_CLK_DIV_16,
  PWMAUX_CLK_DIV_64,
  PWMAUX_CLK_DIV_256,
  PWMAUX_CLK_DIV_1024,
}PWMAUX_Prescaler_t;


void PWMAUX_Init(PWMAUX_Port_CH_t port,PWMAUX_ClockSource_t source,PWMAUX_Prescaler_t devider,uint16_t peroid);
void PWMAUX_RegisterReloadIRQ(PWMAUX_ClockSource_t source, PwmAuxIsrCallback_t callback);
void PWMAUX_UnRegisterReloadIRQ(PWMAUX_ClockSource_t source);
void PWMAUX_SetMatchValue(PWMAUX_Port_CH_t channel, uint16_t matchRisingValue, uint16_t matchFaillValue);


#endif