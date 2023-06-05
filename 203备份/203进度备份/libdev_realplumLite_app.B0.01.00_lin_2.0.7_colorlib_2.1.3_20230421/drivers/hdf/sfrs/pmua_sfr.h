/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file pmua_sfr.h
 */

#ifndef PMUA_SFR_H__
#define PMUA_SFR_H__

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
 * @brief A structure to represent Special Function Registers for PMUA.
 */
typedef union {
  struct {
    uint8_t  HIBERNATE                :  1;
    uint8_t  FASTBOOT                 :  1; /*!< Fast boot */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  BG_OK                    :  1; /*!< the flag of bandgap OK */
    uint8_t                           :  3; /*!<   (reserved) */
    uint8_t  DBG_EN_LOWIQ             :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  DBG_DIS_BG_UVLO          :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  DBG_EN_LP_CAPLESS        :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  VDD1V5_LDO_CHOOSE        :  1; /*!< reserved   */
    uint8_t  OPT_EN_LOWIQ             :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  OPT_EN_LP_CAPLESS        :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  IGNORE_CIFS              :  1; /*!< Ignore QACKs */
    uint8_t  DBG_TEST                 :  1;
    uint8_t                           :  1; /*!<   (reserved) */
    uint8_t  DBG_DISCHARGE_1V5        :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  DBG_DISCHARGE_3V3        :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  PD1V5_ENA_HIBERNATE      :  1; /*!< enable of 1V5 Power Domain at Hibernate mode,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  DBG_DIS_CP               :  1;
    uint8_t  DBG_DIS_LDO_3V3          :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  DBG_DIS_LDO_1V5          :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  DBG_DIS_BOR_3V3          :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  DBG_DIS_BOR_1V5          :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  DBG_ISO                  :  1; /*!< NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t                           :  1; /*!<   (reserved) */
    uint8_t  UPDATE                   :  1; /*!< PMUA Configure Update */
  };
  uint32_t WORD;
}PMUA_CTRL_t; /* +0x000 */


typedef union {
  struct {
    uint8_t  OCP_CTRL_1V5             :  4; /*!< OCP trim for 1.5V LDO(default 30mA) at PMU debug mode ,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode*/
    uint8_t  OCP_CTRL_3V3             :  4; /*!< OCP trim for 3P3V LDO(default 30mA) at PMU debug mode ,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode*/
    uint8_t  TRIM_VREF_BUF            :  8; /* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  TRIM_BG                  :  8; /* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t  VDD1V5_LDO_TRIM          :  2; /* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t                           :  2; /*   (reserved) */
    uint8_t  VDD3V3_LDO_TRIM          :  3; /* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE at PMU debug mode  */
    uint8_t                           :  1; /*   (reserved) */
  };
  uint32_t WORD;
} PMUA_TRIM_t; /* +0x004 */


typedef union {
  struct {
    uint8_t  ENABLE_1V5               :  4; /*!< Enable 1.5V dwell time */
    uint8_t  ATTACH_1V5               :  4; /*!< Attach 1.5V dwell time */
    uint8_t  ATTACH_3V3               :  4; /*!< Attach 3.3V dwell time */
    uint8_t  POWER_DOWN_MCU           :  4; /*!< Power down MCU dwell time */
    uint16_t                          : 16; /*   (reserved) */
  };
  uint32_t WORD;
}PMUA_DWELL_t; /* +0x008 */



typedef union {
  struct {
    uint8_t  BAT_UV_EN                :  1; /*!< Battery Under Voltage Monitor Enable,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE  */
    uint8_t  BAT_OV_EN                :  1; /*!< Battery Over Voltage Monitor Enable,NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE  */
    uint8_t  UV_POL                   :  1; /*!< Battery Voltage Monitor Under Voltage Interrupt Event Polarity */
    uint8_t  OV_POL                   :  1; /*!< Battery Voltage Monitor Over Voltage Interrupt Event Polarity */
    uint8_t                           :  4; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t                           :  8; /*   (reserved) */
    uint8_t  LOW                      :  1; /*!< Battery Voltage Low Status */
    uint8_t  HIGH                     :  1; /*!< Battery Voltage High Status */
/*    uint8_t  LOW_DBNC                 :  1; !< Battery Voltage Low Status after debouncing */
/*    uint8_t  HIGH_DBNC                :  1; !< Battery Voltage High Status after deboucing */
    uint8_t  BATT_STATE               :  2;
    uint8_t                           :  4; /*   (reserved) */
  };
  uint32_t WORD;
}PMUA_VBATCTRL_t; /* +0x00C */


