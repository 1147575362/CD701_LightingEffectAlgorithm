/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file pmu_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <stddef.h>
#include <pmu_device.h>
#include <errno.h>
#include <isrfuncs.h>
#include <gpio_device.h>
#include <lin_device.h>
#include <wica_sfr.h>
#include <pwm_device.h>
#include <wdt_device.h>

static __INLINE void WDTA_Start(void);

void BOR_Handler(void) 
{
    
}
   
void PMU_BORInit(Bor1V5Thres_t lowThreshold, Bor3V3Thres_t highThreshold)
{
    SYSCTRLA_SFRS->DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY    = 0x05U;
    PMUA_SFRS->TRIM.VDD3V3_LDO_TRIM = HWCFG_Get3V3CalibValue();
    PMUA_SFRS->TRIM.VDD1V5_LDO_TRIM = HWCFG_Get1V5CalibValue();
    PMUA_SFRS->TRIM.OCP_CTRL_3V3    = HWCFG_Get3V3_OCPValue();
    PMUA_SFRS->TRIM.OCP_CTRL_1V5    = HWCFG_Get1V5_OCPValue();
    PMUA_SFRS->TRIM.TRIM_VREF_BUF   = HWCFG_Get1V1CalibValue();
    
    CRGA_SFRS->BORCONFIG.S_BOR_1P5V = (uint8_t)lowThreshold;
    CRGA_SFRS->BORCONFIG.S_BOR_3P3V = (uint8_t)highThreshold;
    CRGA_SFRS->BORACTION.VDD1V5 = (uint8_t)PMU_BROWNOUT_RESET;
    CRGA_SFRS->BORACTION.VDD3V3 = (uint8_t)PMU_BROWNOUT_RESET;
    
    CRGA_SFRS->BORACTION.BOR_1V5_LOCK = 1U;
    CRGA_SFRS->BORACTION.BOR_3V3_LOCK = 1U;
    CRGA_SFRS->OVTEMPCONFIG.TEMPSENSE_EN = 1U; /* enable temperature sensor */
    EVTHOLD_SFRS->HOLD = 0U; /* clear  Lullaby_Handler after wake up  */
    
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U){}
    
    SYSCTRLA_SFRS->DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY    = 0x00U;
    
}

void PMU_WakeTimerInit(PMU_WAKEUP_TIMEER_MODE_t mode, PMU_WAKEUP_TIMEER_Interval_t interval)
{
  if (mode == WAKEUP_TIMEER_DISABLE){
      WICA_SFRS->CTRL.TIMER_ENA = 0U;
  }else{
      WICA_SFRS->CTRL.TIMER_TAPSEL = (uint8_t)interval;
      WICA_SFRS->CTRL.TIMER_ENA    = 1U;
  }
}

void PMU_EnterDeepSleepMode(void)
{
    CRGA_SFRS->MODULERSTREQ |= MODUE_SOFT_RESET_PWM;
    /* Add GPIO function here for low power mode*/

    for (uint8_t i = (uint8_t)GPIO_INIT_PORT_1; i<= (uint8_t)GPIO_INIT_PORT_4; i++ ){
		GPIO_Init((GpioInitPort_t)i,GPIO_DIR_INPUT,GPIO_PULL_DOWN);
    }

    /*  end of GPIO settigns   */
    CRGA_SFRS->OVTEMPCONFIG.TEMPSENSE_EN = 0U;     /* disbale temperature sensor when sleep */
    CRGA_SFRS->OVTEMPCONFIG.OVERTEMP_EN  = 0U;     
    PMUA_SFRS->VBATCTRL.BAT_UV_EN        = 0U;
    PMUA_SFRS->VBATCTRL.BAT_OV_EN        = 0U;
    
    SYSCTRLA_REG_PMU_ACCESS_KEY.PMU_ACCESS_KEY = 0x0A;
    PMUA_SFRS->CTRL.PD1V5_ENA_HIBERNATE  = 1U;

    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U){}	
	
    /* Enable wakeup detect and IRQ  when LINS is in sleep mode(LINS_REG_CTRL.SLEEP = 1U;)  */
	WICA_REG_CTRL.LINS_IRQCLR = 1U;
    WICA_REG_CTRL.LINS_ENA    = 1U;
    WICA_REG_CTRL.LINS_IRQENA = 1U;
    
    NVIC_EnableIRQ(WULINS_IRQn);
    NVIC_EnableIRQ(Lullaby_IRQn);               /*  Enable  Lullaby interrupt*/
    EVTHOLD_SFRS->HOLD = 1U;                    /*  Trigger ISR then entering sleep in ISR */
    /* Waiting to enter sleep mode */
    for(;;){
    }
}

/* Please make sure here and don't delete these functions!!!! or it would cause severe error*/
/* hibernate wake up interrupt ISR */
void Lullaby_Handler(void)
{
    /* Set lin to sleep mode */
    if (LINS_REG_CTRL.SLEEP == 1U){
        LINS_REG_CTRL.SLEEP = 0;
        IOCTRLA_SFRS->LIN.UPDATE   = 1U; 
        while(IOCTRLA_SFRS->LIN.UPDATE == 1U){} 
    }
    LINS_REG_CTRL.SLEEP = 1U;
    IOCTRLA_SFRS->LIN.UPDATE   = 1U; 
    while(IOCTRLA_SFRS->LIN.UPDATE == 1U){} 
    /*  Enter sleep mode  */
    PMUA_SFRS->CTRL.HIBERNATE = 1U;
}

#pragma optimize=none
static __INLINE void WDTA_Start(void)
{
    CRGA_REG_WDTACTION.WDTBARKCNTCLR = 1U;
    WDTA_SFRS->CTRL.TIMEOUTSEL = (uint8_t)WDTA_INTERVAL_9216MS;
    WDTA_SFRS->CLEAR = 0x3c574ad6U;
    WDTA_SFRS->STOP.STOP = 0U;/* start wdt*/
}


void NMI_Handler(void)
{
    WDTA_Start();
    for(;;){}
}

void HardFault_Handler(void)
{
    WDTA_Start();
    for(;;){}
}

void SVC_Handler(void)
{
    WDTA_Start();
    for(;;){}
}

void PendSV_Handler(void)
{
    WDTA_Start();
    for(;;){}
}

void BTE_Handler(void)
{
    WDTA_Start();
    for(;;){}
}

void SDIO_Handler(void)
{
    WDTA_Start();
    for(;;){}
}


