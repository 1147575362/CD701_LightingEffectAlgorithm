/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file hwProtection.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <hwProtection.h>
#include <appConfig.h>
#include <isrfuncs.h>

static hwProtIsrCallback_t otCallback = NULL;
static hwProtIsrCallback_t uvCallback = NULL;
static hwProtIsrCallback_t ovCallback = NULL;

/*  !!!!UV is a level interrupt   */
void UV_Handler(void)
{
    if (uvCallback != NULL){
        uvCallback();
    }
    NVIC_DisableIRQ(UV_IRQn);
    PMUA_SFRS->PMUIRQ.CLEAR.UV  = 1U;
    
}

/*  !!!!OV is a level interrupt   */
void OV_Handler(void)
{
    if (ovCallback != NULL){
        ovCallback();
    }
    NVIC_DisableIRQ(OV_IRQn);
    PMUA_SFRS->PMUIRQ.CLEAR.OV;
}

void OVTEMP_Handler(void)
{
    if (otCallback != NULL){
        otCallback();
    }
    NVIC_DisableIRQ(OVTEMP_IRQn);
    CRGA_SFRS->RESETCTRL.OVTEMPFLAGCLR = 1U;
}


void HW_PROT_SetOverTmperature(OverTempThres_t threshold, OverTempAction_t action)
{
    CRGA_SFRS->OVTEMPCONFIG.VTEMP_SEL = (uint8_t)threshold;
    CRGA_SFRS->OVTEMPACTION.OVTEMP      = (uint8_t)action;
    CRGA_SFRS->OVTEMPCONFIG.TEMPSENSE_EN = 0U;    /* enable temperature sensor */
    CRGA_SFRS->OVTEMPCONFIG.OVERTEMP_EN  = 1U;    /* enable ov temperature action */ 
    
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U){}
}

void HW_PROT_RegisterOVTemperatureIRQ(hwProtIsrCallback_t callback)
{
    otCallback = callback;
    CRGA_SFRS->RESETCTRL.OVTEMPFLAGCLR = 1U;
    NVIC_EnableIRQ(OVTEMP_IRQn);
}

void HW_PROT_UnRegisterOVTemperatureIRQ(void)
{
    NVIC_DisableIRQ(OVTEMP_IRQn);
    CRGA_SFRS->RESETCTRL.OVTEMPFLAGCLR = 1U;
    otCallback = NULL;
}

void HW_PROT_SetOverVoltage(OverVoltThres_t threshold,OverVoltHysteresis_t hys, VoltDebouceTime_t triggerTime, VoltDebouceTime_t releaseTime)
{
    PMUA_SFRS->VBATTRIM.OVLEVEL = (uint8_t)threshold;
    PMUA_SFRS->VBATTRIM.OVHYS   = (uint8_t)hys;
    
    PMUA_SFRS->VBATDBNC.OVSTRB0SEL = 1U; /*  16KHz source  */
    PMUA_SFRS->VBATDBNC.OVSTRB1SEL = 1U; /*  16KHz source  */
    
    PMUA_SFRS->VBATDBNCTHRES.OVTHRES1 = (uint8_t)triggerTime;/* value*62.5us when 16Khz*/
    PMUA_SFRS->VBATDBNCTHRES.OVTHRES0 = (uint8_t)releaseTime;/* value*62.5us when 16Khz*/
    
    PMUA_SFRS->VBATCTRL.BAT_OV_EN = 1U;
    
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U){}
}

void HW_PROT_RegisterOverVoltageIRQ(hwProtIsrCallback_t callback)
{
    ovCallback = callback;
    PMUA_SFRS->PMUIRQ.CLEAR.OV  = 1U;
    PMUA_SFRS->PMUIRQ.ENABLE.OV = 1U;
    NVIC_EnableIRQ(OV_IRQn);
}

void HW_PROT_UnRegisterOverVoltageIRQ(void)
{
    NVIC_DisableIRQ(OV_IRQn);
    PMUA_SFRS->PMUIRQ.ENABLE.OV = 0U;
    PMUA_SFRS->PMUIRQ.CLEAR.OV  = 1U;
    ovCallback = NULL;
}


void HW_PROT_SetUnderVoltage(UnderVoltThres_t threshold,UnderVoltHysteresis_t hys, VoltDebouceTime_t triggerTime, VoltDebouceTime_t releaseTime)
{
    PMUA_SFRS->VBATTRIM.UVLEVEL = (uint8_t)threshold;
    PMUA_SFRS->VBATTRIM.UVHYS   = (uint8_t)hys;
    
    PMUA_SFRS->VBATDBNC.UVSTRB0SEL = 1U; /*  16KHz source  */
    PMUA_SFRS->VBATDBNC.UVSTRB1SEL = 1U; /*  16KHz source  */
    
    PMUA_SFRS->VBATDBNCTHRES.UVTHRES1 = (uint8_t)triggerTime; /* value*62.5us when 16Khz*/
    PMUA_SFRS->VBATDBNCTHRES.UVTHRES0 = (uint8_t)releaseTime; /* value*62.5us when 16Khz*/
    
    PMUA_SFRS->VBATCTRL.BAT_UV_EN     = 1U;
    
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U){}
}

void HW_PROT_RegisterUnderVoltageIRQ(hwProtIsrCallback_t callback)
{
    uvCallback = callback;
    PMUA_SFRS->PMUIRQ.CLEAR.UV  = 1U;
    PMUA_SFRS->PMUIRQ.ENABLE.UV = 1U;
    NVIC_EnableIRQ(UV_IRQn);
}

void HW_PROT_UnRegisterUnderVoltageIRQ(void)
{
    NVIC_DisableIRQ(UV_IRQn);
    PMUA_SFRS->PMUIRQ.ENABLE.UV = 0U;
    PMUA_SFRS->PMUIRQ.CLEAR.UV  = 1U;
    uvCallback = NULL;
}


uint8_t HW_PROT_GetBattVoltState(void)
{
    uint8_t result = (uint8_t)PMUA_SFRS->VBATCTRL.BATT_STATE;
    return result;
}

uint8_t HW_PROT_ChipIsOverHeat(void)
{
    uint8_t result = (uint8_t)CRGA_SFRS->RESETCTRL.OVTEMPFLAG;
    return result;
}




