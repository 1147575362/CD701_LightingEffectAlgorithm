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

#if LIN_STACK_TYPE == LIN_STACK_TYPE_SAEJ2602


BEGIN_PACK

typedef struct{
  uint8_t       reserved;
  uint32_t      calibPartNumber;
  uint8_t       dateLength;
  uint8_t       smallCalibCfgIndex;
}SmallCalibCfgParam_t;

END_PACK

static SmallCalibCfgParam_t smallCalibCfg = {0U,0x00000000U,16U,0U};

#define SLAVE_PN_REPORT_STRING_NUM     (15U)
#define SLAVE_PN_REPORT_PACK_NUM       (SLAVE_PN_REPORT_STRING_NUM/6U + 1U)
static const uint8_t slavePartNumberReporting[SLAVE_PN_REPORT_STRING_NUM] = {'I','N','D','8','3','2','0','9',' ','F','W',':','2','.','0'};
static uint8_t slavePartNumberReportingIndex = 0U;

#define LARGE_CALIB_RECV_NUM    (14U)
static uint8_t largeCalibDataRecvBuff[LARGE_CALIB_RECV_NUM];
static uint8_t largeCalibIsRunning = FALSE;

#define FID_LED_CTRL            DEFAULT_LINS_FID0
#define FID_LED_REPORT          DEFAULT_LINS_FID1
#define FID_SMALL_CAL           DEFAULT_LINS_FID2
#define FID_CAL_PN_REPORT       DEFAULT_LINS_FID3
#define FID_LARGE_CAL           DEFAULT_LINS_FID4
#define FID_SLAVE_PN_REPORT     DEFAULT_LINS_FID5

#define FID_RESVD0              DEFAULT_LINS_FID6
#define FID_RESVD1              DEFAULT_LINS_FID7

#define DIAG_DATA_BUFF_SIZE     (128U)

#define FID_LED_CTRL_INDEX              (0)
#define FID_LED_REPORT_INDEX            (1)
#define FID_SMALL_CAL_INDEX             (2)
#define FID_CAL_PN_REPORT_INDEX         (3)
#define FID_LARGE_CAL_INDEX             (4)
#define FID_SLAVE_PN_REPORT_INDEX       (5)

/* ******************internal function declarations****************************/
void DiagnosticSubscribedCmdsHandle(const DiagReqInfo_t * const frameInfo);
void UnconditionalSubscribedCmdsHandle(LIN_Device_Frame_t const *frame);
void PublishedCmdsTxFinishedISR(uint8_t frameID,uint8_t j2602CfgRsp);
void UnconditionalPublishedCmdsISR(LIN_Device_Frame_t *const frame);
void DiagnosticSleepRequestHandle(SleepRequestType_t type);
void BusWakeUpRequestHandle(BusWakeUpRequestResult_t result);
void SAEJ2602CfgSubscribedCmdsHandle(uint8_t fid,uint8_t const *payload, uint8_t length);
void SAEJ2602DNNBroadcastSubscribedCmdsHandle(uint8_t fid,uint8_t const *payload, uint8_t length);

/* *******************global variants declarations*****************************/
static TaskState_t linsTaskState = TASK_STATE_INIT;
static uint8_t diagDataRspBuff[DIAG_DATA_BUFF_SIZE];
static uint8_t diagDataRecvBuff[DIAG_DATA_BUFF_SIZE];
static uint8_t busWakeupRetryCount = 0U;

static J2602StatusByte_t j2602StatusByte = {
    .appInfo0   = 0U,
    .appInfo1   = 0U,
    .appInfo2   = 0U,
    .appInfo3   = 1U,
    .appInfo4   = 1U,
    .err        = 0U,

};

void busWakeupRetryTimerExpired(SoftTimer_t *timer);

static SoftTimer_t busWakeupRetryTimer = {
    .mode     = TIMER_ONE_SHOT_MODE,
    .interval = 2250U,
    .handler  = busWakeupRetryTimerExpired
};


