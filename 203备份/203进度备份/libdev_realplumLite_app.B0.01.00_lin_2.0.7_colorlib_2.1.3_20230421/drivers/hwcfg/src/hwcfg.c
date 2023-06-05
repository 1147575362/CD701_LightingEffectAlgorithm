/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file hwcfg.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <stddef.h>
#include <stdint.h>
#include <realplumLite.h>
#include "hwcfg.h"


uint8_t HWCFG_Get1V5CalibValue(void)
{
    uint8_t trim = (uint8_t)HWCFG_SFRS->LDO1V5_TRIM_CODE;
    if (trim == 0xFFU){
        trim = 0x00;
    }
    return trim;
}

uint8_t HWCFG_Get1V5_OCPValue(void)
{
    uint8_t trim = (uint8_t)HWCFG_SFRS->LDO1V5_OCP_TRIM_CODE;
    if (trim == 0xFFU){
        trim = 0x04;
    }
    return trim;
}

uint8_t HWCFG_Get3V3CalibValue(void)
{
    uint8_t trim = (uint8_t)HWCFG_SFRS->LDO3V3_TRIM_CODE;
    if (trim == 0xFFU){
        trim = 0x00;
    }
    return trim;
}

uint8_t HWCFG_Get3V3_OCPValue(void)
{
    uint8_t trim = (uint8_t)HWCFG_SFRS->LDO3V3_OCP_TRIM_CODE;
    if (trim == 0xFFU){
        trim = 0x08;
    }
    return trim;
}

uint8_t HWCFG_Get1V1CalibValue(void)
{
    uint8_t trim = (uint8_t)HWCFG_SFRS->VREF1P1_TRIM_CODE;
    if (trim == 0xFFU){
        trim = 0x00;
    }
    return trim;
}


uint8_t HWCFG_GetRCHFClockCalibValue(void)
{
    uint8_t trim = (uint8_t)HWCFG_SFRS->CLK_RC_HF_16MHz_TRIMCODE;
    if (trim == 0xFFU){
        trim = 0xB6;
    }
    return trim;
}

  
uint8_t HWCFG_GetRCLFClockCalibValue(void)
{
    uint8_t trim =  (uint8_t)HWCFG_SFRS->CLK_RC_LF_250KHz_TRIMCODE;
    if (trim == 0xFFU){
        trim = 0xAAU;
    }
    return trim;
}


uint16_t HWCFG_GetVBG(void)
{
    uint16_t volt = HWCFG_SFRS->BANDGAP_VOLT_mV;/* 2*VBG  */
    
    if (volt == 0xFFU){
        volt = E_HWCFG_ADC_VBG;
    }
    return volt;
}

/**
 * @brief Get the version of calibration data.
 *
 * @return The version for this calibration data block.
 */
uint32_t HWCFG_GetCalibrationVersion(void)
{
    uint32_t version = HWCFG_SFRS->CHIP.VERSION;
    return version;
  
}

uint8_t HWCFG_GetLedBiasTrimValue(void)
{
    uint8_t trimValue = (uint8_t)HWCFG_SFRS->LED_BIAS_TRIM_CODE;
    if (trimValue == 0xFFU){
        trimValue =  0x80U;
    }
    return trimValue;
}


uint16_t HWCFG_GetLEDTrimValue(uint8_t ledIndex)
{
    uint16_t trimValue = HWCFG_SFRS->LED[ledIndex].LED_30mA_TRIMCODE;
    if (trimValue > 0x1FFU){
        trimValue =  250U; /* 120uA*250 = 30mA */
    }
    return trimValue;
}


uint8_t HWCFG_GetOffLEDTrimValue(void)
{
    uint8_t trimValue = (uint8_t)HWCFG_SFRS->LED[0].LED_2mA_TRIMCODE;
    
    if (trimValue >= 255U){
        trimValue = 200U; 
    }
    return trimValue;
}


void HWCFG_TrimAccessUnlock(void)
{
    SYSCTRLA_SFRS->TRIM_ACCESS_KEY.TRIM_ACCESS_KEY = 0x0EU;
}


void HWCFG_TrimAccessLockUntilReset(void)
{
    SYSCTRLA_SFRS->TRIM_ACCESS_KEY.TRIM_LOCK = 1U;
}

uint8_t HWCFG_GetLinDutyTrimValue(void)
{
    uint16_t trimValue = HWCFG_SFRS->LIN_DUTY_TRIM_CODE;
    
    if (trimValue >= 8U){
        trimValue = 5U;
    }
    return ((uint8_t)trimValue);
}

uint8_t HWCFG_GetLinTxSlewRateTrimValue(void)
{
    uint16_t trimValue = HWCFG_SFRS->LIN_SLEWRATE_TRIM_CODE;
    
    if (trimValue >= 8U){
        trimValue = 7U;
    }
    return ((uint8_t)trimValue);
}
    
