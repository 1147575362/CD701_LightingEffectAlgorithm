#include <appConfig.h>
#if ENABLE_FUNCTION_VALIDATION == 1


#include <realplumLite.h>
#include <validation.h>
#include <gpio_device.h>
#include <adc_device.h>
#include <clock_device.h>
#include <lin_device.h>
#include <hwProtection.h>
#include <adc_device.h>
#include <pwm_aux_device.h>

SYSCTRLA_SFRS_t sysCtrl;

uint32_t convertDown = 0;
int32_t sum = 0;
int32_t buff[16];
uint8_t buffCount = 0;

void  LED_Current(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0E;
    IOCTRLA_SFRS->LEDPIN.HWMODE = 0U;
    IOCTRLA_SFRS->LEDPIN.DATA =0x07U;
    
    SYSCTRLA_SFRS->LEDBIASTRIM = 190;
    SYSCTRLA_SFRS->LED[0].TRIM = 250U;
    SYSCTRLA_SFRS->LED[1].TRIM = 250U;
    SYSCTRLA_SFRS->LED[2].TRIM = 250U;
    PMUA_REG_CTRL.UPDATE = 1U;
    while(1);
}


void  Major_BandgapOUT(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.LEDSEL     = 2;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONREG = 0x10;      //Bandgap-> LED1
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONSEL = 1;
    while(1);
}

void  Tempsensor_OUT(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.LEDSEL = 2;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONREG = 0x40;      //Tempsensor-> LED1
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONSEL = 1;
    while(1);
}

void  ADC_REFP_OUT(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    SAR_CTRL_SFRS->AFECTRL.SARAFEEN     = 1U;
    SAR_CTRL_SFRS->SARANACFG.ADCVREFSEL = (uint8_t)ADC_REF_VBG_2TIMES;
    SAR_CTRL_SFRS->SARANACFG.SARPREAMPEN = 1;
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ = 1U;
    PMUA_REG_CTRL.UPDATE = 1U;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.LEDSEL = 2;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONREG = 0x80;      //ADC_REFP-> LED1
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONSEL = 1;
    while(1);
}

void  VDD1V5_OUT(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.LEDSEL = 1;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONREG = 0x2;       //VDD1V5-> LED0
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONSEL = 1;
    while(1);
}

void  VDD3V3_Analog_OUT(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.LEDSEL = 1;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONREG = 0x1;       //VDD3V3-> LED0
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONSEL = 1;
    while(1);
}

void SET_VDD_PRE5V_OUT(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;     //ADC_REFP-> LED0
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.LEDSEL = 1;
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONREG = 4;         
    IOCTRLA_SFRS->ANALOGTESTMUXOVERRIDE.GPIOCONSEL = 1;
    while(1);
}




void LF_32KHz_OUT(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0E;
    IOCTRLA_REG_GPIO1.MUXSEL = 0x7U;                    /* 0x7: CRGA (Clock Reset Generation Asic) (scan_test_mode | lf_rc_clk) */
    IOCTRLA_REG_GPIO1.HWMODE = 2;                       //LF-> GPIO1
    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCENA   = 0U;
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U);
    
    while(1);
}

void HF_16MHz_OUT(void)
{
//    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
//    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0E;
//    IOCTRLA_REG_GPIO1.GPIO2_MUSEL = 0x8U;       /* 0x8: CRGA (Clock Reset Generation Asic) (scan_test_mode | hf_rc_clk) */
//    IOCTRLA_REG_GPIO2.GPIO2HW_MODE_MSB = 1;
//    IOCTRLA_REG_GPIO2.HWMODE = 0;               /*  HF-> GPIO2  */
//    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCENA   = 0U;
//    PMUA_REG_CTRL.UPDATE = 1U;
//    while(PMUA_REG_CTRL.UPDATE == 1U);
}


