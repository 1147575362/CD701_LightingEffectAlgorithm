/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file lin_device.h
 */

#ifndef LIN_DEVICE_H__
#define LIN_DEVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include <realplumLite.h>
#include "lins_sfr.h"

#define E_LIN_STATUS_COMPLETE       (1U << 0)
#define E_LIN_STATUS_WAKEUP         (1U << 1)
#define E_LIN_STATUS_ERROR          (1U << 2)
#define E_LIN_STATUS_INT_REQ        (1U << 3)
#define E_LIN_STATUS_DATA_REQ       (1U << 4)
#define E_LIN_STATUS_ABORT          (1U << 5)
#define E_LIN_STATUS_IDLE_TIMEOUT   (1U << 6)
#define E_LIN_STATUS_ACTIVE         (1U << 7)

#define E_LIN_ERROR_BIT             (1U << 0)
#define E_LIN_ERROR_CHECKSUM        (1U << 1)
#define E_LIN_ERROR_TIMEOUT         (1U << 2)
#define E_LIN_ERROR_PARITY          (1U << 3)


#define E_LIN_TIME_INACTIVE_SEC_4       (0U)
#define E_LIN_TIME_INACTIVE_SEC_6       (1U)
#define E_LIN_TIME_INACTIVE_SEC_8       (2U)
#define E_LIN_TIME_INACTIVE_SEC_10      (3U)
#define E_LIN_TIME_INACTIVE_MAX         (3U) /*E_LIN_TIME_INACTIVE_SEC_10*/

#define E_LIN_TIME_WAKEUP_REPEAT_MS_180 (0U)
#define E_LIN_TIME_WAKEUP_REPEAT_MS_200 (1U)
#define E_LIN_TIME_WAKEUP_REPEAT_MS_220 (2U)
#define E_LIN_TIME_WAKEUP_REPEAT_MS_240 (3U)
#define E_LIN_TIME_WAKEUP_REPEAT_MAX    (3U)/*E_LIN_TIME_WAKEUP_REPEAT_MS_240*/


#define LIN_RX_FILTER_2_3_us            (0U)/* LINS_RX_BIAS_BOOST */
#define LIN_RX_FILTER_1_3_us            (1U)
#define LIN_RX_FILTER_1_0_us            (2U)
#define LIN_RX_FILTER_0_8_us            (3U) 
   
#define LIN_TX_DRIVE_CURRENT_32_mA      (0U)/* pull down capacity */
#define LIN_TX_DRIVE_CURRENT_48_mA      (1U)
#define LIN_TX_DRIVE_CURRENT_66_mA      (2U)
#define LIN_TX_DRIVE_CURRENT_88_mA      (3U)
#define LIN_TX_DRIVE_CURRENT_101_mA     (4U)
#define LIN_TX_DRIVE_CURRENT_118_mA     (5U)
#define LIN_TX_DRIVE_CURRENT_135_mA     (6U)
#define LIN_TX_DRIVE_CURRENT_151_mA     (7U) 
   
#define LIN_TX_SLEW_RATE_0_6V_PER_US    (1U)/* Slew from 40%*Vbat to 60%*Vbat @Vbat=13V,Cbus=1nF */
#define LIN_TX_SLEW_RATE_1_2V_PER_US    (2U)
#define LIN_TX_SLEW_RATE_1_7V_PER_US    (3U)
#define LIN_TX_SLEW_RATE_2_3V_PER_US    (4U)
#define LIN_TX_SLEW_RATE_2_8V_PER_US    (5U)
#define LIN_TX_SLEW_RATE_3_4V_PER_US    (6U)
#define LIN_TX_SLEW_RATE_5_1V_PER_US    (7U)


#define LIN_BUFF_SIZE                  8U

/* Defines for LIN device to handle message direction.  */
typedef enum{
    LIN_MSG_TYPE_RX = 0U,  /*received data from sender for slave; */
    LIN_MSG_TYPE_TX,       /*data request from sender for slave; */
    LIN_MSG_TYPE_TX_EVENT, /*event trigger data request from sender for slave; */
}LinMessageType_t;


typedef enum{
    E_LIN_BAUDRATE_10417,
    E_LIN_BAUDRATE_19200,
    E_LIN_BAUDRATE_20000,
}LIN_BaudRate_t;

#define E_LIN_BAUDRATE_MAX   (3U)/*E_LIN_BAUDRATE_20000*/


typedef enum{
    LIN_CHECKSUM_CLASSIC = 0U,  /*!< classic checksum does not include ID Byte.  */ 
    LIN_CHECKSUM_ENHANCED       /*!< "enhanced" checksum includes ID Byte.  */ 
}ChecksumType_t;


typedef enum{
  AA_SWITCH_OFF = 0U,
  AA_SWITCH_ON,
}AutoaddressingSwitchState_t;

/**
* @brief A structure to represent LIN frame information.
*/
typedef struct { 
    uint8_t             frame_id;                  /*!< frame ID.  */ 
    LinMessageType_t    msg_type;                  /*!< Type of message, rx,tx or event trigger.  */ 
    uint8_t             checksum;                  /*!< Type of checksum (enhanced or classic).  */ 
    uint8_t             length;                    /*!< Number of bytes in the data. */ 
    union{
        uint8_t         data[LIN_BUFF_SIZE];       /*!< data buffer. */
        uint16_t        data16[LIN_BUFF_SIZE/2];
        uint32_t        data32[LIN_BUFF_SIZE/4];
    };
    uint8_t             responseValid;
    uint8_t             frameIsValid;
    uint8_t             eventTriggered;                  /*!< event triggered when the msg_type is LIN_MSG_TYPE_TX_EVENT */
    uint8_t             linkedEventTriggerFidTableIndex; /*!< event triggered when the msg_type is LIN_MSG_TYPE_TX_EVENT */
} LIN_Device_Frame_t;


typedef void (*lin_cb_func_t)(LIN_Device_Frame_t const *frame);
typedef void (*lin_stack_cb_func_t)(void);
typedef void (*lin_wakeup_cb_func_t)(void);

void LINS_Init(void);
void LINS_RegisterLINStack_IRQ(lin_stack_cb_func_t callback,lin_wakeup_cb_func_t wakeupCallback);

#if LIN_STACK_TYPE == LIN_STACK_TYPE_PRIVATE

typedef void (*ls_DataReqCallback_t)(LIN_Device_Frame_t const *frame);
typedef void (*ls_DataRspCallback_t)(LIN_Device_Frame_t *const frame);
typedef void (*ls_SleepReqCallback_t)(void);

typedef struct{
  ls_DataReqCallback_t         dataReqCallback;   /* received data from master */
  ls_DataRspCallback_t         dataRspCallback;   /* send data to master */
  ls_SleepReqCallback_t        sleepReqCallback;
}ls_LinsPrivateFramesCallback_t;

void LINS_RegisterPrivateServices(LIN_Device_Frame_t *frameTable, uint8_t tableSize, const ls_LinsPrivateFramesCallback_t *pFunCallback);

#endif


#endif /* __LIN_DEVICE_H__ */
