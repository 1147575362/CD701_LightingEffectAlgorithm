/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file wdta_sfr.h
 */

#ifndef WDTA_SFR_H__
#define WDTA_SFR_H__

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
 * @brief A structure to represent Special Function Registers for WDTA.
 */

typedef union {
  struct {
    uint8_t  UPDATE                   :  1; /*!< Window Mode Enable */
    uint8_t  RUNNING                  :  1; /*!< Running status */
    uint8_t                           :  6; /*   (reserved) */
    uint8_t  TIMEOUTSEL               :  3; /*!< Timeout select */
    uint8_t  WINOPENENA               :  1; /*!< Window Mode Enable */
    uint8_t  WINOPENSEL               :  2; /*!< Window Mode open select */
    uint8_t  WINOPENFLAG              :  1; /*!< Window open flag */
    uint8_t                           :  1; /*   (reserved) */
    uint16_t                          : 16; /*   (reserved) */
  };
  uint32_t WORD;
}WDTA_CTRL_t; /* +0x000 */


typedef union {
  struct {
    uint8_t  STOP                     :  8;
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  STOP_LOCK                :  1;
  };
  uint32_t WORD;
}WDTA_STOP_t; /* +0x004 */


typedef struct {
  union {
    struct {
      uint8_t  WDTA                 :  1; /*!< WDTA timeout Interrupt Enable */
      uint8_t  WINOPEN              :  1; /*!< Window Open Interrupt Enable */
      uint8_t                       :  6; /*   (reserved) */
    };
    uint8_t BYTE;
  } ENABLE;
  union {
    struct {
      uint8_t  WDTA                 :  1; /*!< WDTA timeout Interrupt Clear */
      uint8_t  WINOPEN              :  1; /*!< Window Open Interrupt Clear */
      uint8_t                       :  6; /*   (reserved) */
    };
    uint8_t BYTE;
  } CLEAR;
  union {
    struct {
      uint8_t  WDTA                 :  1; /*!< WDTA timeout Status */
      uint8_t  WINOPEN              :  1; /*!< Window Open Status */
      uint8_t                       :  6; /*   (reserved) */
    };
    uint8_t BYTE;
  } STATUS;
  union {
    struct {
      uint8_t  WDTA                 :  1; /*!< WDTA timeout Interrupt */
      uint8_t  WINOPEN              :  1; /*!< Window Open Interrupt */
      uint8_t                       :  6; /*   (reserved) */
    };
    uint8_t BYTE;
  } IRQ;
}WDTA_INT_t; /* +0x010 */


typedef struct {

  WDTA_CTRL_t   CTRL;   /* +0x000 */
  WDTA_STOP_t   STOP;   /* +0x004 */
  uint32_t      CLEAR;  /* +0x008 */
  uint32_t      CNTVAL; /*<! Counter value +0x00C */
  WDTA_INT_t    INT;    /* +0x010 */
} WDTA_SFRS_t;

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
 * @brief The starting address of WDTA SFRS.
 */
#define WDTA_REG_CTRL           (*(__IO   WDTA_CTRL_t *)(0x50010300))
#define WDTA_REG_STOP           (*(__IO   WDTA_STOP_t *)(0x50010304))
#define WDTA_REG_CLEAR          (*(__IO      uint32_t *)(0x50010308))
#define WDTA_REG_CNTVAL         (*(__IO      uint32_t *)(0x5001030C))
#define WDTA_REG_INT            (*(__IO    WDTA_INT_t *)(0x50010310))

#define WDTA_SFRS ((__IO WDTA_SFRS_t *)0x50010300)

#endif /* end of __WDTA_SFR_H__ section */