void  SSC_TEST(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0E;
    sysCtrl = *SYSCTRLA_SFRS;
    CRGA_SFRS->SYSCLKCTRL.HFRCENA = 1U;
    while(CRGA_SFRS->SYSCLKCTRL.HFRCSTS == 0U){}
    SYSCTRLA_SFRS->HF_OSC_TRIM.TRIM_HF_RC = 0x42U;
    CRGA_SFRS->SYSCLKCTRL.SYSCLKSEL = 1;
    CRGA_SFRS->SYSCLKCTRL.DIVSYSCLK = 0;
    /*  frequency spread    */
    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCDEEP  = 7U;
    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCDIV   = 255U;
    SYSCTRLA_SFRS->HF_OSC_TRIM.SSCENA   = 0U;
    IOCTRLA_REG_GPIO1.HWMODE = 2;               /*  HF-> GPIO1  */
    IOCTRLA_REG_GPIO1.MUXSEL = 0x8;             /*0x8: CRGA (Clock Reset Generation Asic) (scan_test_mode | hf_rc_clk)*/
    
    while(1);
}


void  SOFT_CTRL_LIN(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    sysCtrl = *SYSCTRLA_SFRS;
    
    IOCTRLA_SFRS->LIN.LINS_HWMODE       = 0U;   /* Hardware Mode Enabled. LIN slave peripheral writes/read the LIN I/O pin.*/
    IOCTRLA_SFRS->LIN.LINS_PU30K_ENA    = 1U;   /* LIN 30K pullup enable.*/
    IOCTRLA_SFRS->LIN.LINS_TXENA        = 1U;   /* LIN transmit enable.*/
    IOCTRLA_SFRS->LIN.LINS_RXENA        = 1U;   /* LIN receive enable. */
    
    while(1){
        GPIO_Set(GPIO_PORT_LINS, GPIO_LOW);     
        GPIO_Set(GPIO_PORT_LINS, GPIO_HIGH);    
    }
}

void  ADC_MEASURE_VBAT(void)
{
    int16_t result = 0;
    uint16_t code   = 0;

    SAR_CTRL_SFRS->SARANACFG.ADCVREFSEL = (uint8_t)ADC_REF_VBG_2TIMES;
    SAR_CTRL_SFRS->SARCFG.TRIGSEL       = (uint8_t)ADC_TRIGGER_SOURCE_SOFT_INPUT;
    SAR_CTRL_SFRS->SARCFG.TRIGDLY = 4U;        /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->SARCFG.SAMPCYC = 4U;         /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->AFECTRL.SARINPUTGAIN = ADC_AFE_GAIN_31_OF_32;
    SAR_CTRL_SFRS->AFECTRL.SARINPUTMODE = ADC_AFE_INPUT_INP_BUFFERED_INN_BUFFERED;
    
    SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = ADC_CH_SEL_VBAT_GND;
    SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
    
    SAR_CTRL_SFRS->SARCLKDIV = 16;
    SAR_CTRL_SFRS->AFECTRL.SARAFEEN     = 1U;
    SAR_CTRL_SFRS->SARANACFG.SARPREAMPEN = 1;
    SAR_CTRL_SFRS->AFECTRL.ADCSELVINVCMEXT = 0U;// set to 1 if only test PN voltage and enable low voltage mode when SAR_CTRL_SFRS->AFECTRL.SARAFEEN = 0;
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ = 1U;       /* SAR ADC Enable. Set to enable the SAR analog & digital part */

    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1;
    SAR_CTRL_SFRS->SARCTRL.CONVERT = 1;
    
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U);
    
    while(1){
        while(SAR_CTRL_SFRS->SARINT.STATUS.CONV_DONE == 0);
        convertDown = SAR_CTRL_SFRS->SARINT.STATUS.CONV_DONE;
        code = SAR_CTRL_SFRS->DATA[0].DATA;
        if ( (code & (1 << 11))!= 0){
            code = code | 0xF000;
        }
        result = code;
        buff[buffCount] = result;
        sum +=result;
        buffCount++;
        if(buffCount >= 16){
            /* VBAT-LED  */
            DEBUG_OUT("VBAT(V):%1.3f,CODE:%d\n",(float)sum*2.415*32/(31*2048),sum/16);
            buffCount = 0;
            sum = 0;
        }
        SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1;
        SAR_CTRL_SFRS->SARCTRL.CONVERT = 1;
    }
}


