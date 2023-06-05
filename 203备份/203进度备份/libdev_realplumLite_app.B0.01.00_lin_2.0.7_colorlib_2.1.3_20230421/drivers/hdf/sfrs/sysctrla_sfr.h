/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file sysctrla_sfr.h
 */

#ifndef SYSCTRLA_SFR_H__
#define SYSCTRLA_SFR_H__

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
 * @brief A structure to represent Special Function Registers for SYSCTRLA.
 */

typedef union {
  struct {
    uint8_t  DEBUG_ACCESS_KEY         :  8;
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  DEBUG_LOCK               :  1;
  };
  uint32_t WORD;
}SYSCTRLA_DEBUG_ACCESS_KEY_t; /* +0x008 */


typedef union {
  struct {
    uint8_t  TRIM_ACCESS_KEY          :  8;
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  TRIM_LOCK                :  1;
  };
  uint32_t WORD;
}SYSCTRLA_TRIM_ACCESS_KEY_t; /* +0x010 */


typedef union {
  struct {
    uint8_t  PMU_ACCESS_KEY           :  8;
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  PMU_LOCK                 :  1;
  };
  uint32_t WORD;
}SYSCTRLA_PMU_ACCESS_KEY_t; /* +0x018 */

typedef union {
  struct {
    uint8_t  TRIM_HF_RC               :  8; /*!< High Frequency RC Oscillator trim */
    uint8_t  SSCENA                   :  1; /*!< SSC Enable */
    uint8_t  SSCDEEP                  :  3; /*!< SSC Depth Configuration */
    uint8_t                           :  4; /*   (reserved) */
    uint8_t  SSCDIV                   :  8; /*!< SSC Clock Divider */
    uint8_t                           :  8; /*   (reserved) */
  };
  uint32_t WORD;
}SYSCTRLA_HF_OSC_TRIM_t; /* +0x024 */


typedef union {
  struct {
    uint8_t  LEDBIASSEL               :  1; /*!< High Voltage LED bias select */
    uint8_t  LEDBIASREG               :  1; /*!< High Voltage LED bias select register */
    uint8_t                           :  6; /*   (reserved) */
    uint32_t                          : 24; /*   (reserved) */
  };
  uint32_t WORD;
}SYSCTRLA_BIAS_t; /* +0x028 */

typedef union{
  struct {
      uint8_t  _RESERVED0;                      /* +0x028 */
      uint8_t  _RESERVED1;                      /* +0x029 */
      uint16_t TRIM;                             /*<! LED trim (120uA step) +0x02A */
  };
  uint32_t WORD;
}SYSCTRLA_TRIMLED_t;


