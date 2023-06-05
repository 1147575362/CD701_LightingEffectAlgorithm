/**
 * @copyright 2021 indie Semiconductor
 *
 * This file is proprietary to indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of indie Semiconductor.
 *
 * @file lins_sfr.h
 */

#ifndef LINS_SFR_H__
#define LINS_SFR_H__

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


#define LINS_DL_LENGTH_POS              (0)
#define LINS_DL_DISAUTOSLEEP_POS        (5)
#define LINS_DL_DISBITMON_POS           (6)
#define LINS_DL_ENHCHK_POS              (7)

#define DATAACK_TPYE_RECEIVE              (0x01)
#define DATAACK_TPYE_TRANSMIT             (0x03)
/**
 * @brief A structure to represent Special Function Registers for LINS.
 */
typedef union{
  struct{
    uint8_t  DATA;                          /*<! Data Buffer 1 +0x000 */
    uint8_t  _RESERVED_01[3];                   /* +0x001 */
  };
  uint32_t WORD;
}LINS_DATABUF_t;


typedef union {
  struct {
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  WAKEUPREQ                :  1; /*!< WakeUp Request */
/*    uint8_t  RSTERR                   :  1; !< Reset Error */
/*    uint8_t  RSTINT                   :  1; !< Reset interrupt */
    uint8_t  RST_INT_ERR              :  2; /*!< Reset Reset interrupt, Error */
    uint8_t  DATAACK                  :  1; /*!< Data Acknowledgement */
    uint8_t  TRANSMIT                 :  1; /*!< Transmit Operation */
    uint8_t  SLEEP                    :  1; /*!< Sleep Request,NOTE: When the chip uses the LINs wakeup logic in pre5v domain(PMU_SFRS->CTRL.PD1V5_ENA_HIBERNATE=0x0), the write operation of this register takes effect by IOCTRLA_SFRS->LIN.UPDATE */
    uint8_t  STOP                     :  1; /*!< Stop Register */
  };
  uint8_t BYTE;
} LINS_CTRL_t; /* +0x020 */

typedef union{
  struct {
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  WAKEUPREQ                :  1; /*!< WakeUp Request when LINS send a wakeup signal*/
    uint8_t  RST_INT_ERR              :  2; /*!< Reset Reset interrupt, Error */
    uint8_t  DATAACK_TPYE             :  2; /*!< Data Acknowledgement type : 0x01: received ack, 0x03: Transmit ack*/
    uint8_t  SLEEP                    :  1; /*!< Sleep Request,NOTE: When the chip uses the LINs wakeup logic in pre5v domain(PMU_SFRS->CTRL.PD1V5_ENA_HIBERNATE=0x0), the write operation of this register takes effect by IOCTRLA_SFRS->LIN.UPDATE */
    uint8_t  STOP                     :  1; /*!< Stop Register */
  };
  uint8_t BYTE;
}LINS_CTRL_T2_t;


typedef union {
  struct {
    uint8_t  COMPLETE                 :  1;
    uint8_t  WAKEUP                   :  1;
    uint8_t  ERROR                    :  1; /*!< Lin Error */
    uint8_t  INTR                     :  1; /*!< Interupt Request */
    uint8_t  DATAREQ                  :  1; /*!< Data Request */
    uint8_t  ABORTED                  :  1;
    uint8_t  BUSIDLETIMEOUT           :  1; /*!< BUS Idle Timeout */
    uint8_t  ACTIVE                   :  1; /*!< Lin Bus Active */
  };
  uint8_t BYTE;
} LINS_STATUS_t; /* +0x024 */


typedef union {
  struct {
    uint8_t  BITERR                   :  1; /*!< Bit Error in Byte field */
    uint8_t  CHK                      :  1; /*!< Checksum Error */
    uint8_t  TIMEOUT                  :  1; /*!< Timeout Error */
    uint8_t  PARITY                   :  1; /*!< Parity Error */
    uint8_t  BITMON                   :  1; /*!< Bit Monitor Error */
    uint8_t  SBITERR                  :  1; /*!< Start Bit Error in Byte field */
    uint8_t  FRAMEERR                 :  1; /*!< Byte Field Framing Error */
    uint8_t  BITMONSTOP               :  1; /*!< Bit Monitor Error occurred in Stop Bit */
    uint8_t  BITMONDATA               :  1; /*!< Bit Monitor Error occurred in Start or Data Bits */
    uint8_t                           :  7;
  };
  uint16_t BYTE;
} LINS_ERROR_t; /* +0x028 */

typedef union {
  struct {
    uint8_t  LENGTH                   :  4; /*!< Data Length */
    uint8_t                           :  1; /*   (reserved) */
    uint8_t  DISAUTOSLEEP             :  1; /*!< Disable Auto Sleep */
    uint8_t  DISBITMON                :  1; /*!< Disable Bit Monitor */
    uint8_t  ENHCHK                   :  1; /*!< Enhancement Check */
  };
  uint8_t BYTE;
} LINS_DL_t; /* +0x02C */
  

typedef union {
  struct {
    uint8_t  BTDIV8                   :  1; /*!< Bt Div Most Significant bit */
    uint8_t                           :  5; /*   (reserved) */
    uint8_t  PRESCL                   :  2; /*!< Prescaler Register */
  };
  uint8_t BYTE;
} LINS_BITTIME_t;

