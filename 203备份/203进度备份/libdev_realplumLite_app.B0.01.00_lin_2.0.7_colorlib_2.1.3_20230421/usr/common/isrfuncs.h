#ifndef ISR_FUNCS_H_
#define ISR_FUNCS_H_

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void WULINS_Handler(void);
void IOCTRLA_Handler(void);
void WUTIMER_Handler(void);
void BOR_Handler(void);
void WatchdogA_Handler(void);

void UV_Handler(void);
void OV_Handler(void);
void LINS_Handler(void);
void ADC_Handler(void);
void PWM_Handler(void);
void PWMAUX_Handler(void);


void GPIO_Handler(void);
void WULINM_Handler(void);
void OVTEMP_Handler(void);


void Lullaby_Handler(void);
void Timer0_Handler(void);
void Timer1_Handler(void);
void Timer2_Handler(void);
void Watchdog_Handler(void);
void BTE_Handler(void);
void SDIO_Handler(void);




#endif