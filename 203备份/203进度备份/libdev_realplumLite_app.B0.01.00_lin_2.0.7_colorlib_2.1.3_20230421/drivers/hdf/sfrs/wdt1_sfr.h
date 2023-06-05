/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file wdt1_sfr.h
 */

#ifndef WDT1_SFR_H__
#define WDT1_SFR_H__

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
 * @brief A structure to represent Special Function Registers for WDT1.
 */
typedef union {
  struct {
    uint8_t  ENA                      :  1; /*!< WDT Enable */
    uint8_t  RSTEN                    :  1; /*!< Reset enable */
    uint8_t  RSTFLAG                  :  1; /*!< Reset flag */
    uint8_t  PRESET                   :  2;
    uint8_t                           :  3; /*   (reserved) */
    uint32_t                          : 24; /*   (reserved) */
  };
  uint32_t WORD;
} WDT1_CFG_t; /* +0x000 */

typedef struct {
  WDT1_CFG_t CFG; /* +0x000 */
  uint32_t KEY;                               /* +0x004 */
} WDT1_SFRS_t;

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
 * @brief The starting address of WDT1 SFRS.
 */
#define WDT1_REG_CFG (*(__IO WDT1_CFG_t *)(0x50020018))
#define WDT1_REG_KEY (*(__IO   uint32_t *)(0x5002001C))

#define WDT1_SFRS ((__IO WDT1_SFRS_t *)0x50020018)

#endif /* end of __WDT1_SFR_H__ section */


