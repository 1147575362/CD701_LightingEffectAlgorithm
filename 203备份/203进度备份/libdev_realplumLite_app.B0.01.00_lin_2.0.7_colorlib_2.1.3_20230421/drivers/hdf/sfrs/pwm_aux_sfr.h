/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file pwm_aux_sfr.h
 */

#ifndef PWM_AUX_SFR_H__
#define PWM_AUX_SFR_H__

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
 * @brief A structure to represent Special Function Registers for PWM_AUX.
 */

typedef struct {
  struct {
    uint8_t                          :  8; /*   (reserved) */
    uint8_t  PRESCALESEL             :  8; /*!< Prescaler select */
    uint16_t PERIOD                  : 16; /*!< Period */
  }BASE; /* +0x000 */
  uint16_t PWMCNT;                          /* +0x004 */
  uint8_t  _RESERVED[2];                   /* +0x006 */
}PWM_AUX_t;

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
}PWM_AUX_ENAREQ_t; /* +0x014 */

typedef union {
  struct {
    uint16_t PFALL                   : 16; /*!< Pulse Fall */
    uint16_t PRISE                   : 16; /*!< Pulse Rise */
  };
  uint32_t WORD;
}PWM_AUX_PULSE_t; /* +0x028 */

typedef union {
  struct {
    union {
      struct {
        uint8_t  PERIOD               :  8; /*!< Period Interrupt enable */
      };
      uint8_t BYTE;
    } ENABLE;
    union {
      struct {
        uint8_t  PERIOD               :  8; /*!< Period Interrupt clear */
      };
      uint8_t BYTE;
    } CLEAR;
    union {
      struct {
        uint8_t  PERIOD               :  8; /*!< Period Interrupt status */
      };
      uint8_t BYTE;
    } STATUS;
    union {
      struct {
        uint8_t  PERIOD               :  8; /*!< Period Interrupt active */
      };
      uint8_t BYTE;
    } IRQ;
  };
  uint32_t WORD;
}PWM_AUX_INTPERIOD_t; /* +0x050 */


typedef union {
  struct {
    union {
      struct {
        uint8_t  UPD                  :  8; /*!< Updated Interrupt enable */
      };
      uint8_t BYTE;
    } ENABLE;
    union {
      struct {
        uint8_t  UPD                  :  8; /*!< Updated Interrupt clear */
      };
      uint8_t BYTE;
    } CLEAR;
    union {
      struct {
        uint8_t  UPD                  :  8; /*!< Updated Interrupt status */
      };
      uint8_t BYTE;
    } STATUS;
    union {
      struct {
        uint8_t  UPD                  :  8; /*!< Updated Interrupt active */
      };
      uint8_t BYTE;
    } IRQ;
  };
  uint32_t WORD;
}PWM_AUX_INTUPDATED_t; /* +0x054 */


typedef struct {
  PWM_AUX_t             PWM_AUX[2];
  uint8_t               BASE_SEL;                          /* +0x010 */
  uint8_t               _RESERVED_11[3];                   /* +0x011 */
  PWM_AUX_ENAREQ_t      ENAREQ;
  uint8_t               ENASTS;                            /* +0x018 */
  uint8_t               _RESERVED_19[3];                   /* +0x019 */
  uint8_t               INIT;                              /* +0x01C */
  uint8_t               _RESERVED_1D[3];                   /* +0x01D */
  uint8_t               INVERT;                            /* +0x020 */
  uint8_t               _RESERVED_21[3];                   /* +0x021 */
  uint8_t               UPDATE;                            /* +0x024 */
  uint8_t               _RESERVED_25[3];                   /* +0x025 */
  PWM_AUX_PULSE_t       PULSE[4];
  uint8_t               INTPOSEDGENA;                      /*<! Interrupt enable +0x038 */
  uint8_t               _RESERVED_39[3];                   /* +0x039 */
  uint8_t               INTNEGEDGENA;                      /*<! Interrupt enable +0x03C */
  uint8_t               _RESERVED_3D[3];                   /* +0x03D */
  uint8_t               INTPOSEDGCLR;                      /*<! Interrupt clear +0x040 */
  uint8_t               _RESERVED_41[3];                   /* +0x041 */
  uint8_t               INTNEGEDGCLR;                      /*<! Interrupt clear +0x044 */
  uint8_t               _RESERVED_45[3];                   /* +0x045 */
  uint8_t               INTPOSEDGSTS;                      /*<! Interrupt status +0x048 */
  uint8_t               _RESERVED_49[3];                   /* +0x049 */
  uint8_t               INTNEGEDGSTS;                      /*<! Interrupt status +0x04C */
  uint8_t               _RESERVED_4D[3];                   /* +0x04D */
  uint8_t               INTPOSEDGIRQ;                      /*<! Interrupt active +0x050 */
  uint8_t               _RESERVED_51[3];                   /* +0x051 */
  uint8_t               INTNEGEDGIRQ;                      /*<! Interrupt active +0x054 */
  uint8_t               _RESERVED_55[3];                   /* +0x055 */
  PWM_AUX_INTPERIOD_t   INTPERIOD; /* +0x050 */
  PWM_AUX_INTUPDATED_t  INTUPDATED; /* +0x054 */

} PWM_AUX_SFRS_t;

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
 * @brief The starting address of PWM_AUX SFRS.
 */
#define PWM_AUX_REG_BASE(x)             (*(__IO            PWM_AUX_t *)(0x50010900+ x*8))
#define PWM_AUX_REG_BASE_SEL            (*(__IO              uint8_t *)(0x50010910))
#define PWM_AUX_REG_ENAREQ              (*(__IO     PWM_AUX_ENAREQ_t *)(0x50010914))
#define PWM_AUX_REG_ENASTS              (*(__IO              uint8_t *)(0x50010918))
#define PWM_AUX_REG_INIT                (*(__IO              uint8_t *)(0x5001091C))
#define PWM_AUX_REG_INVERT              (*(__IO              uint8_t *)(0x50010920))
#define PWM_AUX_REG_UPDATE              (*(__IO              uint8_t *)(0x50010924))
#define PWM_AUX_REG_PULSE(x)            (*(__IO      PWM_AUX_PULSE_t *)(0x50010928U+x*4U))
#define PWM_AUX_REG_INTPOSEDGENA        (*(__IO              uint8_t *)(0x50010938))
#define PWM_AUX_REG_INTNEGEDGENA        (*(__IO              uint8_t *)(0x5001093C))
#define PWM_AUX_REG_INTPOSEDGCLR        (*(__IO              uint8_t *)(0x50010940))
#define PWM_AUX_REG_INTNEGEDGCLR        (*(__IO              uint8_t *)(0x50010944))
#define PWM_AUX_REG_INTPOSEDGSTS        (*(__IO              uint8_t *)(0x50010948))
#define PWM_AUX_REG_INTNEGEDGSTS        (*(__IO              uint8_t *)(0x5001094C))
#define PWM_AUX_REG_INTPOSEDGIRQ        (*(__IO              uint8_t *)(0x50010950))
#define PWM_AUX_REG_INTNEGEDGIRQ        (*(__IO              uint8_t *)(0x50010954))
#define PWM_AUX_REG_INTPERIOD           (*(__IO  PWM_AUX_INTPERIOD_t *)(0x50010958))
#define PWM_AUX_REG_INTUPDATED          (*(__IO PWM_AUX_INTUPDATED_t *)(0x5001095C))


#define PWM_AUX_SFRS ((__IO PWM_AUX_SFRS_t *)0x50010900)

#endif /* end of __PWM_AUX_SFR_H__ section */