void  ADC_MEASURE_GPIO(void)
{
    GPIO_Init(GPIO_INIT_PORT_1,GPIO_DIR_INPUT,GPIO_PULL_NONE);
    uint16_t result = 0;
    SAR_CTRL_SFRS->SARANACFG.ADCVREFSEL = (uint8_t)ADC_REF_VBG_2TIMES;
    SAR_CTRL_SFRS->SARCFG.TRIGSEL       = (uint8_t)ADC_TRIGGER_SOURCE_SOFT_INPUT;
    SAR_CTRL_SFRS->SARCFG.TRIGDLY = 4U;          /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->SARCFG.SAMPCYC = 4U;          /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->AFECTRL.SARINPUTGAIN = ADC_AFE_GAIN_31_OF_32;
    SAR_CTRL_SFRS->AFECTRL.SARINPUTMODE = ADC_AFE_INPUT_INP_BUFFERED_INN_EXT;
    SAR_CTRL_SFRS->AFECTRL.SARAFEEN     = 1U;
    SAR_CTRL_SFRS->AFECTRL.ADCSELVINVCMEXT = 0U;// set to 1 if only test PN voltage and enable low voltage mode when SAR_CTRL_SFRS->AFECTRL.SARAFEEN = 0;
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ = 1U;       /* SAR ADC Enable. Set to enable the SAR analog & digital part */
    SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = ADC_CH_SEL_GPIO1_GND;
    SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
    SAR_CTRL_SFRS->SARANACFG.SARPREAMPEN = 1;
    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1;
    SAR_CTRL_SFRS->SARCTRL.CONVERT = 1;
    while(1){
        while(SAR_CTRL_SFRS->SARINT.STATUS.CONV_DONE == 0);
        convertDown = SAR_CTRL_SFRS->SARINT.STATUS.CONV_DONE;
        result = SAR_CTRL_SFRS->DATA[0].DATA;
        buff[buffCount] = result;
        sum +=result;
        buffCount++;
        if(buffCount >= 16){
            DEBUG_OUT("result:%d\n",sum/8);
            buffCount = 0;
            sum = 0;
        }
        SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1;
        SAR_CTRL_SFRS->SARCTRL.CONVERT = 1;
    }
}

void  ADC_MEASURE_VLED(void)
{
    int16_t result = 0;
    uint16_t code   = 0;
    IOCTRLA_REG_LEDPIN.SENSE_ENA = 1U;      /* LED Forward Voltage Sense Enable. Set to enable LED forward voltage sense module.*/
    IOCTRLA_REG_LEDPIN.GAIN_SEL= ADC_LED_GAIN_1_OF_2;
    IOCTRLA_REG_LEDPIN.VFW_ENA = 1;
    IOCTRLA_REG_LEDPIN.HWMODE = 0x00;
    IOCTRLA_REG_LEDPIN.DATA = 0;
    
    SAR_CTRL_SFRS->SARANACFG.ADCVREFSEL = (uint8_t)ADC_REF_VBG_2TIMES;
    SAR_CTRL_SFRS->SARCFG.TRIGSEL       = (uint8_t)ADC_TRIGGER_SOURCE_SOFT_INPUT;
    SAR_CTRL_SFRS->SARCFG.TRIGDLY = 15U;        /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->SARCFG.SAMPCYC = 15U;         /* 0.25us/count @4MHz*/
    SAR_CTRL_SFRS->AFECTRL.SARINPUTGAIN = ADC_AFE_GAIN_31_OF_32;
    SAR_CTRL_SFRS->AFECTRL.SARINPUTMODE = ADC_AFE_INPUT_INP_BUFFERED_INN_BUFFERED;
    
    SAR_CTRL_SFRS->ADCCHCONF.CH1SEL = ADC_CH_SEL_VBAT_LED0;
    SAR_CTRL_SFRS->ADCCHCONF.SEQCNT  = (uint8_t)ADC_SQ_CH1;
    
    SAR_CTRL_SFRS->SARCLKDIV = 16;
    SAR_CTRL_SFRS->AFECTRL.SARAFEEN     = 1U;
    SAR_CTRL_SFRS->SARANACFG.SARPREAMPEN = 1;
    SAR_CTRL_SFRS->AFECTRL.ADCSELVINVCMEXT = 0U;// set to 1 if only test PN voltage and enable low voltage mode when SAR_CTRL_SFRS->AFECTRL.SARAFEEN = 0;
    SAR_CTRL_SFRS->SARCTRL.SARENAREQ = 1U;       /* SAR ADC Enable. Set to enable the SAR analog & digital part */

    SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1;
    SAR_CTRL_SFRS->SARCTRL.CONVERT = 1;
    
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U);
    IOCTRLA_SFRS->LIN.UPDATE   = 1U; 
    while(IOCTRLA_SFRS->LIN.UPDATE == 1U);  
    
    while(1){
        while(SAR_CTRL_SFRS->SARINT.STATUS.CONV_DONE == 0);
        convertDown = SAR_CTRL_SFRS->SARINT.STATUS.CONV_DONE;
        code = SAR_CTRL_SFRS->DATA[0].DATA;
        if ( (code & (1 << 11))!= 0){
            code = code | 0xF000;
        }
        result = code;
//        DEBUG_OUT("Code:%d\n",code);
        buff[buffCount] = result;
        sum +=result;
        buffCount++;
        if(buffCount >= 16){
          /* VBAT-GND  */
//          DEBUG_OUT("result:%1.3f,Code:%d\n",(float)sum*2.415*32/(31*2048),sum/16);
          /* LED-GND  */
//          DEBUG_OUT("result:%1.3f,Code:%d\n",(float)sum*2.415*32/(31*2048*16),sum/16);
          /* VBAT-LED  */
          DEBUG_OUT("result:%1.3f,Code:%d\n",(float)sum*2.415*2*32/(2048*16*31),sum/16);
            buffCount = 0;
            sum = 0;
        }
        SAR_CTRL_SFRS->SARINT.CLEAR.INT_CONV_DONE_CLR = 1;
        SAR_CTRL_SFRS->SARCTRL.CONVERT = 1;
        
    }
}