typedef union {
  struct {
    uint8_t  LINS_TX_SLOPE            :  8; /*NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t  LINS_TX_BIAS_BOOST       :  3; /*NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  LINS_RX_BIAS_BOOST       :  2; /*NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t                           :  2; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t  LINS_TX_DUTY             :  8; /*NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
  };
  uint32_t WORD;
}SYSCTRLA_LIN_t; /* +0x040 */


typedef union {
  struct {
    uint8_t  DFTCODE                  :  8;
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  7; /*   (reserved) */
    uint8_t  DFT_LOCK                 :  1;
  };
  uint32_t WORD;
}SYSCTRLA_DFTCODE_t; /* +0x044 */
  

typedef struct {

  uint8_t                       RETAIN0;                           /*<! NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
  uint8_t                       _RESERVED_01[3];                   /* +0x001 */
  uint8_t                       RETAIN1;                           /*<! Firmware scratch register 1 (0x1) +0x004 */
  uint8_t                       _RESERVED_05[3];                   /* +0x005 */
  SYSCTRLA_DEBUG_ACCESS_KEY_t   DEBUG_ACCESS_KEY; /* +0x008 */
  uint8_t                       DEBUG_ACCESS_ENABLED;              /* +0x00C */
  uint8_t                       _RESERVED_0D[3];                   /* +0x00D */

  SYSCTRLA_TRIM_ACCESS_KEY_t    TRIM_ACCESS_KEY; /* +0x010 */

  uint8_t                       TRIM_ACCESS_ENABLED;               /* +0x014 */
  uint8_t                       _RESERVED_15[3];                   /* +0x015 */

  SYSCTRLA_PMU_ACCESS_KEY_t     PMU_ACCESS_KEY; /* +0x018 */

  uint8_t                       PMU_ACCESS_ENABLED;                /* +0x01C */
  uint8_t                       _RESERVED_1D[3];                   /* +0x01D */

  uint8_t                       TRIM_LF_RC;                        /*<! LF RC oscillator trim ,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE */
  uint8_t                       _RESERVED_21[3];                   /* +0x021 */

  SYSCTRLA_HF_OSC_TRIM_t        HF_OSC_TRIM; /* +0x024 */

  SYSCTRLA_BIAS_t               BIAS; /* +0x028 */

  uint8_t                       LEDBIASTRIM;                       /*<! LED bias current trim +0x02C */
  uint8_t                       _RESERVED_2D[3];                   /* +0x02D */

  SYSCTRLA_TRIMLED_t            LED[3];

  uint8_t                       TRIMVFW;                           /*<! PN Forward Voltage Current trim (10uA step max about 2.56mA) +0x03C */
  uint8_t                       _RESERVED_3D[3];                   /* +0x03D */

  SYSCTRLA_LIN_t                LIN; /* +0x040 NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/

  SYSCTRLA_DFTCODE_t            DFTCODE; /* +0x044 */

  uint8_t                       DFT_ACCESS_ENABLED;                /* +0x048 */
  uint8_t                       _RESERVED_49[3];                   /* +0x049 */

  uint8_t                       DFTTESTMODESTART;                  /* +0x04C */
  uint8_t                       _RESERVED_4D[3];                   /* +0x04D */

  uint32_t                      NAME;                              /*<! ASIC name +0x050 */

  uint16_t                      REV;                               /*<! Silicon Revision +0x054 */
  uint8_t                       _RESERVED_56[2];                   /* +0x056 */

  uint8_t                       ENABORTESTMODE;                    /*<! BOR Testmode Enable +0x058 */
  uint8_t                       _RESERVED_59[3];                   /* +0x059 */

} SYSCTRLA_SFRS_t;

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
 * @brief The starting address of SYSCTRLA SFRS.
 */
#define SYSCTRLA_REG_RETAIN0                            (*(__IO                     uint8_t *)(0x50012000))/*  please don't use for it's reserved for system use  NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE */
#define SYSCTRLA_REG_RETAIN1                            (*(__IO                     uint8_t *)(0x50012004))/*  please don't use for it's reserved for system use   */
#define SYSCTRLA_REG_DEBUG_ACCESS_KEY                   (*(__IO SYSCTRLA_DEBUG_ACCESS_KEY_t *)(0x50012008))
#define SYSCTRLA_REG_DEBUG_ACCESS_ENABLED               (*(__IO                     uint8_t *)(0x5001200C))
#define SYSCTRLA_REG_TRIM_ACCESS_KEY                    (*(__IO  SYSCTRLA_TRIM_ACCESS_KEY_t *)(0x50012010))
#define SYSCTRLA_REG_TRIM_ACCESS_ENABLED                (*(__IO                     uint8_t *)(0x50012014))
#define SYSCTRLA_REG_PMU_ACCESS_KEY                     (*(__IO   SYSCTRLA_PMU_ACCESS_KEY_t *)(0x50012018))
#define SYSCTRLA_REG_PMU_ACCESS_ENABLED                 (*(__IO                     uint8_t *)(0x5001201C))
#define SYSCTRLA_REG_TRIM_LF_RC                         (*(__IO                     uint8_t *)(0x50012020))/* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE  */
#define SYSCTRLA_REG_HF_OSC_TRIM                        (*(__IO      SYSCTRLA_HF_OSC_TRIM_t *)(0x50012024))
#define SYSCTRLA_REG_BIAS                               (*(__IO             SYSCTRLA_BIAS_t *)(0x50012028))
#define SYSCTRLA_REG_TRIM_LED(x)                        (*(__IO          SYSCTRLA_TRIMLED_t *)(0x5001202C + x*4U))
#define SYSCTRLA_REG_TRIMVFW                            (*(__IO                     uint8_t *)(0x5001203C))
#define SYSCTRLA_REG_LIN                                (*(__IO              SYSCTRLA_LIN_t *)(0x50012040))/* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE */
#define SYSCTRLA_REG_DFTCODE                            (*(__IO          SYSCTRLA_DFTCODE_t *)(0x50012044))
#define SYSCTRLA_REG_DFT_ACCESS_ENABLED                 (*(__IO                     uint8_t *)(0x50012048))
#define SYSCTRLA_REG_DFTTESTMODESTART                   (*(__IO                     uint8_t *)(0x5001204C))
#define SYSCTRLA_REG_NAME                               (*(__IO                    uint32_t *)(0x50012050))
#define SYSCTRLA_REG_REV                                (*(__IO                    uint16_t *)(0x50012054))
#define SYSCTRLA_REG_ENABORTESTMODE                     (*(__IO                     uint8_t *)(0x50012058))

#define SYSCTRLA_SFRS ((__IO SYSCTRLA_SFRS_t *)0x50012000)

#endif /* end of __SYSCTRLA_SFR_H__ section */


