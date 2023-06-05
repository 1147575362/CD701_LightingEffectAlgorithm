/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file pwm_sfr.h
 */

#ifndef PWM_SFR_H__
#define PWM_SFR_H__

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
 * @brief A structure to represent Special Function Registers for PWM.
 */
typedef union {
  struct {
    uint8_t                           :  8; /*   (reserved) */
    uint8_t  PRESCALESEL              :  8; /*!< Prescaler select */
    uint16_t PERIOD                   : 16;
  };
  uint32_t WORD;
} PWM_BASE_t; /* +0x000 */


typedef union {
  struct {
    uint8_t  ENAREQ                   :  8;
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t  ENAREQALL                :  1;
    uint8_t  CLRREQALL                :  1;
    uint8_t  FORCEINACTIVE            :  1;
    uint8_t                           :  5; /*   (reserved) */
  };
  uint32_t WORD;
} PWM_ENAREQ_t; /* +0x008 */


typedef union {
  struct {
    uint16_t PFALL                   : 16; /*!< Pulse Fall */
    uint16_t PRISE                   : 16; /*!< Pulse Rise */
  };
  uint32_t WORD;
} PWM_PULSE_t; /* +0x01C */




typedef union {
  struct {
    union {
      struct {
        uint8_t  PERIOD               :  1; /*!< Period Interrupt enable */
        uint8_t  UPD                  :  1; /*!< Updated Interrupt enable */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } ENABLE;
    union {
      struct {
        uint8_t  PERIOD               :  1; /*!< Period Interrupt clear */
        uint8_t  UPD                  :  1; /*!< Updated Interrupt clear */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } CLEAR;
    union {
      struct {
        uint8_t  PERIOD               :  1; /*!< Period Interrupt status */
        uint8_t  UPD                  :  1; /*!< Updated Interrupt status */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } STATUS;
    union {
      struct {
        uint8_t  PERIOD               :  1; /*!< Period Interrupt active */
        uint8_t  UPD                  :  1; /*!< Updated Interrupt active */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } IRQ;
  };
  uint32_t WORD;
} PWM_INTPWM_t; /* +0x048 */


typedef struct {

  PWM_BASE_t BASE; /* +0x000 */
  uint16_t PWMCNT;                            /* +0x004 */
  uint8_t  _RESERVED_06[2];                   /* +0x006 */
  PWM_ENAREQ_t ENAREQ; /* +0x008 */
  uint8_t  ENASTS;                            /* +0x00C */
  uint8_t  _RESERVED_0D[3];                   /* +0x00D */
  uint8_t  INIT;                              /* +0x010 */
  uint8_t  _RESERVED_11[3];                   /* +0x011 */
  uint8_t  INVERT;                            /* +0x014 */
  uint8_t  _RESERVED_15[3];                   /* +0x015 */
  uint8_t  UPDATE;                            /* +0x018 */
  uint8_t  _RESERVED_19[3];                   /* +0x019 */
  PWM_PULSE_t PULSE[3];

  uint8_t  INTPOSEDGENA;                      /*<! Interrupt enable +0x028 */
  uint8_t  _RESERVED_29[3];                   /* +0x029 */

  uint8_t  INTNEGEDGENA;                      /*<! Interrupt enable +0x02C */
  uint8_t  _RESERVED_2D[3];                   /* +0x02D */

  uint8_t  INTPOSEDGCLR;                      /*<! Interrupt clear +0x030 */
  uint8_t  _RESERVED_31[3];                   /* +0x031 */

  uint8_t  INTNEGEDGCLR;                      /*<! Interrupt clear +0x034 */
  uint8_t  _RESERVED_35[3];                   /* +0x035 */

  uint8_t  INTPOSEDGSTS;                      /*<! Interrupt status +0x038 */
  uint8_t  _RESERVED_39[3];                   /* +0x039 */

  uint8_t  INTNEGEDGSTS;                      /*<! Interrupt status +0x03C */
  uint8_t  _RESERVED_3D[3];                   /* +0x03D */

  uint8_t  INTPOSEDGIRQ;                      /*<! Interrupt active +0x040 */
  uint8_t  _RESERVED_41[3];                   /* +0x041 */

  uint8_t  INTNEGEDGIRQ;                      /*<! Interrupt active +0x044 */
  uint8_t  _RESERVED_45[3];                   /* +0x045 */

  PWM_INTPWM_t INTPWM; /* +0x048 */

} PWM_SFRS_t;

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
 * @brief The starting address of PWM SFRS.
 */
#define PWM_REG_BASE                    (*(__IO   PWM_BASE_t *)(0x50010600))
#define PWM_REG_PWMCNT                  (*(__IO     uint16_t *)(0x50010604))
#define PWM_REG_ENAREQ                  (*(__IO PWM_ENAREQ_t *)(0x50010608))
#define PWM_REG_ENASTS                  (*(__IO      uint8_t *)(0x5001060C))
#define PWM_REG_INIT                    (*(__IO      uint8_t *)(0x50010610))
#define PWM_REG_INVERT                  (*(__IO      uint8_t *)(0x50010614))
#define PWM_REG_UPDATE                  (*(__IO      uint8_t *)(0x50010618))
#define PWM_REG_PULSE(index)            (*(__IO  PWM_PULSE_t *)(0x5001061C + index*4)) /* index: 0-23 */
#define PWM_REG_INTPOSEDGENA            (*(__IO      uint8_t *)(0x50010628))
#define PWM_REG_INTNEGEDGENA            (*(__IO      uint8_t *)(0x5001062C))
#define PWM_REG_INTPOSEDGCLR            (*(__IO      uint8_t *)(0x50010630))
#define PWM_REG_INTNEGEDGCLR            (*(__IO      uint8_t *)(0x50010634))
#define PWM_REG_INTPOSEDGSTS            (*(__IO      uint8_t *)(0x50010638))
#define PWM_REG_INTNEGEDGSTS            (*(__IO      uint8_t *)(0x5001063C))
#define PWM_REG_INTPOSEDGIRQ            (*(__IO      uint8_t *)(0x50010640))
#define PWM_REG_INTNEGEDGIRQ            (*(__IO      uint8_t *)(0x50010644))
#define PWM_REG_INTPWM                  (*(__IO PWM_INTPWM_t *)(0x50010648))


#define PWM_SFRS ((__IO PWM_SFRS_t *)0x50010600)

#endif /* end of __PWM_SFR_H__ section */