void delay(uint32_t delay)
{
  for (uint32_t i = 0; i < delay; i++){
      for (uint32_t j = 0; j < 10000; j++){
        ;
      }
  }
}

uint8_t tx,rx;
void LIN_PL_Test(void)
{
  
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    IOCTRLA_REG_FILT_ACCESS.FILT_UNLOCK = 1U;
    LINS_REG_CTRL.SLEEP = 0U;                     /*  wake up lins when init if sleep  */
    
    IOCTRLA_SFRS->LINTXDMONITOR.LINSTXDMONITORENA        = 1U;          /* LINS TxD Monitor enable   */
    IOCTRLA_SFRS->LIN.LINS_PUOFF_TIMEOUT                 = 1U;          

    IOCTRLA_SFRS->LINSGFCONF.LINSRXGF1ST_ENA = 1U;
    IOCTRLA_SFRS->LINSGFCONF.LINSRXGF2ND_ENA = 1U;
    IOCTRLA_SFRS->LINSGFCONF.LINSRXGF3RD_ENA = 1U;

    IOCTRLA_SFRS->LINSGFCONF1.LINS1ST_DBNCTHRES0  = 0x01;        /* glitch filter settings */
    IOCTRLA_SFRS->LINSGFCONF1.LINS1ST_DBNCTHRES1  = 0x01;        /* value*62.5us   */
    
    IOCTRLA_SFRS->LINSGFCONF1.LINS2ND_DBNCTHRES0  = 0x2;        /* glitch filter settings */
    IOCTRLA_SFRS->LINSGFCONF1.LINS2ND_DBNCTHRES1  = 0x2;        /* value*62.5us   */
    
    IOCTRLA_REG_LINSGFCONF.LINS3RD_DBNCTHRES0     = 0x3;           /* glitch filter settings */
    IOCTRLA_REG_LINSGFCONF.LINS3RD_DBNCTHRES1     = 0x3;           /* value*62.5us   */
    
    SYSCTRLA_SFRS->LIN.LINS_TX_BIAS_BOOST       = LIN_TX_DRIVE_CURRENT_118_mA;
    SYSCTRLA_SFRS->LIN.LINS_TX_SLOPE            = LIN_TX_SLEW_RATE_5_1V_PER_US;
    SYSCTRLA_SFRS->LIN.LINS_TX_DUTY             = 5;
    SYSCTRLA_SFRS->LIN.LINS_RX_BIAS_BOOST       = 3;
    
    IOCTRLA_REG_LIN.LINS_PU30K_ENA              = 1U;     /* LIN 30K pullup enable.*/
    IOCTRLA_REG_LIN.LINS_TXENA                  = 1U;     /* LIN transmit enable.*/
    IOCTRLA_REG_LIN.LINS_RXENA                  = 1U;     /* LIN receive enable. */
    PMUA_REG_CTRL.UPDATE                        = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U);
    
    tx = IOCTRLA_REG_LIN.LINS_TXENA;
    rx = IOCTRLA_REG_LIN.LINS_RXENA;
  
    IOCTRLA_SFRS->LIN.LINS_HWMODE       = 0U;
    IOCTRLA_REG_GPIO1.LINS_SEL          = 0U;
    
    IOCTRLA_REG_GPIO1.HWMODE            = 2U;     /* LINS RXD signal */
    IOCTRLA_REG_GPIO1.MUXSEL            = 5U;
    IOCTRLA_REG_GPIO1.RDENA             = 1U;
    
    IOCTRLA_REG_GPIO2.HWMODE            = 2U;     /* LINS TXD signal */
    IOCTRLA_REG_GPIO2.RDENA             = 1U;
    IOCTRLA_REG_GPIO2.PULLMODE          = GPIO_PULL_UP;
    
    IOCTRLA_SFRS->LIN.UPDATE   = 1U; 
    while(IOCTRLA_SFRS->LIN.UPDATE == 1U);  
    
    while(1);
}