typedef union {
  struct {
    uint8_t  UVLEVEL                  :  8; /*!< Battery Voltage Monitor Under Voltage Select ,                 NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t  UVHYS                    :  8; /*!< Battery Voltage Monitor Under Voltage Hysterisis Select ,      NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t  OVLEVEL                  :  8; /*!< Battery Voltage Monitor Over Voltage Select ,                  NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
    uint8_t  OVHYS                    :  8; /*!< Battery Voltage Monitor Over Voltage Hysterisis Select ,       NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE*/
  };
  uint32_t WORD;
}PMUA_VBATTRIM_t; /* +0x010 */


typedef union {
  struct {
    uint8_t  UV                       :  1; /*!< under voltage signal debounce enable */
    uint8_t  OV                       :  1; /*!< over voltage signal debounce enable */
    uint8_t  UVSTRB0SEL               :  1; /*!< Low frequency strobing select for debouncing */
    uint8_t  UVSTRB1SEL               :  1; /*!< Low frequency strobing select for debouncing */
    uint8_t  OVSTRB0SEL               :  1; /*!< Low frequency strobing select for debouncing */
    uint8_t  OVSTRB1SEL               :  1; /*!< Low frequency strobing select for debouncing */
    uint8_t                           :  2; /*   (reserved) */
    uint32_t                          : 24; /*   (reserved) */
  };
  uint32_t WORD;
}PMUA_VBATDBNC_t; /* +0x014 */



typedef union {
  struct {
    uint8_t  UVTHRES1                 :  8; /*!< Under Voltage debouncing threshold for 0to1 Transition */
    uint8_t  OVTHRES1                 :  8; /*!< Over Voltage debouncing threshold for 0to1 Transition */
    uint8_t  UVTHRES0                 :  8; /*!< Under Voltage debouncing threshold for 1to0 Transition */
    uint8_t  OVTHRES0                 :  8; /*!< Over Voltage debouncing threshold for 1to0 Transition */
  };
  uint32_t WORD;
}PMUA_VBATDBNCTHRES_t; /* +0x018 */



typedef union {
  struct {
    union {
      struct {
        uint8_t  UV                   :  1; /*!< under voltage interrupt enable */
        uint8_t  OV                   :  1; /*!< over voltage interrupt enable */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } ENABLE;
    union {
      struct {
        uint8_t  UV                   :  1; /*!< under voltage interrupt clear */
        uint8_t  OV                   :  1; /*!< over voltage interrupt clear */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } CLEAR;
    union {
      struct {
        uint8_t  UV                   :  1; /*!< under voltage interrupt status */
        uint8_t  OV                   :  1; /*!< over voltage interrupt status */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } STATUS;
    union {
      struct {
        uint8_t  UV                   :  1; /*!< under voltage interrupt active */
        uint8_t  OV                   :  1; /*!< over voltage interrupt active */
        uint8_t                       :  6; /*   (reserved) */
      };
      uint8_t BYTE;
    } IRQ;
  };
  uint32_t WORD;
}PMUA_PMUIRQ_t; /* +0x01C */


typedef struct {
  PMUA_CTRL_t           CTRL;
  PMUA_TRIM_t           TRIM;
  PMUA_DWELL_t          DWELL;
  PMUA_VBATCTRL_t       VBATCTRL;
  PMUA_VBATTRIM_t       VBATTRIM;
  PMUA_VBATDBNC_t       VBATDBNC;
  PMUA_VBATDBNCTHRES_t  VBATDBNCTHRES;
  PMUA_PMUIRQ_t         PMUIRQ;
} PMUA_SFRS_t;

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
 * @brief The starting address of PMUA SFRS.
 */
#define PMUA_REG_CTRL                   (*(__IO          PMUA_CTRL_t *)(0x50000040))
#define PMUA_REG_TRIM                   (*(__IO          PMUA_TRIM_t *)(0x50000044))
#define PMUA_REG_DWELL                  (*(__IO         PMUA_DWELL_t *)(0x50000048))
#define PMUA_REG_VBATCTRL               (*(__IO      PMUA_VBATCTRL_t *)(0x5000004C))
#define PMUA_REG_VBATTRIM               (*(__IO      PMUA_VBATTRIM_t *)(0x50000050))
#define PMUA_REG_VBATDBNC               (*(__IO      PMUA_VBATDBNC_t *)(0x50000054))
#define PMUA_REG_VBATDBNCTHRES          (*(__IO PMUA_VBATDBNCTHRES_t *)(0x50000058))
#define PMUA_REG_PMUAIRQ                (*(__IO        PMUA_PMUIRQ_t *)(0x5000005C))



#define PMUA_SFRS ((__IO PMUA_SFRS_t *)0x50000040)

#endif /* end of __PMUA_SFR_H__ section */


