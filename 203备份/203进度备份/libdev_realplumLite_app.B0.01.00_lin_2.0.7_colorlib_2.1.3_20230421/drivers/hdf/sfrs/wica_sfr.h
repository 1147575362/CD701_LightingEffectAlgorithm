/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file wica_sfr.h
 */

#ifndef WICA_SFR_H__
#define WICA_SFR_H__

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
 * @brief A structure to represent Special Function Registers for WICA.
 */
typedef union {
  struct {
    uint16_t  LINS_ENA                 :  1; /*!< LIN Slave Wakeup Enable */
    uint16_t  LINS_WICA_SEL            :  1; /*!< LIN Slave Wakeup Source Select */
    uint16_t  TIMER_ENA                :  1; /*!< Wakeup Timer Enable */
    uint16_t                           :  2; /*   (reserved) */
    uint16_t  TIMER_TAPSEL             :  4; /*!< WakeUp Timer Tap Select !!misaligned!! */
    uint16_t  LINS_IRQENA              :  1; /*!< LIN Slave Wakeup Interrupt Enable */
    uint16_t  TIMER_IRQENA             :  1; /*!< Timer Wakeup Interrupt Enable */
    uint16_t                           :  1; /*   (reserved) */
    uint16_t  LINS_IRQCLR              :  1; /*!< clear the wulin_irq */
    uint16_t                           :  1; /*   (reserved) */
    uint16_t  TIMER_IRQCLR             :  1; /*!< clear the wutimer_irq */
    uint16_t                           :  1; /*   (reserved) */
    uint16_t                           : 16; /*   (reserved) */
  };
  uint32_t WORD;
} WICA_CTRL_t; /* +0x000 */

typedef union {
  struct {
    uint8_t  LINS                     :  1; /*!< LIN Slave Wakeup Status */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  TIMER                    :  1; /*!< Wakeup Timer Status */
    uint8_t                           :  5; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint16_t TIMERCNT                 : 16; /*!< Wakeup Timer Counter Value */
  };
  uint32_t WORD;
} WICA_STATUS_t; /* +0x004 */

typedef struct {
  WICA_CTRL_t   CTRL; /* +0x000 */
  WICA_STATUS_t STATUS; /* +0x004 */
} WICA_SFRS_t;

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
 * @brief The starting address of WICA SFRS.
 */
#define WICA_REG_CTRL           (*(__IO   WICA_CTRL_t *)(0x50010100))
#define WICA_REG_STATUS         (*(__IO WICA_STATUS_t *)(0x50010104))

#define WICA_SFRS ((__IO WICA_SFRS_t *)0x50010100)

#endif /* end of __WICA_SFR_H__ section */