/*  LIN command handle callbacks declarations  */
static ls_LinsFramesCallback_t linsFramesCallback ={
  UnconditionalSubscribedCmdsHandle,            /* received data from master */
  UnconditionalPublishedCmdsISR,                /* send data to master, it's an interrupt function, please fill the data as fast as possible */
  PublishedCmdsTxFinishedISR,                   /* send data to master finished, it's an interrupt function, please fill the data as fast as possible  */
  DiagnosticSubscribedCmdsHandle,               /* Diagnostic sigle PDU and MultiPDU received data from master */
  NULL,                                         /* special functional NAD (0x7E) handler*/
  DiagnosticSleepRequestHandle,                 /* Diagnostic sleep request from master */
  BusWakeUpRequestHandle,                       /* Get the bus wake up result this would take effects after call API: ls_send_wake_up_bus_signal()*/
  SAEJ2602CfgSubscribedCmdsHandle,              /* SAE J2602 -1 Configuration Messages */
  SAEJ2602DNNBroadcastSubscribedCmdsHandle,     /*  5.7.2.5 DNN Based Broadcast Messages */
};

/* **FID table declarations****/
static LIN_Device_Frame_t UnconditionalCmdsTable[] = {
  [FID_LED_CTRL_INDEX] = {              /* frame_id: LED Control  */                  
      .frame_id = FID_LED_CTRL,
      .msg_type = LIN_MSG_TYPE_RX,
      .checksum = LIN_CHECKSUM_ENHANCED,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_LED_REPORT_INDEX] = {
      .frame_id = FID_LED_REPORT,       /* frame_id: LED state report  */
      .msg_type = LIN_MSG_TYPE_TX,
      .checksum = LIN_CHECKSUM_ENHANCED,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_SMALL_CAL_INDEX] = {
      .frame_id = FID_SMALL_CAL,        /* frame_id: Small Calibration/Configuration  */
      .msg_type = LIN_MSG_TYPE_RX,
      .checksum = LIN_CHECKSUM_ENHANCED,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_CAL_PN_REPORT_INDEX] = {
      .frame_id = FID_CAL_PN_REPORT,    /* frame_id: Slave Application Calibration Part Number */
      .msg_type = LIN_MSG_TYPE_TX,
      .checksum = LIN_CHECKSUM_ENHANCED,       
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_LARGE_CAL_INDEX] = {
      .frame_id = FID_LARGE_CAL,        /* frame_id: Large Calibration/Configuration   */
      .msg_type = LIN_MSG_TYPE_RX,
      .checksum = LIN_CHECKSUM_ENHANCED,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
  [FID_SLAVE_PN_REPORT_INDEX] = {
      .frame_id = FID_SLAVE_PN_REPORT,  /* frame_id:  Slave Part Number Reporting */
      .msg_type = LIN_MSG_TYPE_TX,
      .checksum = LIN_CHECKSUM_ENHANCED,       
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
  },
};


/* please don't do any changes for it would be used by lin stack */
LIN_Device_Frame_t *LINS_GetUcndFramesTable(uint8_t *tableItemsCount)
{
    *tableItemsCount = (uint8_t)(sizeof(UnconditionalCmdsTable)/sizeof(LIN_Device_Frame_t));
    return UnconditionalCmdsTable;
}

/* 
  Diagnostic subscribed frame received from LIN master
*/
void DiagnosticSubscribedCmdsHandle(const DiagReqInfo_t *const diagReq)
{
    DiagRspInfo_t diagRsp={
      .sid      = diagReq->sid,
      .type     = diagReq->type,
      .payload  = diagDataRspBuff,
    };
    if (diagReq->type == PDU_TYPE_SINGLE_PDU){
        if (diagReq->sid >= LIN_SID_ASSIGN_NAD && diagReq->sid <= LIN_SID_ASSIGN_FRAME_ID_RANGE){
            /* handle Node configuration and Identification commands*/
            diagRsp.type = PDU_TYPE_SINGLE_PDU;
            if (LNCI_HandleNodeCfgIdentifyRequest(diagReq,&diagRsp)== TRUE){
                ls_handle_diagnostic_response(&diagRsp);
            }
        }else{
            if (diagReq->sid == LIN_SID_APP_DIAG_LED_GET_CONFIG){
                /*  prepare diagnostic single pdu, user defined diagnostic frame, here is for LED param configuration*/
                for (uint16_t i = 0; i < diagReq->frameDataLength; i++){
                    diagDataRecvBuff[i+diagReq->packCurrIndex] = diagReq->payload[i];
                }
                if(APPL_PrepareLedParamRequest(diagReq->sid,diagDataRecvBuff,&diagRsp) == TRUE){
                    ls_handle_diagnostic_response(&diagRsp);
                }
            }else if (diagReq->sid == LIN_SID_APP_DIAG_SINGLE_PDU_RECV_MULTI_RSP){
                /*  handle diagnostic single pdu, multi response, user defined diagnostic frame here for example: */
                for (uint16_t i = 0U; i < 15U; i++){
                    diagDataRspBuff[i] = (uint8_t)i;
                }
                diagRsp.sid = diagReq->sid;
                diagRsp.packLength = 15U;
                diagRsp.type = (diagRsp.packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
                ls_handle_diagnostic_response(&diagRsp); 
            }
#ifdef BOOTLOADER_EN
            else if (diagReq->sid == LIN_SID_APP_DIAG_BOOT_MODE && diagReq->packLength == sizeof(EnterBootloaderFrame_t)){
                  EnterBootloaderFrame_t *boot = (EnterBootloaderFrame_t *)((void*)diagReq->payload);
                  if ( boot->command == BOOT_MODE_HANDSHAKE && boot->magicKey == BOOT_MODE_MAGIC_KEY_HANDSHAKE){
                      /*  Reset Chip   */
                      CRGA_REG_RESETCTRL.HARDRSTREQ = 1U;
                  }
            }
#endif
        }
    }else{
        /*  handle diagnostic multi pdu received sigle pdu response, user defined diagnostic frame here   */
        if (diagReq->sid == LIN_SID_APP_DIAG_LED_SET_CONFIG){
            for (uint16_t i = 0; i < diagReq->frameDataLength; i++){
                diagDataRecvBuff[i+diagReq->packCurrIndex] = diagReq->payload[i];
            }
            /* Package received finished */
            if ((diagReq->frameDataLength + diagReq->packCurrIndex)>=diagReq->packLength){
                /* handle finished data here  */
                if ( APPL_HandleLedParamConfigRequest(diagReq->sid,diagDataRecvBuff,&diagRsp) == TRUE){
                    ls_handle_diagnostic_response(&diagRsp);  
                }
            }
        }else if (diagReq->sid == LIN_SID_APP_DIAG_LED_GET_CONFIG){
            /*  handle diagnostic multi pdu received multi pdu response, user defined diagnostic frame here   */
            for (uint16_t i = 0; i < diagReq->frameDataLength; i++){
                diagDataRecvBuff[i+diagReq->packCurrIndex] = diagReq->payload[i];
            }
            if ((diagReq->frameDataLength + diagReq->packCurrIndex)>=diagReq->packLength){
                /*  prepare diagnostic single pdu, user defined diagnostic frame, here is for LED param configuration*/
                if(APPL_PrepareLedParamRequest(diagReq->sid,diagDataRecvBuff,&diagRsp) == TRUE){
                    ls_handle_diagnostic_response(&diagRsp);
                }
            }
        
        }else if (diagReq->sid == LIN_SID_APP_DIAG_MULTI_PDU_RECV_SINGLE_RSP){
            /*  handle diagnostic multi pdu received single pdu response, user defined diagnostic frame here   */
            for (uint16_t i = 0; i < diagReq->frameDataLength; i++){
                diagDataRecvBuff[i+diagReq->packCurrIndex] = diagReq->payload[i];
            }
            /* Package received finished */
            if ((diagReq->frameDataLength + diagReq->packCurrIndex)>=diagReq->packLength){
                /* handle finished data here  */
                for (uint16_t i = 0U; i < 6U; i++){
                    diagDataRspBuff[i] = (uint8_t)i;
                }
                /* handle response data here  */
                diagRsp.sid = diagReq->sid;
                diagRsp.packLength = 5U;/* Valid data length not includes sid*/
                diagRsp.type = (diagRsp.packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
                ls_handle_diagnostic_response(&diagRsp);  
            }
        }else if (diagReq->sid == LIN_SID_APP_DIAG_MULTI_PDU_RECV_MULTI_RSP){
            /*  handle diagnostic multi pdu received multi pdu response, user defined diagnostic frame here   */
            for (uint16_t i = 0; i < diagReq->frameDataLength; i++){
                diagDataRecvBuff[i+diagReq->packCurrIndex] = diagReq->payload[i];
            }
            /* Package received finished */
            if ((diagReq->frameDataLength + diagReq->packCurrIndex)>=diagReq->packLength){
                /* handle finished data here  */
                /* handle response data here  */
                for (uint16_t i = 0U; i < 15U; i++){
                    diagDataRspBuff[i] = (uint8_t)(DIAG_DATA_BUFF_SIZE - i);
                }
                diagRsp.sid = diagReq->sid;
                diagRsp.packLength = 15U; /* Valid data length not includes sid*/
                diagRsp.type = (diagRsp.packLength > SF_MAX_DATA_LENGTH)? PDU_TYPE_MULTI_PDU:PDU_TYPE_SINGLE_PDU;
                ls_handle_diagnostic_response(&diagRsp);  
            }
        }else{
            
        }
    }
}

/* 
  Received sleep command from lin master or bus idle timeout occurs
*/
void DiagnosticSleepRequestHandle(SleepRequestType_t type)
{
    /* Set system to hibranate mode     */
    PMU_EnterDeepSleepMode();
}


void PublishedCmdsTxFinishedISR(uint8_t frameID,uint8_t j2602CfgRsp)
{
    if ( (frameID != LIN_FRAME_ID_SLAVE_RSP) || (frameID == LIN_FRAME_ID_SLAVE_RSP && j2602CfgRsp == TRUE)){
        ls_reset_J2602_error_code((J2602ErrCode_t)j2602StatusByte.err);     /* clear error when response successfully  */
    }
}

void LINS_SetDeviceUnconfigured(void)
{
    j2602StatusByte.appInfo4 = 1U;
    j2602StatusByte.appInfo3 = 1U;
    smallCalibCfg.calibPartNumber = 0U;
}

/*  
  User defined published frame callback function which from LINS ISR
  please fill frame data[0-7] only, please don't do any changes on the left info except data[x]!!!!
*/
void UnconditionalPublishedCmdsISR(LIN_Device_Frame_t *const frame)
{
    j2602StatusByte.err = (uint8_t)ls_read_J2602_error_code();          /* read error but not clear automatically */
    /*   feed buffer to frame   */
    if (frame->frame_id == UnconditionalCmdsTable[FID_LED_REPORT_INDEX].frame_id){
        /* LED status report */
        frame->data[0] = j2602StatusByte.payload;
        frame->data[1] = APPL_GetLEDState();
        frame->data[2] = 0xFFU;
        frame->data[3] = MES_DataIntegrityError() | ((uint8_t)SAFM_GetBatteryState() << 2) | ((uint8_t)SAFM_GetChipTemperatureState() << 4);
        /* MUST BE 4 bytes Aligned  */
        uint8_t *ledPNState = (uint8_t *)(frame->data+4U);
        *ledPNState = 0U;
        for (uint8_t i = 0; i < LED_NUM; i++){
            (*ledPNState) |= ((uint8_t)SAFM_GetLedPNStateState((LedNum_t)i) << (i*2U));
        }
    }else if (frame->frame_id == UnconditionalCmdsTable[FID_CAL_PN_REPORT_INDEX].frame_id){
        /* Calibration Part Number report*/
        frame->data[0] = j2602StatusByte.payload; 
        ((SmallCalibCfgParam_t *)(void*)frame->data)->calibPartNumber = smallCalibCfg.calibPartNumber;
        for (uint8_t i = 5U; i < LIN_BUFF_SIZE; i++){
            frame->data[i] = 0xFFU;
        }
        smallCalibCfg.smallCalibCfgIndex = 0U;
    }else if (frame->frame_id == UnconditionalCmdsTable[FID_SLAVE_PN_REPORT_INDEX].frame_id){
        j2602StatusByte.appInfo4 = 1U; /* I'm not configured  */
        frame->data[0] = j2602StatusByte.payload;
        frame->data[1] = slavePartNumberReportingIndex;
        for (uint8_t i = 0U; i < LIN_BUFF_SIZE -2U; i++){
            if ( (slavePartNumberReportingIndex*6U + i)>= SLAVE_PN_REPORT_STRING_NUM){
                frame->data[i+2U] = 0x00U;
            }else{
                frame->data[i+2U] = slavePartNumberReporting[slavePartNumberReportingIndex*6U + i];
            }
        }
        if (++slavePartNumberReportingIndex >= SLAVE_PN_REPORT_PACK_NUM){
            slavePartNumberReportingIndex = 0U;
        }
    }else{
        /* intent to empty   */
    }
}


void SAEJ2602CfgSubscribedCmdsHandle(uint8_t fid,uint8_t const *payload, uint8_t length)
{
    uint8_t databuff[7]={0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU,0xFFU};
    SaeJ2602_3C3ECfg_t const *cfg = (SaeJ2602_3C3ECfg_t const*)((void const*)payload);
    j2602StatusByte.err = (uint8_t)ls_read_J2602_error_code();
    if (fid == 0x3CU){/*  0x3C */
        if ( cfg->magicKey == 0x55555555U){
            databuff[0] = (cfg->nad & 0x70U) >> 4U;
            ls_handle_SAEJ2602Cfg_response(j2602StatusByte.payload, databuff);
        }
    }else{/*  0x3E */
        if ( cfg->magicKey == 0xAAAAAAAAU){
            databuff[0] = (cfg->nad & 0xF0U) >> 4U;
            ls_handle_SAEJ2602Cfg_response(j2602StatusByte.payload, databuff);
        }
    }
}

/* SAEJ2602 -1 :5.7.2.5 DNN Based Broadcast Messages: 0x38,0x39,0x3A,0x3B based on MESSAGE_ID_BLOCK_COUNT and NAD*/
void SAEJ2602DNNBroadcastSubscribedCmdsHandle(uint8_t fid,uint8_t const *payload, uint8_t length)
{
#if MESSAGE_ID_BLOCK_COUNT == 4U || MESSAGE_ID_BLOCK_COUNT == 8U || MESSAGE_ID_BLOCK_COUNT == 16U
    uint32_t broadcaseContent = 0U;
    uint8_t dnn = (ls_read_nad()&0x0FU);
  
    if (fid == 0x38U || fid == 0x3AU){
        for (uint8_t i = 0U; i < (MESSAGE_ID_BLOCK_COUNT/4U); i++){
            broadcaseContent += (uint32_t)payload[dnn+i] << (8U*i);
        }
    }else{
        for (uint8_t i = 0U; i < (MESSAGE_ID_BLOCK_COUNT/4U); i++){
            broadcaseContent += (uint32_t)payload[dnn + i - 8U] << (8U*i);
        }
    }
#else
    #error invalid MESSAGE_ID_BLOCK_COUNT !!
#endif

}

/* 
  User defined subscribed frame received from LIN master
*/
void UnconditionalSubscribedCmdsHandle(LIN_Device_Frame_t const *frame)
{
    if (frame->frame_id == UnconditionalCmdsTable[FID_LED_CTRL_INDEX].frame_id){
        APPL_HandleColorControlCommands(frame);
    }else if (frame->frame_id == UnconditionalCmdsTable[FID_SMALL_CAL_INDEX].frame_id){
        smallCalibCfg.calibPartNumber = 0U;
        j2602StatusByte.appInfo3 = 1U;
        for (uint8_t i = 0U; i < LIN_BUFF_SIZE; i++){
            diagDataRecvBuff[smallCalibCfg.smallCalibCfgIndex*LIN_BUFF_SIZE + i] = frame->data[i];
        }
        smallCalibCfg.smallCalibCfgIndex++;
        if ( (smallCalibCfg.smallCalibCfgIndex*LIN_BUFF_SIZE) >= smallCalibCfg.dateLength ){
            /* do checksum calculating   */
            uint8_t checksum = 0U;
            for (uint8_t i = 0U; i < smallCalibCfg.dateLength; i++){
                checksum += diagDataRecvBuff[i];
            }
            if (checksum == 0xFFU){
                smallCalibCfg.calibPartNumber = ((SmallCalibCfgParam_t*)(void*)diagDataRecvBuff)->calibPartNumber;
                j2602StatusByte.appInfo3 = 0U;
            }
            smallCalibCfg.smallCalibCfgIndex = 0U;
        }
    }else if (frame->frame_id == UnconditionalCmdsTable[FID_LARGE_CAL_INDEX].frame_id){
        
        if (largeCalibIsRunning == TRUE && frame->data[0] == 0U){       /* Calibration/Configuration Mode has finished */
            largeCalibIsRunning = FALSE;
            j2602StatusByte.appInfo4 = 0U;
        }else if (largeCalibIsRunning == FALSE && frame->data[0] == 0U){/* Calibration/Configuration Mode has started */
            largeCalibIsRunning = TRUE;
            j2602StatusByte.appInfo4 = 1U;
            for (uint8_t i = 0; i < 7U; i++){
                largeCalibDataRecvBuff[i] = frame->data[1U+ i];
            }
        }else{
            for (uint8_t i = 0; i < 7U; i++){
                if ((frame->data[0]*7U+i) < LARGE_CALIB_RECV_NUM){
                    largeCalibDataRecvBuff[frame->data[0]*7U+i] = frame->data[1U+ i];
                }
            }
        }
    }
}


void busWakeupRetryTimerExpired(SoftTimer_t *timer)
{
    ls_send_wake_up_bus_signal();
}

/* Called timeout after bus wake up 3 tries, or called when bus recovery */
void BusWakeUpRequestHandle(BusWakeUpRequestResult_t result)
{
    if (result == BUS_WAKEUP_REQ_RESULT_REQ_TIMEOUT){
        if (busWakeupRetryCount == 1U){
            busWakeupRetryCount++;
            SoftTimer_Start(&busWakeupRetryTimer);
        }else{
            busWakeupRetryCount = 0U;
        }
    }else{
        busWakeupRetryCount = 0U;
        SoftTimer_Stop(&busWakeupRetryTimer);
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


void LINS_SendWakeUpSignal_ISR(void)
{
    if (busWakeupRetryCount == 0U){
        busWakeupRetryCount++;
        ls_send_wake_up_bus_signal();
    }
}

void LINS_TaskHandler(void)
{
    switch(linsTaskState){
    case TASK_STATE_ACTIVE:
      break;
    case TASK_STATE_INIT:
      ls_register_services(LIN_PROTOCOL_SAE_J2602,UnconditionalCmdsTable, (l_u8)(sizeof(UnconditionalCmdsTable)/sizeof(LIN_Device_Frame_t)), DIAG_DATA_BUFF_SIZE,&linsFramesCallback);
      (void)ls_set_tp_timeout(N_AS, N_CR);
      /* Note that please don't change the following glitch configuration!!!!*/
      (void)ls_set_lins_rx_glitch_filter_1st(0x00U,0x08U);
      (void)ls_set_lins_rx_glitch_filter_2nd(0x0AU,0x10U);
      (void)ls_set_lins_rx_glitch_filter_3rd(0x30U,0x30U);
      (void)l_sys_init();
      (void)ls_disable_lin_auto_sleep(TRUE);
      ls_set_J2602_hardware_reset(TRUE);/*  It's a hardreset init  */
      if(GPIO_SFRS->GPIO_CFG[(uint8_t)GPIO_PORT_2].ACTDET != 0U){
          GPIO_SFRS->GPIO_CFG[(uint8_t)GPIO_PORT_2].CLR = 1U;
          LINS_SendWakeUpSignal_ISR();
      }
      GPIO_RegisterIRQ(GPIO_PORT_2, GPIO_EDGE_FALLING, LINS_SendWakeUpSignal_ISR);
      linsTaskState = TASK_STATE_ACTIVE;
      break;
    default:
      break;
    }
}

#endif