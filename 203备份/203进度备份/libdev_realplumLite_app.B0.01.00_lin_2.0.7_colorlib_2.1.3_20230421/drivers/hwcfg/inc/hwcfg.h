/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file hwcfg.h
 */

#ifndef HWCFG_H__
#define HWCFG_H__

#include <realplumLite.h>

typedef struct{
  uint16_t LED_2mA_TRIMCODE;
  uint16_t LED_2mA_Current_Value;/*  uA */
  uint16_t LED_30mA_TRIMCODE;
  uint16_t LED_30mA_Current_Value;/*  uA */
  uint16_t LED_PN1P5V_VOLT;
  uint16_t LED_PN1P5V_CODE;
  uint16_t LED_PN4P0V_VOLT;
  uint16_t LED_PN4P0V_CODE;
}LED_CH_PARAM_t;

typedef struct{
   uint16_t RESERVED_6;
   uint16_t RESERVED_7;
   uint16_t RESERVED_8;
   uint16_t RESERVED_9;
   uint16_t PN1P5V_VOLT_VBAT8V;
   uint16_t PN1P5V_CODE_VBAT8V;
   uint16_t PN4P0V_VOLT_VBAT8V;
   uint16_t PN4P0V_CODE_VBAT8V;
}LED_CH_PARAM_Vbat8V_t;
/**
 * @brief A structure to represent the data in hardware config block.
 */
typedef union {
    struct {
        struct {
            uint32_t crc32;
            struct {
                uint32_t  NAME;
                uint8_t  VERSION;
                uint8_t  LOT_NO[3];
                uint8_t  TESTTIME[4];
                uint32_t CHIPID;
                uint8_t  TEST_REV[4];
                uint8_t  CUSTOMER_INFO[16];
            }CHIP;/*20*/
            uint16_t RESVED00;
            uint16_t TSENSOR_CODE_25C;          /* 2 times of ADC code  */
            uint16_t LIN_SLEWRATE_TRIM_CODE;
            uint16_t LIN_DUTY_TRIM_CODE;  
            uint8_t  BANDGAP_CODE_UNUSED;
            uint8_t  RESVED0;
            uint16_t BANDGAP_VOLT_mV;           /* in mV */
            uint16_t ADC_0V5_CODE;              /* ADC count @0.5V */
            uint16_t ADC_1V0_CODE;              /* ADC count @1.0V */
            uint16_t TSENSOR_mV_25C;            
            uint16_t VREF1P1_TRIM_CODE;
            uint16_t VBAT_CODE_13V5;            /* ADC count @VBAT=13.5V*/
            uint16_t VBAT_CODE_8V0;             /* ADC count @VBAT=8.0V */
            uint16_t CLK_RC_LF_250KHz_TRIMCODE;
            uint16_t CLK_RC_HF_16MHz_TRIMCODE;
            uint16_t LDO3V3_TRIM_CODE;
            uint16_t LDO3V3_OCP_TRIM_CODE;
            uint16_t LDO1V5_TRIM_CODE;
            uint16_t LDO1V5_OCP_TRIM_CODE;
            uint16_t CLK_HF_16MHZ_VAULE;
            uint16_t LED_BIAS_TRIM_CODE;
        };
        LED_CH_PARAM_t  LED[3];
        LED_CH_PARAM_Vbat8V_t LED_VBAT8V[3];/*  resvd */
        
        
    };
    uint8_t payload[512];

} HWCFG_SFRS_t;

/**
 * @brief The starting address of hardware config block.
 */
#define HWCFG_SFRS ((__IO HWCFG_SFRS_t *) (0x00010200))
#define HWCFG_SFRS_START ((HWCFG_SFRS_t *) (0x00010200))

#define E_HWCFG_ADC_VBG              1220U       /* 1.22V */


uint8_t HWCFG_Get1V5CalibValue(void);
uint8_t HWCFG_Get1V5_OCPValue(void);
uint8_t HWCFG_Get3V3CalibValue(void);
uint8_t HWCFG_Get3V3_OCPValue(void);
uint8_t HWCFG_Get1V1CalibValue(void);


/**
 * @brief Get High Frequency RC clock trim value.
 *
 * @return The trim value stored in flash memory.
 */
uint8_t HWCFG_GetRCHFClockCalibValue(void);

/**
 * @brief Get Low Frequency RC clock trim value.
 *
 * @return The trim value stored in flash memory.
 */
uint8_t HWCFG_GetRCLFClockCalibValue(void);

/**
 * @brief Get band gap voltage.
 *
 * @return The trim value stored in flash memory.
 */
uint16_t HWCFG_GetVBG(void);

/**
 * @brief Get the version of calibration data.
 *
 * @return The version for this calibration data block.
 */
uint32_t HWCFG_GetCalibrationVersion(void);

uint8_t HWCFG_GetLedBiasTrimValue(void);
uint16_t HWCFG_GetLEDTrimValue(uint8_t ledIndex);
uint8_t HWCFG_GetOffLEDTrimValue(void);

uint8_t HWCFG_GetLinTxSlewRateTrimValue(void);
uint8_t HWCFG_GetLinDutyTrimValue(void);


void HWCFG_TrimAccessUnlock(void);
void HWCFG_TrimAccessLockUntilReset(void);


#endif /* __HWCFG_H__ */

