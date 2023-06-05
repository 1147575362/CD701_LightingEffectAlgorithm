/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file ioctrla_sfr.h
 */

#ifndef IOCTRLA_SFR_H__
#define IOCTRLA_SFR_H__

#include <stdint.h>

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/**
 * @brief A structure to represent Special Function Registers for IOCTRLA.
 */

typedef union {
  struct {
    uint8_t  HWMODE                   :  3; /*!< hardware mode */
    uint8_t  LINS_SEL                 :  1; /*!<  */
    uint8_t  PULLMODE                 :  2; /*!< 01: pull none, 00: pull up, 11: pull down, 10: pull up and down */
    uint8_t  RDENA                    :  1; /*!< read enable */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t  GPIO2_MUXSEL             :  8;
    uint8_t  MUXSEL                   :  8;
  };
  uint32_t WORD;
}IOCTRLA_GPIO1_t; /* +0x000 */

typedef union {
  struct {
    uint8_t  HWMODE                   :  3; /*!< hardware mode */
    uint8_t                           :  1; /*!<  */
    uint8_t  PULLMODE                 :  2; /*!< 01: pull none, 00: pull up, 11: pull down, 10: pull up and down */
    uint8_t  RDENA                    :  1; /*!< read enable */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8;
    uint8_t                           :  8;
  };
  uint32_t WORD;
}IOCTRLA_GPIO2_t; /* +0x000 */

typedef union {
  struct {
    uint8_t  HWMODE                   :  3; /*!< hardware mode */
    uint8_t  LINS_SEL                 :  1; /*!<  */
    uint8_t  PULLMODE                 :  2; /*!< 01: pull none, 00: pull up, 11: pull down, 10: pull up and down */
    uint8_t  RDENA                    :  1; /*!< read enable */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8;
    uint8_t                           :  8;
  };
  uint32_t WORD;
}IOCTRLA_GPIO3_t; /* +0x000 */

typedef union {
  struct {
    uint8_t  HWMODE                   :  3; /*!< hardware mode */
    uint8_t                           :  1; /*!<  */
    uint8_t  PULLMODE                 :  2; /*!< 01: pull none, 00: pull up, 11: pull down, 10: pull up and down */
    uint8_t  RDENA                    :  1; /*!< read enable */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8;
    uint8_t                           :  8;
  };
  uint32_t WORD;
}IOCTRLA_GPIO4_t; /* +0x000 */


typedef union {
  struct {
    uint8_t  HWMODE                   :  3; /*!< hardware mode */
    uint8_t                           :  1; /*!<  */
    uint8_t  PULLMODE                 :  2; /*!< 01: pull none, 00: pull up, 11: pull down, 10: pull up and down */
    uint8_t  RDENA                    :  1; /*!< read enable */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8;
    uint8_t                           :  8;
  };
  uint32_t WORD;
}IOCTRLA_GPIO_t; /* +0x000 */


typedef union {
  struct {
    uint8_t  LINS_HWMODE              :  1; /*!< LIN Slave hardware mode */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  LINS_PU30K_ENA           :  1; /*!< LIN 30K pullup enable */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  LINS_TXENA               :  1; /*!< LIN transmit enable */
    uint8_t  LINS_RXENA               :  1; /*!< LIN receive  enable */
    uint8_t  LINS_RXD_HIGH_RST_ENA    :  1; /*!< Enable signal that LINs RXD glitch filter at sleep mode is asynchronous reset by high of LINs_RXD ,NOTE: The write operation of this register takes effect by configuring SYSCTRLA_SFRS->TRIM_ACCESS_KEY.TRIM_ACCESS_KEY, FILT_ACCESS.FILT_UNLOCK and IOCTRLA_SFRS->LIN.UPDATE*/
    uint8_t  LINS_SLEEP_GF_THRES0     :  4; /*!< NOTE: The write operation of this register takes effect by configuring SYSCTRLA_SFRS->TRIM_ACCESS_KEY.TRIM_ACCESS_KEY, FILT_ACCESS.FILT_UNLOCK and IOCTRLA_SFRS->LIN.UPDATE   */
    uint8_t  LINS_SLEEP_GF_THRES1     :  4; /*!< NOTE: The write operation of this register takes effect by configuring SYSCTRLA_SFRS->TRIM_ACCESS_KEY.TRIM_ACCESS_KEY, FILT_ACCESS.FILT_UNLOCK and IOCTRLA_SFRS->LIN.UPDATE   */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  LINS_PUOFF_TIMEOUT       :  1; /*!< LINS Pullup Disable in dominant TimeOut condition,NOTE: The write operation of this register takes effect by PMUA_SFRS->CTRL.UPDATE */
    uint8_t                           :  5; /*   (reserved) */
    uint8_t  PMODE                    :  1; /*!< LIN Power Mode */
    uint8_t                           :  6; /*   (reserved) */
    uint8_t  UPDATE                   :  1; /*!< LIN Configure Update */
  };
  uint32_t WORD;
}IOCTRLA_LIN_t; /* +0x000 */


