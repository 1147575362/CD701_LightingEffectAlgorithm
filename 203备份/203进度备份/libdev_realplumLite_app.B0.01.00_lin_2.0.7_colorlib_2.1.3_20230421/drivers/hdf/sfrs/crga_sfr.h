/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file crga_sfr.h
 */

#ifndef CRGA_SFR_H_
#define CRGA_SFR_H_

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

#define MODUE_SOFT_RESET_ADC            (0x01U)
#define MODUE_SOFT_RESET_LINS           (0x02U)
#define MODUE_SOFT_RESET_PWM            (0x04U)
#define MODUE_SOFT_RESET_PWM_AUX        (0x08U)

/**
 * @brief A structure to represent Special Function Registers for CRGA.
 */
typedef union {
  struct {
    uint8_t  LFRCSTS                  :  8; /*!< Slow oscillator status */
    uint8_t  CLKLFSEL                 :  8; /*!< LF Clock Source select */
    uint16_t                          : 16; /*   (reserved) */
  };
  uint32_t WORD;
} CRGA_LFCLKCTRL_t; /* +0x000 */

typedef union {
  struct {
    uint8_t  HFRCENA                  :  1; /*!< Fast oscillator enable */
    uint8_t  HFRCSTS                  :  1; /*!< Fast oscillator status */
    uint8_t                           :  6; /*   (reserved) */
    uint8_t  SYSCLKSEL                :  8; /*!< Clock select */
    uint8_t  DIVSYSCLK                :  3; /*!< Clock div select */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  HF_RC_LDO_VSEL           :  2;
    uint8_t                           :  2; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
  };
  uint32_t WORD;
} CRGA_SYSCLKCTRL_t; /* +0x004 */


typedef union {
  struct {
    uint8_t  PORFLAG                  :  1; /*!< Power on reset flag */
    uint8_t  OVFLAG                   :  1; /*!< Over Voltage Monitor flag */
    uint8_t  BOR3V3FLAG               :  1; /*!< BOR 3v3 flag */
    uint8_t  UVFLAG                   :  1; /*!< Under Voltage Monitor flag */
    uint8_t  BOR1V5FLAG               :  1; /*!< BOR 1v5 flag */
    uint8_t  WDTFLAG                  :  1; /*!< Watchdog bark flag */
    uint8_t  OVTEMPFLAG               :  1; /*!< Over Temp Violation flag */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  PORFLAGCLR               :  1; /*!< POR flag clear */
    uint8_t  OVFLAGCLR                :  1; /*!< OV flag clear */
    uint8_t  BOR3V3FLAGCLR            :  1; /*!< BOR 3v3 clear */
    uint8_t  UVFLAGCLR                :  1; /*!< UV flag clear */
    uint8_t  BOR1V5FLAGCLR            :  1; /*!< BOR 1v5 clear */
    uint8_t  WDTFLAGCLR               :  1; /*!< WDT flag clear */
    uint8_t  OVTEMPFLAGCLR            :  1; /*!< OVTEMP flag clear */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  HARDRSTREQ               :  1; /*!< Hard reset request */
    uint8_t  SAFEMODEREQ              :  1; /*!< Safe Mode request */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  SAFEMODEENA              :  1; /*!< Safe Mode enable */
    uint8_t                           :  3; /*   (reserved) */
    uint8_t  SOFTRSTREQ               :  8; /*!< Soft reset request */
  };
  uint32_t WORD;
} CRGA_RESETCTRL_t; /* +0x008 */


typedef union {
  struct {
    uint8_t  VDD3V3                   :  1; /*!< BOR 3v3 action */
    uint8_t                           :  3; /*   (reserved) */
    uint8_t  VDD1V5                   :  1; /*!< BOR 1v5 action */
    uint8_t                           :  3; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  BOR_3V3_LOCK             :  1;
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  BOR_1V5_LOCK             :  1;
  };
  uint32_t WORD;
} CRGA_BORACTION_t; /* +0x010 */


typedef union {
  struct {
    uint8_t  S_BOR_3P3V               :  8; /*!< BOR 3v3 threshold,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t  S_BOR_1P5V               :  8; /*!< BOR 1v5 threshold,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE */
    uint8_t                           :  8; /*   (reserved) */
  };
  uint32_t WORD;
} CRGA_BORCONFIG_t; /* +0x014 */

typedef union {
  struct {
    uint8_t  ENA_BOR1V5_DEGLITCH      :  1;
    uint8_t                           :  3; /*   (reserved) */
    uint8_t  SEL_BOR1V5_NEG_DEGLITCH  :  2;
    uint8_t  SEL_BOR1V5_POS_DEGLITCH  :  2;
    uint8_t  ENA_BOR3V3_DEGLITCH      :  1;
    uint8_t                           :  3; /*   (reserved) */
    uint8_t  SEL_BOR3V3_NEG_DEGLITCH  :  2;
    uint8_t  SEL_BOR3V3_POS_DEGLITCH  :  2;
    uint16_t                          : 16; /*   (reserved) */
  };
  uint32_t WORD;
} CRGA_BORDEGLITCH_t; /* +0x018 */