void LIN_GlitchFilter_Test(void)
{
  
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    IOCTRLA_REG_FILT_ACCESS.FILT_UNLOCK            = 1U;
    
    LINS_REG_CTRL.SLEEP = 0U;                     /*  wake up lins when init if sleep  */
    /* Config GPIO to LIN mode, enable transmission */
    IOCTRLA_SFRS->LIN.LINS_HWMODE       = 1U;     /* Hardware Mode Enabled. LIN slave peripheral writes/read the LIN I/O pin.*/
    IOCTRLA_SFRS->LIN.LINS_PU30K_ENA    = 1U;     /* LIN 30K pullup enable.*/
    IOCTRLA_SFRS->LIN.LINS_TXENA        = 1U;     /* LIN transmit enable.*/
    IOCTRLA_SFRS->LIN.LINS_RXENA        = 1U;     /* LIN receive enable. */
    
    IOCTRLA_SFRS->LINSGFCONF.LINSRXGF1ST_ENA = 1U;
    IOCTRLA_SFRS->LINSGFCONF.LINSRXGF2ND_ENA = 1U;
    IOCTRLA_SFRS->LINSGFCONF.LINSRXGF3RD_ENA = 3U;
    IOCTRLA_SFRS->LINTXDMONITOR.LINSTXDMONITORENA = 1U;         /* LINS TxD Monitor enable   */
  
    IOCTRLA_SFRS->LINSGFCONF1.LINS1ST_DBNCTHRES0 = 0x08;        /* glitch filter settings */
    IOCTRLA_SFRS->LINSGFCONF1.LINS1ST_DBNCTHRES1 = 0x00;        /* value*62.5us   */
    
    IOCTRLA_SFRS->LINSGFCONF1.LINS2ND_DBNCTHRES0 = 0x10;        /* glitch filter settings */
    IOCTRLA_SFRS->LINSGFCONF1.LINS2ND_DBNCTHRES1 = 0x08;        /* value*62.5us   */
    
    IOCTRLA_REG_LINSGFCONF.LINS3RD_DBNCTHRES0 = 0x30;           /* glitch filter settings */
    IOCTRLA_REG_LINSGFCONF.LINS3RD_DBNCTHRES1 = 0x30;           /* value*62.5us   */
    
    
}


void WDTA_WindowMode_Test(void)
{
//    CRGA_SFRS->WDTACTION.WDTACTION = 0;
//    WDTA_SFRS->INTERRUPT.TIMEOUT_EN = 0;//    
//    WDTA_SFRS->INTERRUPT.WINDOW_OPEN_EN = 1;//
//    WDTA_SFRS->CTRL.TIMEOUTSEL = 7;
//    WDTA_SFRS->CTRL.WINOPENENA = 1;     //Window Mode Enable  
//    WDTA_SFRS->CTRL.UPDATE = 1;         //Window Mode take effect 
//    while(WDTA_SFRS->CTRL.UPDATE == 1);
//    WDTA_SFRS->STOP.STOP = 0U;/* start wdt*/
//    CRGA_SFRS->WDTACTION.WDTBARKCNT = 0;
//    while (1){
//        if (WDTA_SFRS->CTRL.WINOPENFLAG == 1){
//            WDTA_SFRS->CLEAR = 0x3c574ad6U;
//        }
//    }
}