typedef union {
  struct {
      uint8_t  LINSRXGF3RD_ENA    :  2; /*!< LINS RXD 3rd Glitch Filter enables*/
      uint8_t  LINSRXGF1ST_ENA    :  1; /*!< LINS RXD 1st Glitch Filter enables */
      uint8_t  LINSRXGF2ND_ENA    :  1; /*!< LINS RXD 2nd Glitch Filter enables */
      uint8_t                     :  4; /*   (reserved) */
      uint8_t  LINS3RD_DBNCTHRES0 :  8; /*!< 3rd Stage LINS Debounce Threshold for 1 to 0 */
      uint8_t  LINS3RD_DBNCTHRES1 :  8; /*!< 3rd Stage LINS Debounce Threshold for 0 to 1 */
      uint8_t                     :  8; /*   (reserved) */
  };
  uint32_t WORD;
}IOCTRLA_LINSGFCONF_t; /* +0x00C */



typedef union {
  struct {
    uint8_t  LINSTXDMONITORENA        :  8; /*!< LINS TxD Monitor enable */
    uint8_t  LINSTXDTIMEOUTDOMINANT   :  8; /*!< Tx Dominant Timeout */
    uint16_t                          : 16; /*   (reserved) */
  };
  uint32_t WORD;
}IOCTRLA_LINTXDMONITOR_t; /* +0x010 */


typedef union {
  struct {
    uint8_t  HWMODE                   :  3; /*!< LED hardware mode */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  DATA                     :  3; /*!< LED Data Out */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  SENSE_ENA                :  1; /*!< LED Forward Voltage Sense Enable */
    uint8_t  SENSE_CTRL               :  3; /*!< LED */
    uint8_t                           :  3; /*   (reserved) */
    uint8_t  VFW_ENA                  :  1; /*!< LED Forward Voltage Current Enable */
    uint8_t  GAIN_SEL                 :  1; /*!< LED Sense AFE gain select */
    uint8_t  PN_OP_START_BIAS_BOOST   :  2; /*!< PN OP START BIAS BOOST */
    uint8_t  OP_CHOOSE_R              :  3; /*!< LED SENSE OP CHOOSE R */
    uint8_t  OP_GBW_SEL               :  2; /*!< LED SENSE OP GBW SEL */
    uint8_t  LED_PULLUP_EN            :  8; /*!< LED PULLUP ENABLE */
  };
  uint32_t WORD;
}IOCTRLA_LEDPIN_t;



typedef union {
  struct {
    uint8_t  GPIOCONSEL               :  1; /*!< Hardware/Firmware Select */
    uint8_t  ADCCONSEL                :  1; /*!< ADC CON SEL */
    uint8_t  ADCSELSEL                :  1; /*!< Hardware/Firmware Select */
    uint8_t                           :  5; /*   (reserved) */
    uint32_t GPIOCONREG               :  9; /*!< Firmware Debug Value !!misaligned!! */
    uint8_t                           :  3; /*   (reserved) */
    uint8_t  ADCCONREG                :  3; /*!< Firmware Debug Value */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  ADCSELREG                :  4; /*!< Firmware Debug Value */
    uint8_t  LEDSEL                   :  3; /*!< Firmware Debug Value */
    uint8_t                           :  1; /*   (reserved) */
  };
  uint32_t WORD;
} IOCTRLA_ANALOGTESTMUXOVERRIDE_t; /* +0x018 */

typedef struct {
  struct {
      uint8_t  LINS_TXD_DOM         :  8; /*!< LINS TXD Dominant Monitor interrupt enable */
  }ENABLE;
  struct {
      uint8_t  LINS_TXD_DOM         :  8; /*!< LINS TXD Dominant Monitor interrupt clear */
  }CLEAR;
  struct {
      uint8_t  LINS_TXD_DOM         :  8; /*!< LINS TXD Dominant Monitor interrupt status */
  }STATUS;
  struct {
      uint8_t  LINS_TXD_DOM         :  8; /*!< LINS TXD Dominant Monitor interrupt active */
  }IRQ;
} IOCTRLA_LINDMIRQ_t; /* +0x01C */


typedef union {
  struct {
    uint8_t  LINS1ST_DBNCTHRES0     :  8; /*!< 1st Stage LINS Debounce Threshold for 1 to 0 */
    uint8_t  LINS1ST_DBNCTHRES1     :  8; /*!< 1st Stage LINS Debounce Threshold for 0 to 1 */
    uint8_t  LINS2ND_DBNCTHRES0     :  8; /*!< 2nd Stage LINS Debounce Threshold for 1 to 0 */
    uint8_t  LINS2ND_DBNCTHRES1     :  8; /*!< 2nd Stage LINS Debounce Threshold for 0 to 1 */
  };
  uint32_t WORD;
} IOCTRLA_LINSGFCONF1_t; /* +0x020 */


typedef union {
  struct {
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  FILT_UNLOCK              :  1;
  };
  uint32_t WORD;
} IOCTRLA_FILT_ACCESS_t; /* +0x024 */