typedef union {
  struct {
    uint8_t  WUPREPEAT                :  2; /*!< wakeup repeat time */
    uint8_t  BUSINACTIVE              :  2; /*!< Bus Inactivity Time,               NOTE: When the chip uses the LINs bus idle detection in pre5v domain(PMU_SFRS->CTRL.PD1V5_ENA_HIBERNATE=0x0), the write operation of this register takes effect by IOCTRLA_SFRS->LIN.UPDATE  */
    uint8_t  BUSDOMINANTRELEASEWUPENA :  1; /*!< Bus Dominant Release Wakeup Enable,NOTE: When the chip uses the LINs bus idle detection in pre5v domain(PMU_SFRS->CTRL.PD1V5_ENA_HIBERNATE=0x0), the write operation of this register takes effect by IOCTRLA_SFRS->LIN.UPDATE */
    uint8_t                           :  3; /*   (reserved) */
  };
  uint8_t BYTE;
} LINS_BUSTIME_t; /* +0x03C */


typedef union {
    struct {
      uint8_t  COMPLETETX               :  1; /*!< Complete TX */
      uint8_t  BUSIDLETIMEOUTDOMINANT   :  1; /*!< Dominant Bus Idle Timeout */
      uint8_t  BUSIDLEMONITOR           :  2; /*!< Bus Idle Monitor Status */
      uint8_t                           :  4; /*   (reserved) */
    };
  uint8_t BYTE;
} LINS_TX_STATUS_t; /* +0x040 */

typedef union {
  struct {
    uint8_t  INTERBYTECNT             :  2; /*!< Inter-Byte Space Bit Count */
    uint8_t                           :  6;
    uint8_t  BITMONMODE               :  1; /*!< BIT Monitor Mode */
    uint8_t                           :  7;
  };
  uint16_t HALFWORD;
}LINS_CONF_t; /* +0x048 */

typedef struct {
  LINS_DATABUF_t        DATABUFF[8];             /*<! Data Buffer 1 +0x000 */
  LINS_CTRL_t           CTRL;
  uint8_t               _RESERVED_20[3];
  LINS_STATUS_t         STATUS;                 /* +0x024 */
  uint8_t               _RESERVED_24[3]; 
  LINS_ERROR_t          ERROR;                  /* +0x028 */
  uint8_t               _RESERVED_30[2];
  LINS_DL_t             DL;                     /* +0x02C */
  uint8_t               _RESERVED_2C[3];
  uint8_t               BTDIV07;                /*<! Bt Div LSBs +0x030 */
  uint8_t               _RESERVED_31[3];        /* +0x031 */
  LINS_BITTIME_t        BITTIME;                /* +0x034 */
  uint8_t               _RESERVED_34[3];    
  uint8_t               ID;                     /* +0x038 */
  uint8_t               _RESERVED_39[3];        /* +0x039 */
  LINS_BUSTIME_t        BUSTIME;                /* +0x03C */
  uint8_t               _RESERVED_3C[3];  
  LINS_TX_STATUS_t      TX_STATUS;              /*<! Complete TX +0x040 */
  uint8_t               _RESERVED_40[3];        /* +0x041 */
  uint8_t               WUPDETECTTHRES;         /*<! Wakeup Detection Threshold +0x044 ,NOTE: When the chip uses the LINs bus idle detection in pre5v domain(PMU_SFRS->CTRL.PD1V5_ENA_HIBERNATE=0x0), the write operation of this register takes effect by IOCTRLA_SFRS->LIN.UPDATE */
  uint8_t               _RESERVED_45[3];        /* +0x045 */
  LINS_CONF_t           CONF;
  uint8_t               _RESERVED_46[2];        /* +0x045 */
} LINS_SFRS_t;

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
 * @brief The starting address of LINS SFRS.
 */
#define LINS_REG_DATA_LOW_4BYTES        (*(__IO           uint32_t *)(0x50010700))
#define LINS_REG_DATA_HIGH_4BYTES       (*(__IO           uint32_t *)(0x50010710))
#define LINS_REG_DATA(index)            (*(__IO            uint8_t *)(0x50010700 + index*4))
   
#define LINS_REG_CTRL                   (*(__IO     LINS_CTRL_T2_t *)(0x50010720))
#define LINS_REG_STATUS                 (*(__IO            uint8_t *)(0x50010724))
#define LINS_REG_ERROR                  (*(__IO       LINS_ERROR_t *)(0x50010728))
#define LINS_REG_DL                     (*(__IO            uint8_t *)(0x5001072C))
#define LINS_REG_BTDIV07                (*(__IO            uint8_t *)(0x50010730))
#define LINS_REG_BITTIME                (*(__IO     LINS_BITTIME_t *)(0x50010734))
#define LINS_REG_ID                     (*(__IO            uint8_t *)(0x50010738))
#define LINS_REG_BUSTIME                (*(__IO     LINS_BUSTIME_t *)(0x5001073C))
#define LINS_REG_TX_STATUS              (*(__IO   LINS_TX_STATUS_t *)(0x50010740))
#define LINS_REG_WUPDETECTTHRES         (*(__IO            uint8_t *)(0x50010744))/* NOTE: The write operation of this register takes effect by PMU_SFRS->CTRL.UPDATE */
#define LINS_REG_CONF                   (*(__IO        LINS_CONF_t *)(0x50010748))

#define LINS_SFRS ((__IO LINS_SFRS_t *)0x50010700)

#endif /* end of __LINS_SFR_H__ section */