void UV_ISR(void)
{
    DEBUG_OUT("Battery LOW ISR!!\n");
}

void OV_ISR(void)
{
    DEBUG_OUT("Battery HIGH ISR!!\n");
}

void UV_Test(void)
{
    uint8_t ov = 0U,uv = 0;
    HW_PROT_SetUnderVoltage(UV_VOLT_5_135V,UV_HYS_06_PERCENT, VOLT_DEBOUNCE_TIME_10ms, VOLT_DEBOUNCE_TIME_10ms);// 10ms
    HW_PROT_RegisterUnderVoltageIRQ(UV_ISR);
    
    HW_PROT_SetOverVoltage(OV_VOLT_15_6V,OV_HYS_09_PERCENT, VOLT_DEBOUNCE_TIME_10ms, VOLT_DEBOUNCE_TIME_10ms);
    HW_PROT_RegisterOverVoltageIRQ(OV_ISR);
    
    GPIO_Init(GPIO_INIT_PORT_1,GPIO_DIR_OUTPUT,GPIO_PULL_NONE);
    
    GPIO_Init(GPIO_INIT_PORT_2,GPIO_DIR_OUTPUT,GPIO_PULL_NONE);
    while(1){
        if (HW_PROT_GetBattVoltState() == 1){
            GPIO_Set(GPIO_PORT_1, GPIO_HIGH);
            if (uv == 0){
                uv = 1;
                DEBUG_OUT("Battery LOW triggerred!!\n");
            }
        }else{
            GPIO_Set(GPIO_PORT_1, GPIO_LOW);
            if (uv == 1){
                uv = 0;
                PMUA_SFRS->PMUIRQ.CLEAR.UV  = 1U;
                DEBUG_OUT("Battery LOW released! UV %d:\n",PMUA_SFRS->PMUIRQ.IRQ.UV);
                NVIC_ClearPendingIRQ(UV_IRQn);
                NVIC_EnableIRQ(UV_IRQn);
            }
        }
        
        if (HW_PROT_GetBattVoltState() == 2){
            GPIO_Set(GPIO_PORT_2, GPIO_HIGH);
            if (ov == 0){
                ov = 1;
                DEBUG_OUT("Battery HIGH triggerred!!\n");
            }
        }else{
            GPIO_Set(GPIO_PORT_2, GPIO_LOW);
            if (ov == 1){
                ov = 0;
                DEBUG_OUT("Battery HIGH released!!\n");
                PMUA_SFRS->PMUIRQ.CLEAR.OV  = 1U;
                NVIC_ClearPendingIRQ(OV_IRQn);
                NVIC_EnableIRQ(OV_IRQn);
            }
        }
    }
}


#define PWM_AUX_PEROID (16385)
void GPIO_PWM_AUX_Test(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    PWMAUX_Init(PWMAUX_PORT1_CH0,PWMAUX_BASE_0,PWMAUX_CLK_DIV_1,PWM_AUX_PEROID);
    PWMAUX_SetMatchValue(PWMAUX_PORT1_CH0, 0, PWM_AUX_PEROID/3);
    
    PWMAUX_Init(PWMAUX_PORT2_CH1,PWMAUX_BASE_0,PWMAUX_CLK_DIV_1,PWM_AUX_PEROID);
    PWMAUX_SetMatchValue(PWMAUX_PORT2_CH1, 0, PWM_AUX_PEROID/2);
    
    while(1);
}

void validation_main(void)
{
    SYSCTRLA_REG_DEBUG_ACCESS_KEY.DEBUG_ACCESS_KEY = 0x05U;
    SYSCTRLA_REG_TRIM_ACCESS_KEY.TRIM_ACCESS_KEY   = 0x0EU;
    Clock_SystemMainClockInit(SYS_MAIN_CLOCK_DIV);
//  Major_BandgapOUT();
//  SET_VDD_PRE5V_OUT();
//  ADC_MEASURE_GPIO();
  LIN_PL_Test();
//  Major_BandgapOUT();
//  ADC_REFP_OUT();
//  LED_Current();
//  GPIO_PWM_Test();
//  UV_Test();
//  ADC_MEASURE_VLED();
//  LF_32KHz_OUT();
//  HF_16MHz_OUT();
//    GPIO_PWM_AUX_Test();
    while(1);
}


#endif
