/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file gpio_sfr.h
 */

#ifndef GPIO_SFR_H__
#define GPIO_SFR_H__

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
 * @brief A structure to represent Special Function Registers for GPIO.
 */
typedef union {
  struct {
    uint8_t  GPAENA                   :  1;
    uint8_t                           :  7; /*   (reserved) */
    uint32_t                          : 24; /*   (reserved) */
  };
  uint32_t WORD;
}GPIO_GPENA_t;

typedef struct {
  uint8_t  DIR                  :  1; /*!< GPIO PIN output enable */
  uint8_t  IE                   :  1; /*!< GPIO PIN interrupt mask */
  uint8_t  EDGE                 :  2; /* 00: disable, 01: rising, 10: falling, 11: both */
  uint8_t  CLR                  :  1; /*!< GPIO PIN interrupt clear */
  uint8_t  ACTDET               :  1; /*!< GPIO PIN activity interrupt */
  uint8_t  ACTDETRE             :  1; /*!< GPIO PIN activity rise status */
  uint8_t  ACTDETFE             :  1; /*!< GPIO PIN activity fall status */
} GPIO_Cfg_t; /* +0x1000 */


typedef struct {

  uint8_t       GPADATA[1024];                        /*<! Port A data +0x000 */
  uint32_t      _RESERVED_08[256];
  GPIO_GPENA_t  GPAENA;                       /* +0x800 */
  uint32_t      _RESERVED_804[511];
  GPIO_Cfg_t    GPIO_CFG[5];
  uint8_t       _RESERVED_805[3];
} GPIO_SFRS_t;

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
 * @brief The starting address of GPIO SFRS.
 */
#define GPIO_REG_GPENA          (*(__IO  GPIO_GPENA_t *)(0x50018800))
#define GPIO_REG_GPIO_CFG(x)    (*(__IO    GPIO_Cfg_t *)(0x50019000 + x))

#define GPIO_SFRS ((__IO GPIO_SFRS_t *)0x50018000)

#endif /* end of __GPIO_SFR_H__ section */


