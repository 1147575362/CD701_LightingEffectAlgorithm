/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file bte_sfr.h
 */

#ifndef BTE_SFR_H__
#define BTE_SFR_H__

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
 * @brief A structure to represent Special Function Registers for BTE.
 */

typedef union {
  struct {
    uint16_t BXADD                    : 16; /*!< Address of the ASIC die (LSB) */
    uint8_t  BXNUM                    :  8; /*!< Number of 32-bit words to transfer */
    uint8_t  INC_ADDR                 :  1;
    uint8_t  TX_DIR                   :  1;
    uint8_t  BLOCKING                 :  1;
    uint8_t  START                    :  1;
    uint8_t                           :  4; /*   (reserved) */
  };
  uint32_t WORD;
} BTE_CTRL_t; /* +0x000 */

typedef struct {
  BTE_CTRL_t CTRL;
  uint16_t BXSRAMADDR;                        /*<! Address of the SRAM (LSB) +0x004 */
  uint8_t  _RESERVED_06[2];                   /* +0x006 */
} BTE_SFRS_t;

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
 * @brief The starting address of BTE SFRS.
 */
#define BTE_REG_CTRL            (*(__IO BTE_CTRL_t *)(0x50000080))
#define BTE_REG_BXSRAMADDR      (*(__IO   uint16_t *)(0x50000084))

#define BTE_SFRS ((__IO BTE_SFRS_t *)0x50000080)

#endif /* end of __BTE_SFR_H__ section */