typedef union {
  struct {
    uint8_t  WDTACTION                :  8; /*!< Watchdog action */
    uint8_t  WDTBARKCNT               :  8; /*!< WatchDog Bark Counter */
    uint8_t  WDTBARKCNTCLR            :  8; /*!< WatchDog Bark Counter Clear */
    uint8_t                           :  8; /*   (reserved) */
  };
  uint32_t WORD;
} CRGA_WDTACTION_t; /* +0x01C */

typedef union {
  struct {
    uint8_t  OVTEMP                   :  8; /*!< Over Temperature action */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  OVTEMP_LOCK              :  1;
  };
  uint32_t WORD;
} CRGA_OVTEMPACTION_t; /* +0x024 */

typedef union {
  struct {
    uint8_t  VTEMP_SEL                :  8; /*!< Over Temp protect threshold temp trim ,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t  OVERTEMP_EN              :  1; /*!< OverTemp Monitor Enable bit ,          NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t  TEMPSENSE_EN             :  1; /*!< enable Temp Sensor Analog Part ,       NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t                           :  6; /*   (reserved) */
  };
  uint32_t WORD;
} CRGA_OVTEMPCONFIG_t; /* +0x028 */

typedef union {
    struct {
      uint8_t  OV_ACTION                :  1; /*!< Over Voltage action */
      uint8_t                           :  3; /*   (reserved) */
      uint8_t  UV_ACTION                :  1; /*!< Under Voltage action */
      uint8_t                           :  3; /*   (reserved) */
      uint8_t                           :  8; /*   (reserved) */
      uint8_t                           :  8; /*   (reserved) */
      uint8_t                           :  3; /*   (reserved) */
      uint8_t  OV_LOCK                  :  1;
      uint8_t                           :  3; /*   (reserved) */
      uint8_t  UV_LOCK                  :  1;
    };
    uint32_t WORD;
}CRGA_OVUVACTION_t; /* +0x02C */


typedef struct {
  CRGA_LFCLKCTRL_t      LFCLKCTRL;
  CRGA_SYSCLKCTRL_t     SYSCLKCTRL;
  CRGA_RESETCTRL_t      RESETCTRL;
  uint8_t  MODULERSTREQ;                      /*<! Module Soft reset request +0x00C */
  uint8_t  _RESERVED_0D[3];                   /* +0x00D */
  CRGA_BORACTION_t      BORACTION;
  CRGA_BORCONFIG_t      BORCONFIG;
  CRGA_BORDEGLITCH_t    BORDEGLITCH;
  CRGA_WDTACTION_t      WDTACTION;
  uint8_t  KILLLFRC;                          /*<! Kill slow RC oscillator +0x020 */
  uint8_t  _RESERVED_21[3];                   /* +0x021 */
  CRGA_OVTEMPACTION_t   OVTEMPACTION;
  CRGA_OVTEMPCONFIG_t   OVTEMPCONFIG;
  CRGA_OVUVACTION_t     OVUVACTION;
} CRGA_SFRS_t;

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
 * @brief The starting address of CRGA SFRS.
 */
#define CRGA_REG_LFCLKCTRL              (*(__IO    CRGA_LFCLKCTRL_t *)(0x50000000))
#define CRGA_REG_SYSCLKCTRL             (*(__IO   CRGA_SYSCLKCTRL_t *)(0x50000004))
#define CRGA_REG_RESETCTRL              (*(__IO    CRGA_RESETCTRL_t *)(0x50000008))
#define CRGA_REG_MODULERSTREQ           (*(__IO             uint8_t *)(0x5000000C))
#define CRGA_REG_BORACTION              (*(__IO    CRGA_BORACTION_t *)(0x50000010))
#define CRGA_REG_BORCONFIG              (*(__IO    CRGA_BORCONFIG_t *)(0x50000014))     /* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE */
#define CRGA_REG_BORDEGLITCH            (*(__IO  CRGA_BORDEGLITCH_t *)(0x50000018))
#define CRGA_REG_WDTACTION              (*(__IO    CRGA_WDTACTION_t *)(0x5000001C))
#define CRGA_REG_KILLLFRC               (*(__IO             uint8_t *)(0x50000020))
#define CRGA_REG_OVTEMPACTION           (*(__IO CRGA_OVTEMPACTION_t *)(0x50000024))
#define CRGA_REG_OVTEMPCONFIG           (*(__IO CRGA_OVTEMPCONFIG_t *)(0x50000028))
#define CRGA_REG_OVUVACTION             (*(__IO   CRGA_OVUVACTION_t *)(0x5000002C))

#define CRGA_SFRS ((__IO CRGA_SFRS_t *)0x50000000)

#endif /* end of __CRGA_SFR_H__ section */


