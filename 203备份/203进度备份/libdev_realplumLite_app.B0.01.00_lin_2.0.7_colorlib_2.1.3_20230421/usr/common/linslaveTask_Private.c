/**
 * @copyright 2021 Indie microcontroller.
 *
 * This file is proprietary to Indie microcontroller.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie microcontroller.
 *
 * @file linSlaveTask.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2021/07/06
 */

#include <linSlaveTask.h>
#include <linStackTask.h>
#include <applicationTask.h>
#include <linsNodeCfgIdentify.h>
#include <softTimerTask.h>
#include <taskManager.h>
#include <ColorMixingTask.h>
#include <isrfuncs.h>
#include <measureTask.h>
#include <pdsTask.h>
#include <safetyMonitorTask.h>

#if LIN_STACK_TYPE == LIN_STACK_TYPE_PRIVATE

BEGIN_PACK

typedef struct{
    uint8_t  nad;
    uint8_t  pci;
    uint8_t  sid;
    uint8_t command;
    uint32_t magicKey;
}bootloaderHandshake_t;

END_PACK



#define FID_COLOR_CTRL                  DEFAULT_LINS_FID0
#define FID_STATUS_FRAME                DEFAULT_LINS_FID1
#define DIAG_SEND_FRAME_3C              (0x3CU)
#define DIAG_REQ_FRAME_3D               (0x3DU)


#define FID_COLOR_CTRL_INDEX            (0)
#define FID_STATUS_FRAME_INDEX          (1)
#define FID_DIAG_SEND_3C_INDEX          (2)
#define FID_DIAG_REQ_3D_INDEX           (3)

/* ******************internal function declarations****************************/
void SubscribedCmdsHandleISR(LIN_Device_Frame_t const *frame);
void PublishedCmdsISR(LIN_Device_Frame_t *const frame);
void SleepRequestHandle(void);
void DiagCmdsHandle(LIN_Device_Frame_t const *frame);
static LIN_Device_Frame_t receivedFrame;

/* *******************global variants declarations*****************************/
static TaskState_t        linsTaskState = TASK_STATE_INIT;

/*  LIN command handle callbacks declarations  */
static ls_LinsPrivateFramesCallback_t linsFramesCallback ={
  SubscribedCmdsHandleISR,         /* received data from master,it's an interrupt function, please fill the data as fast as possible*/
  PublishedCmdsISR,                /* send data to master, it's an interrupt function, please fill the data as fast as possible */
  SleepRequestHandle,    /* sleep request*/
};

