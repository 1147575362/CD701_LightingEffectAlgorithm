#ifndef __HW_PROTECTION_H__
#define __HW_PROTECTION_H__
#include <realplumLite.h>
#include <core_cm0.h>


typedef enum{
  OVT_PROT_93C = 0,
  OVT_PROT_98C,
  OVT_PROT_104C,
  OVT_PROT_109C,
  OVT_PROT_115C,
  OVT_PROT_121C,
  OVT_PROT_127C,
  OVT_PROT_133C,
  OVT_PROT_139C,
  OVT_PROT_146C,
  OVT_PROT_153C,
  OVT_PROT_160C,
  OVT_PROT_168C,
  OVT_PROT_176C,
  OVT_PROT_185C,
  OVT_PROT_195C,
}OverTempThres_t;

typedef enum{
  OVT_ACTION_HW_RESET = 0,
  OVT_ACTION_NONE,
}OverTempAction_t;

typedef enum{
  OV_VOLT_14_7V = 0U,
  OV_VOLT_15_2V,
  OV_VOLT_15_6V,
  OV_VOLT_16_4V,
  OV_VOLT_16_8V,
  OV_VOLT_17_6V,
  OV_VOLT_18_3V,
  OV_VOLT_19_0V,
  OV_VOLT_20_0V,
  OV_VOLT_20_7V,
}OverVoltThres_t;

typedef enum{
  OV_HYS_02_PERCENT = 0U,  /*  3600mV @15V */
  OV_HYS_06_PERCENT,       /*  720mV  @15V*/
  OV_HYS_09_PERCENT,       /*  1080mV @15V */
  OV_HYS_12_PERCENT,       /*  1440mV @15V */
}OverVoltHysteresis_t;


typedef enum{
  UV_VOLT_4_615V = 0x0,
  UV_VOLT_4_685V, 
  UV_VOLT_4_755V,
  UV_VOLT_4_825V, 
  UV_VOLT_4_905V, 
  UV_VOLT_4_975V,
  UV_VOLT_5_055V, 
  UV_VOLT_5_135V,
  
  UV_VOLT_5_225V, 
  UV_VOLT_5_315V, 
  UV_VOLT_5_405V,
  UV_VOLT_5_495V, 
  UV_VOLT_5_585V,
  UV_VOLT_5_685V, 
  UV_VOLT_5_795V, 
  UV_VOLT_5_895V,
  
  UV_VOLT_6_015V, 
  UV_VOLT_6_125V,
  UV_VOLT_6_245V, 
  UV_VOLT_6_375V, 
  UV_VOLT_6_505V,
  UV_VOLT_6_635V, 
  UV_VOLT_6_775V,
  UV_VOLT_6_925V,
  
  UV_VOLT_7_075V, 
  UV_VOLT_7_235V, 
  UV_VOLT_7_405V,
  UV_VOLT_7_585V,
  UV_VOLT_7_765V, 
  UV_VOLT_7_965V,
  UV_VOLT_8_165V,
  UV_VOLT_8_385V, 
  
  UV_VOLT_8_605V, 
  UV_VOLT_8_845V,  
  UV_VOLT_9_095V,
  UV_VOLT_9_365V, 
  UV_VOLT_9_655V, 
  UV_VOLT_9_955V,
  UV_VOLT_10_27V,
  UV_VOLT_10_61V,
}UnderVoltThres_t;

typedef enum{
  UV_HYS_04_PERCENT = 0U,    /*  475mV @15V */
  UV_HYS_06_PERCENT,         /*  835mV @15V */
  UV_HYS_08_PERCENT,        /*  1225mV @15V */
  UV_HYS_10_PERCENT,        /*  1635mV @15V */
}UnderVoltHysteresis_t;


typedef enum{
  VOLT_DEBOUNCE_TIME_0ms  = 0U,
  VOLT_DEBOUNCE_TIME_1ms  = 16U,
  VOLT_DEBOUNCE_TIME_2ms  = 32U,
  VOLT_DEBOUNCE_TIME_3ms  = 48U,
  VOLT_DEBOUNCE_TIME_4ms  = 64U,
  VOLT_DEBOUNCE_TIME_5ms  = 80U,
  VOLT_DEBOUNCE_TIME_6ms  = 96U,
  VOLT_DEBOUNCE_TIME_7ms  = 112U,
  VOLT_DEBOUNCE_TIME_8ms  = 128U,
  VOLT_DEBOUNCE_TIME_9ms  = 144U,
  VOLT_DEBOUNCE_TIME_10ms = 160U,
  VOLT_DEBOUNCE_TIME_11ms = 176U,
  VOLT_DEBOUNCE_TIME_12ms = 192U,
  VOLT_DEBOUNCE_TIME_13ms = 208U,
  VOLT_DEBOUNCE_TIME_14ms = 224U,
  VOLT_DEBOUNCE_TIME_15ms = 240U,
}VoltDebouceTime_t;





typedef void (*hwProtIsrCallback_t)(void);


void HW_PROT_SetOverTmperature(OverTempThres_t threshold, OverTempAction_t action);
void HW_PROT_RegisterOVTemperatureIRQ(hwProtIsrCallback_t callback);
void HW_PROT_UnRegisterOVTemperatureIRQ(void);

void HW_PROT_SetOverVoltage(OverVoltThres_t threshold,OverVoltHysteresis_t hys, VoltDebouceTime_t triggerTime, VoltDebouceTime_t releaseTime);
void HW_PROT_RegisterOverVoltageIRQ(hwProtIsrCallback_t callback);
void HW_PROT_UnRegisterOverVoltageIRQ(void);

void HW_PROT_SetUnderVoltage(UnderVoltThres_t threshold,UnderVoltHysteresis_t hys, VoltDebouceTime_t triggerTime, VoltDebouceTime_t releaseTime);
void HW_PROT_RegisterUnderVoltageIRQ(hwProtIsrCallback_t callback);
void HW_PROT_UnRegisterUnderVoltageIRQ(void);

/* return 0: normal , 1: low, 2:high */
uint8_t HW_PROT_GetBattVoltState(void);
/* return 0: normal , others : over heat */
uint8_t HW_PROT_ChipIsOverHeat(void);




#endif