typedef union {
  struct {
    uint8_t  CAPENA                   :  4; /*!< IO capture enable */
    uint8_t  IRQENA                   :  4; /*!< IO capture irq enable */
    uint8_t  IRQCLR                   :  4; /*!< IO capture irq clear */
    uint8_t  IRQSTS                   :  4; /*!< IO capture irq status */
    uint8_t  PRESCALE_EN              :  1; /*!< prescale enable */
    uint8_t  PRESCALE                 :  2; /*!< prescaler select */
    uint8_t                           :  5; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
  };
  uint32_t WORD;
}IOCTRLA_IOCAPTURE_t; /* +0x028 */



typedef struct {
    uint16_t COUNTER_HIGH       : 16; /*!< GPIO0 input high level counter */
    uint16_t COUNTER_LOW        : 16; /*!< GPIO0 input low level counter */
}IOCTRLA_COUNTERGPIO_t; /* +0x03C */


typedef union {
  struct {
    uint8_t  IO_DBNC_THRES0           :  8; /*!< IO Debounce Threshold for 1 to 0 */
    uint8_t  IO_DBNC_THRES1           :  8; /*!< IO Debounce Threshold for 0 to 1 */
    uint16_t                          : 16; /*   (reserved) */
  };
  uint32_t WORD;
}IOCTRLA_IOGFCONF_t; /* +0x04C */

typedef struct {
  IOCTRLA_GPIO_t                        GPIO[4];
  IOCTRLA_LIN_t                         LIN;
  IOCTRLA_LINSGFCONF_t                  LINSGFCONF;
  IOCTRLA_LINTXDMONITOR_t               LINTXDMONITOR;
  IOCTRLA_LEDPIN_t                      LEDPIN;
  IOCTRLA_ANALOGTESTMUXOVERRIDE_t       ANALOGTESTMUXOVERRIDE;
  IOCTRLA_LINDMIRQ_t                    LINDMIRQ;
  IOCTRLA_LINSGFCONF1_t                 LINSGFCONF1;
  IOCTRLA_FILT_ACCESS_t                 FILT_ACCESS;
  IOCTRLA_IOCAPTURE_t                   IOCAPTURE;
  uint16_t                              IOCAPTUREPERIOD_GPIO[4];
  IOCTRLA_COUNTERGPIO_t                 IOCAPTURE_COUNTER_GPIO[4];
  IOCTRLA_IOGFCONF_t                    IOGFCONF;
} IOCTRLA_SFRS_t;

/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/**
 * @brief The starting address of IOCTRLA SFRS.
 */
#define IOCTRLA_REG_GPIO1                       (*(__IO                 IOCTRLA_GPIO1_t *)(0x50011000))
#define IOCTRLA_REG_GPIO2                       (*(__IO                 IOCTRLA_GPIO2_t *)(0x50011004))
#define IOCTRLA_REG_GPIO3                       (*(__IO                 IOCTRLA_GPIO3_t *)(0x50011008))
#define IOCTRLA_REG_GPIO4                       (*(__IO                 IOCTRLA_GPIO4_t *)(0x5001100C))
#define IOCTRLA_REG_GPIO(index)                 (*(__IO                  IOCTRLA_GPIO_t *)(0x50011000+ index*4)) /* index = 0-3 */

#define IOCTRLA_REG_LIN                         (*(__IO                   IOCTRLA_LIN_t *)(0x50011010))
#define IOCTRLA_REG_LINSGFCONF                  (*(__IO            IOCTRLA_LINSGFCONF_t *)(0x50011014))
#define IOCTRLA_REG_LINTXDMONITOR               (*(__IO         IOCTRLA_LINTXDMONITOR_t *)(0x50011018))
#define IOCTRLA_REG_LEDPIN                      (*(__IO                IOCTRLA_LEDPIN_t *)(0x5001101C)) 
#define IOCTRLA_REG_ANALOGTESTMUXOVERRIDE       (*(__IO IOCTRLA_ANALOGTESTMUXOVERRIDE_t *)(0x50011020))
#define IOCTRLA_REG_LINDMIRQ                    (*(__IO              IOCTRLA_LINDMIRQ_t *)(0x50011024))
#define IOCTRLA_REG_LINSGFCONF1                 (*(__IO           IOCTRLA_LINSGFCONF1_t *)(0x50011028))
#define IOCTRLA_REG_FILT_ACCESS                 (*(__IO           IOCTRLA_FILT_ACCESS_t *)(0x5001102C))
#define IOCTRLA_REG_IOCAPTURE                   (*(__IO             IOCTRLA_IOCAPTURE_t *)(0x50011030))
#define IOCTRLA_REG_IOCAPTURE_PERIOD(index)     (*(__IO                        uint16_t *)(0x50011034 + index* 2))
#define IOCTRLA_REG_COUNTER(index)              (*(__IO                        uint32_t *)(0x5001103C + index *4))
#define IOCTRLA_REG_IOGFCONF                    (*(__IO                        uint32_t *)(0x5001104C))

#define IOCTRLA_SFRS ((__IO IOCTRLA_SFRS_t *)0x50011000)

#endif /* end of __IOCTRLA_SFR_H__ section */