/* **FID table declarations****/
static LIN_Device_Frame_t CmdsTable[DEFAULT_LINS_FID_SIZE] = {
  [FID_COLOR_CTRL_INDEX] = {
      .frame_id = FID_COLOR_CTRL,
      .msg_type = LIN_MSG_TYPE_RX,
      .checksum = LIN_CHECKSUM_ENHANCED,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_STATUS_FRAME_INDEX] = {
      .frame_id = FID_STATUS_FRAME,   /* status management frame */
      .msg_type = LIN_MSG_TYPE_TX,
      .checksum = LIN_CHECKSUM_ENHANCED,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_DIAG_SEND_3C_INDEX] = {
      .frame_id = DIAG_SEND_FRAME_3C,   /* 0x3C */
      .msg_type = LIN_MSG_TYPE_RX,
      .checksum = LIN_CHECKSUM_CLASSIC,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_DIAG_REQ_3D_INDEX] = {
      .frame_id = DIAG_REQ_FRAME_3D,    /* 0x3D */
      .msg_type = LIN_MSG_TYPE_TX,
      .checksum = LIN_CHECKSUM_CLASSIC,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
};

/* 
  Received sleep command from lin master or bus idle timeout occurs
*/
void SleepRequestHandle(void)
{
    /* Set system to hibranate mode*/
    PMU_EnterDeepSleepMode();
}

/*  
  User defined published frame callback function which from LINS ISR
  please fill frame data[0-7] only, please don't do any changes on the left info except data[x]!!!!
*/
void PublishedCmdsISR(LIN_Device_Frame_t *const frame)
{
    /*   feed buffer to frame   */
    if (frame->frame_id == CmdsTable[FID_STATUS_FRAME_INDEX].frame_id){
        /* status management frame */
        uint16_t status = ls_read_sys_status();
        frame->data[0] = (uint8_t)status;
        frame->data[1] = LINS_GetPIDFromFID((uint8_t)(status >> 8));
        frame->data[2] = ls_read_error_code();
        frame->data[3] = MES_DataIntegrityError() | ((uint8_t)SAFM_GetBatteryState() << 2) | ((uint8_t)SAFM_GetChipTemperatureState() << 4);
        /* MUST BE 4 bytes Aligned  */
        uint8_t *ledPNState = (uint8_t *)(frame->data+4U);
        *ledPNState = 0U;
        for (uint8_t i = 0; i < LED_NUM; i++){
            (*ledPNState) |= ((uint8_t)SAFM_GetLedPNStateState((LedNum_t)i) << (i*2U));
        }
    }
}


/* 
  User defined subscribed frame received from LIN master
*/
void SubscribedCmdsHandleISR(LIN_Device_Frame_t const *frame)
{
    receivedFrame = *frame;
    TM_PostTask(TASK_ID_LINS);

}

void DiagCmdsHandle(LIN_Device_Frame_t const *frame)
{
#ifdef BOOTLOADER_EN
    bootloaderHandshake_t *hand = (bootloaderHandshake_t*)frame->data;
    if ( hand->nad == INIT_LINS_NAD_ADDR && hand->pci == 0x06U && hand->sid == LIN_SID_APP_DIAG_BOOT_MODE && hand->command == BOOT_MODE_HANDSHAKE && hand->magicKey == BOOT_MODE_MAGIC_KEY_HANDSHAKE){
        /*  Reset Chip   */
        SYSCTRLA_REG_RETAIN0 = 0x05U;
        PMUA_REG_CTRL.UPDATE = 1U;
        while(PMUA_REG_CTRL.UPDATE == 1U);
        CRGA_REG_RESETCTRL.HARDRSTREQ = 1U;
    }
#endif
  
}

void LINS_TaskHandler(void)
{
    switch(linsTaskState){
    case TASK_STATE_ACTIVE:
      if (receivedFrame.frame_id == CmdsTable[FID_COLOR_CTRL_INDEX].frame_id){
          APPL_HandleColorControlCommands(&receivedFrame);
      }else if (receivedFrame.frame_id == CmdsTable[FID_DIAG_SEND_3C_INDEX].frame_id){
          DiagCmdsHandle(&receivedFrame);
      }else {
      
      }
      break;
    case TASK_STATE_INIT:
      LINS_Init();
      LINS_RegisterPrivateServices(CmdsTable, (l_u8)(sizeof(CmdsTable)/sizeof(LIN_Device_Frame_t)), &linsFramesCallback);
      /* Note that please don't change the following glitch configuration!!!!*/
      (void)ls_set_lins_rx_glitch_filter_1st(0x00U,0x08U);
      (void)ls_set_lins_rx_glitch_filter_2nd(0x0AU,0x10U);
      (void)ls_set_lins_rx_glitch_filter_3rd(0x30U,0x30U);
      linsTaskState = TASK_STATE_ACTIVE;
      break;
    default:
      break;
    }
}


uint8_t LINS_GetPIDFromFID(uint8_t frameId)
{
    uint8_t id = frameId;
    uint8_t P0,P1;
    P0 = (uint8_t)(((id >> 0)&0x01U)^((id >> 1)&0x01U)^((id >> 2)&0x01U) ^ ((id>> 4)&0x01U));
    P1 = (uint8_t)(~(((id >> 1)&0x01U)^((id >> 3)&0x01U)^((id >> 4)&0x01U) ^ ((id>> 5)&0x01U))) & 0x01U;
    id = frameId | (P0 << 6) | (P1 << 7);
    return id;
}


/* please don't do any changes for it would be used by lin stack */
LIN_Device_Frame_t *LINS_GetUcndFramesTable(uint8_t *tableItemsCount)
{
    *tableItemsCount = (uint8_t)(sizeof(CmdsTable)/sizeof(LIN_Device_Frame_t));
    return CmdsTable;
}


static LINClusterParams_t *linParams  = NULL;
static LIN_Device_Frame_t *frameTable = NULL;

void LNCI_Init(void)
{
    uint8_t tableItemsCount;
    /* get lin information from flash storage */
    linParams = PDS_GetLINParam();
    /*  get frame table information   */
    frameTable = LINS_GetUcndFramesTable(&tableItemsCount);
    
    for (uint8_t i = 0U; i < tableItemsCount; i++){
        frameTable[i].frame_id     = linParams->frameInfo[i].frameId;
        frameTable[i].frameIsValid = linParams->frameInfo[i].frameIsValid;
    }
}

#endif