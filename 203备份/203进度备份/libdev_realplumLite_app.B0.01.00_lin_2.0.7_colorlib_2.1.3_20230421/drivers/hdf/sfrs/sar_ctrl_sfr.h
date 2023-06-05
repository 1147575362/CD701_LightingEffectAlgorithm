/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file sar_ctrl_sfr.h
 */

#ifndef SAR_CTRL_SFR_H__
#define SAR_CTRL_SFR_H__

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
 * @brief A structure to represent Special Function Registers for SAR_CTRL.
 */
typedef struct {
  uint16_t DATA;                             /* +0x000 */
  uint8_t  _RESERVED_02[2];                   /* +0x002 */
}SAR_DATA_t;


typedef union {
  struct {
    uint8_t  ADCVREFSEL               :  2; /*!< adc vref select */
    uint8_t                           :  5; /*   (reserved) */
    uint8_t  SARPREAMPEN              :  1; /*!< adc pre-amp enable */
    uint32_t                          : 24; /*   (reserved) */
  };
  uint32_t WORD;
}SAR_SARANACFG_t; /* +0x010 */


typedef union {
  struct {
    uint8_t  SAMPCYC                  :  4; /*!< Sample cycle */
    uint8_t                           :  3; /*   (reserved) */
    uint8_t  ROUND                    :  1; /*!< ADC round enable */
    uint8_t  TRIGSEL                  :  4; /*!< SAR Converion Trigger Selects */
    uint8_t  PWMSEL                   :  2; /*!< PWM Trigger Signal Selects */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  TRIGDLY                  :  8; /*!< trigger delay */
    uint8_t                           :  8; /*   (reserved) */
  };
  uint32_t WORD;
}SAR_SARCFG_t; /* +0x014 */

typedef union {
  struct {
    uint8_t  ADCSELVINVCMEXT          :  1; /*!< Select External Inputs to ADC */
    uint8_t                           :  6; /*   (reserved) */
    uint8_t  SARAFEEN                 :  1; /*!< ADC AFE Enable */
    uint8_t  SARINPUTMODE             :  2; /*!< ADC AFE Input Modes */
    uint8_t                           :  4; /*   (reserved) */
    uint8_t  SARINPUTGAIN             :  2; /*!< choose ADC input gain */
    uint16_t                          : 16; /*   (reserved) */
  };
  uint32_t WORD;
}SAR_AFECTRL_t; /* +0x018 */


typedef union {
  struct {
    uint8_t  SARENAREQ                :  8; /*!< SAR ADC Enable */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  DIGRESET                 :  1; /*!< SAR Digital Part Reset */
    uint8_t                           :  5; /*   (reserved) */
    uint8_t  CONVERT                  :  1; /*!< ADC START/STATUS Register */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  CONT                     :  1; /*!< Continuous Conversion Enable */
    uint8_t                           :  5; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
  };
  uint32_t WORD;
}SAR_SARCTRL_t; /* +0x01C */


typedef union {
  struct {
    uint8_t  SEQCNT                   :  3; /*!< Channel Sequence count */
    uint8_t  CH1SEL                   :  5; /*!< Channel1 Selection */
    uint8_t  CH2SEL                   :  8; /*!< Channel2 Selection */
    uint8_t  CH3SEL                   :  8; /*!< Channel3 Selection */
    uint8_t  CH4SEL                   :  5; /*!< Channel4 Selection */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  TESTBATTERYGAINCHOOSE    :  1; /*!< VBAT Gain Select */
  };
  uint32_t WORD;
}SAR_ADCCHCONF_t; /* +0x020 */


typedef union {
  struct {
    union {
      struct {
        uint8_t  INT_CONV_DONE_ENA    :  8; /*!< Convert Done Interrupt Enable */
      };
      uint8_t BYTE;
    } ENABLE;
    union {
      struct {
        uint8_t  INT_CONV_DONE_CLR    :  8; /*!< Convert Done Interrupt Clear */
      };
      uint8_t BYTE;
    } CLEAR;
    union {
      struct {
        uint8_t  CONV_DONE            :  8; /*!< Convert Done */
      };
      uint8_t BYTE;
    } STATUS;
    union {
      struct {
        uint8_t  INT_CONV_DONE        :  8; /*!< Convert Done Interrupt */
      };
      uint8_t BYTE;
    } IRQ;
  };
  uint32_t WORD;
}SAR_SARINT_t; /* +0x024 */

typedef struct {

  SAR_DATA_t            DATA[4];
  SAR_SARANACFG_t       SARANACFG; /* +0x010 */
  SAR_SARCFG_t          SARCFG; /* +0x014 */
  SAR_AFECTRL_t         AFECTRL; /* +0x018 */
  SAR_SARCTRL_t         SARCTRL; /* +0x01C */
  SAR_ADCCHCONF_t       ADCCHCONF; /* +0x020 */
  SAR_SARINT_t          SARINT; /* +0x024 */
  uint8_t               SARCLKDIV;                         /*<! SAR CLOCK DIVIDE +0x028 */
  uint8_t               _RESERVED_29[3];                   /* +0x029 */

} SAR_CTRL_SFRS_t;

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
 * @brief The starting address of SAR_CTRL SFRS.
 */
#define SAR_REG_DATA(x)            (*(__IO        SAR_DATA_t *)(0x50010D00 +x*4))
#define SAR_REG_SARANACFG          (*(__IO   SAR_SARANACFG_t *)(0x50010D10))
#define SAR_REG_SARCFG             (*(__IO      SAR_SARCFG_t *)(0x50010D14))
#define SAR_REG_AFECTRL            (*(__IO     SAR_AFECTRL_t *)(0x50010D18))
#define SAR_REG_SARCTRL            (*(__IO     SAR_SARCTRL_t *)(0x50010D1C))
#define SAR_REG_ADCCHCONF          (*(__IO   SAR_ADCCHCONF_t *)(0x50010D20))
#define SAR_REG_SARINT             (*(__IO      SAR_SARINT_t *)(0x50010D24))
#define SAR_REG_SARCLKDIV          (*(__IO           uint8_t *)(0x50010D28))

#define SAR_CTRL_SFRS ((__IO SAR_CTRL_SFRS_t *)0x50010D00)

#endif /* end of __SAR_CTRL_SFR_H__ section */


