/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file systemInit.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <appConfig.h>
#include <systemInit.h>
#include <realplumLite.h>
#include <linSlaveTask.h>
#include <linStackTask.h>
#include <adc_device.h>
#include <pwm_device.h>
#include <wdt_device.h>

void leds_driver_Init(void);
void gpios_init(void);
void pmu_init(void);

void leds_driver_Init(void)
{
#if LED_NUM > 1U
    (void)PWM_SetLedCurrent();
    (void)LED_MULTI_ParamsInit();
    ADC_GeneralInit();

    IOCTRLA_SFRS->LEDPIN.VFW_ENA = 1U;                /* Enable 2mA led current source */
    IOCTRLA_SFRS->LEDPIN.SENSE_ENA = 1U;              /* LED Forward Voltage Sense Enable. Set to enable LED forward voltage sense module.*/
      
    SAR_CTRL_SFRS->ADCCHCONF.CH1SEL  = (uint8_t)ADC_CH_SEL_VBAT_GND;
    SAR_CTRL_SFRS->ADCCHCONF.CH2SEL  = (uint8_t)ADC_CH_SEL_TSENSOR_GND; 
    SAR_CTRL_SFRS->ADCCHCONF.CH3SEL  = (uint8_t)ADC_CH_SEL_VBAT_LED0;
    SAR_CTRL_SFRS->ADCCHCONF.CH4SEL  = (uint8_t)ADC_CH_SEL_LED0_GND; 
    
    SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1_CH2;   
    SAR_CTRL_SFRS->SARCFG.TRIGSEL = (uint8_t)ADC_TRIGGER_SOURCE_SOFT_INPUT;

    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1U;
    SAR_CTRL_SFRS->SARINT.ENABLE.INT_CONV_DONE_ENA = 1U;
    NVIC_EnableIRQ(ADC_IRQn);

    PWM_Init(PWM_PRESCALER_DIVIDE_1, PWM_VALUE_MAX);
    
#else
    (void)PWM_SetLedCurrent();
    PWM_Init(PWM_PRESCALER_DIVIDE_1, PWM_VALUE_MAX);    
#endif    
}


void gpios_init(void)
{
#if LED_NUM > 1U
    for (uint8_t i = (uint8_t)GPIO_INIT_PORT_1; i <= (uint8_t)GPIO_INIT_PORT_4; i++){
        GPIO_Init((GpioInitPort_t)i,GPIO_DIR_OUTPUT,GPIO_PULL_NONE);
    }
    GPIO_Set(GPIO_PORT_1, GPIO_LOW);
    GPIO_Set(GPIO_PORT_2, GPIO_LOW);
    GPIO_Set(GPIO_PORT_3, GPIO_LOW);
    GPIO_Set(GPIO_PORT_4, GPIO_LOW); 
#else
    /* Set gpios to input with power up resistor */
    for (uint8_t i = (uint8_t)GPIO_INIT_PORT_1; i <= (uint8_t)GPIO_INIT_PORT_4; i++){
        GPIO_Init((GpioInitPort_t)i,GPIO_DIR_INPUT,GPIO_PULL_DOWN);
    }
    //GPIO_Init(GPIO_INIT_PORT_4, GPIO_DIR_OUTPUT,GPIO_PULL_NONE);
    //GPIO_Set(GPIO_PORT_4, GPIO_HIGH);   /* LED is OFF  */
    
#endif
}

void pmu_init(void)
{
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY        = 0x0EU;
    SYSCTRLA_REG_PMU_ACCESS_KEY.PMU_ACCESS_KEY          = 0x0AU;
    /* Init set BOR voltage level for cpu low voltage safety*/
    PMU_BORInit(BOR_1V5_THRS_1328mV, BOR_3V3_THRS_2998mV);
    /* Disable wake up timer */
    PMU_WakeTimerInit(WAKEUP_TIMEER_DISABLE, WAKEUP_TIMEER_INTERVAL_32768ms);
}


void SYS_Init(void)
{
    /* Enable trim revise access enable*/
    HWCFG_TrimAccessUnlock();
    CRGA_SFRS->MODULERSTREQ = 0xFFU;
    /* Init system clock */
    Clock_SystemMainClockInit(SYS_MAIN_CLOCK_DIV);
    pmu_init();
    /* Init global timer engine for driving soft timer */
    SysTick_Init(SOFT_TIMER_INTERVAL *1000U * MAIN_CPU_CLOCK, SoftTimer_ExpireCallback);
#if WATCH_DOG_EN == 1U
      WDTA_Enable(WDTA_INTERVAL_9216MS); /* 9s */
#endif
    /* Init gpios settings */
    gpios_init();
    /* Init LED current and PWM settings */
    leds_driver_Init();
    /*HWCFG_TrimAccessLockUntilReset();*/
    
    /* tasks init must be called before use. */
    TM_PostTask(TASK_ID_SOFT_TIMER);
    TM_PostTask(TASK_ID_SAFETY_MONITOR);
    TM_PostTask(TASK_ID_LINS);
    TM_PostTask(TASK_ID_PDS);
    TM_PostTask(TASK_ID_ADC_MEASURE);
    TM_PostTask(TASK_ID_COLOR_MIXING);
    TM_PostTask(TASK_ID_COLOR_COMPENSATION);
    TM_PostTask(TASK_ID_APPL);
}



