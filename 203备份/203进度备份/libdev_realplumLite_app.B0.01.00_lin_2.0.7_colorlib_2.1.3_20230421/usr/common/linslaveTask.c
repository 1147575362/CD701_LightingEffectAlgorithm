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

#if LIN_STACK_TYPE == LIN_STACK_TYPE_LIN2_2A

#define FID_COLOR_CTRL                  DEFAULT_LINS_FID0	//0x20
#define FID_STATUS_FRAME                DEFAULT_LINS_FID1	//0x21
#define USER_DATA_REPORT                DEFAULT_LINS_FID2	//0x22
#define EVENT_TRIGGER_DATA_REPORT       DEFAULT_LINS_FID3	//0x23

#define YZ_CHANGE_FID_CTRL				(0x3FU)

#define LEFT_CONFIG						DEFAULT_LINS_LEFT	//0x03
#define RIGHT_CONFIG					DEFAULT_LINS_RIGHT	//0x04

#define DIAG_DATA_BUFF_SIZE     (128U)

#define FID_TABLE_INDEX_0       (0)
#define FID_TABLE_INDEX_1       (1)
#define FID_TABLE_INDEX_2       (2)
#define FID_TABLE_INDEX_3       (3)

#define FID_COLOR_CTRL_INDEX        (0)
#define FID_STATUS_FRAME_INDEX      (1)
#define USER_DATA_REPORT_INDEX      (2)
#define EVENT_TRIGGERED_INDEX       (3)
#define YZ_CHANGE_FID_INDEX         (4)
#define YZ_RESPOND_FID_INDEX		(5)
#define FID_DOOR_CTRL_INDEX     	(6)


#define PROJECT_NAME			0x66

#define COLOR_TABLE_SIZE  64						//copy
static  RGB_xy_t  rgbTable[COLOR_TABLE_SIZE] = {	
    {0xFF0000, 0.1621, 0.1036},   //1
    {0x00FF00, 0.1632, 0.1033},   //2
    {0x4152FF, 0.1738, 0.1075},   //3
    {0x5A51FF, 0.1844, 0.1118},   //4
    {0x6E50FF, 0.1955, 0.1163},   //5
    {0x7F4EFF, 0.2064, 0.1201},   //6
    {0x8C4DFF, 0.21579, 0.12396}, //7
    {0x9D4CFF, 0.2293, 0.1298},   //8
    {0xAA4AFF, 0.2402, 0.1338},   //9
    {0xB848FF, 0.2525, 0.1387},   //10
    {0xC546FF, 0.2645, 0.1435},   //11
    {0xD144FF, 0.2758, 0.148},    //12
    {0xD940FF, 0.2834, 0.14984},  //13
    {0xE33FFF, 0.2931, 0.15427},  //14
    {0xED3EFF, 0.3028, 0.1587},   //15
    {0xF23AF9, 0.31341, 0.16313}, //16
    {0xF936F7, 0.32241, 0.16634}, //17
    {0xFF33F4, 0.3315, 0.1702},   //18
    {0xFF2DEB, 0.34132, 0.17396}, //19
    {0xFF28E2, 0.35168, 0.17843}, //20
    {0xFF23DB, 0.3602, 0.1818},   //21
    {0xFF1ED2, 0.37165, 0.18706}, //22
    {0xFF14CB, 0.3812, 0.19018},  //23
    {0xFF0BC4, 0.391, 0.1942},    //24
    {0xFF05BC, 0.4026, 0.19985},  //25
    {0xFF00B4, 0.41459, 0.2058},  //26
    {0xFF00AC, 0.42683, 0.21254}, //27
    {0xFF00A4, 0.43944, 0.21949}, //28
    {0xFF009C, 0.4524, 0.2266},   //29
    {0xFF0096, 0.46225, 0.23205}, //30
    {0xFF0090, 0.47225, 0.23757}, //31
    {0xFF008A, 0.4823, 0.2431},   //32
    {0xFF0084, 0.49249, 0.24872}, //33
    {0xFF007E, 0.50266, 0.25432}, //34
    {0xFF0077, 0.5145, 0.2608},   //35
    {0xFF0070, 0.52622, 0.2673},  //36
    {0xFF0069, 0.53776, 0.27366}, //37
    {0xFF0063, 0.5475, 0.279},    //38
    {0xFF005C, 0.55841, 0.28504}, //39
    {0xFF0055, 0.56896, 0.29086}, //40
    {0xFF004D, 0.5804, 0.2971},   //41
    {0xFF0044, 0.59228, 0.30371}, //42
    {0xFF003B, 0.60302, 0.30962}, //43
    {0xFF0031, 0.6135, 0.3153},   //44
    {0xFF001E, 0.62841, 0.32361}, //45
    {0xFF0000, 0.6401, 0.33},     //46
    {0xFF0600, 0.6389, 0.3309},   //47
    {0xFF1C00, 0.6329, 0.3356},   //48
    {0xFF2D00, 0.62426, 0.3425},  //49
    {0xFF3E00, 0.6122, 0.3521},   //50
    {0xFF4A00, 0.60178, 0.36035}, //51
    {0xFF5500, 0.5911, 0.3688},   //52
    {0xFF5C00, 0.58382, 0.37461}, //53
    {0xFF6200, 0.57735, 0.37975}, //54
    {0xFF6900, 0.5696, 0.3859},   //55
    {0xFF7000, 0.5616, 0.39225},  //56
    {0xFF7700, 0.55351, 0.39869}, //57
    {0xFF7D00, 0.5465, 0.4042},   //58
    {0xFF8500, 0.53713, 0.41169}, //59
    {0xFF8D00, 0.5278, 0.4191},   //60
    {0xFF9400, 0.51968, 0.42555}, //61
    {0xFF9B05, 0.5111, 0.4313},   //62
    {0xFFA107, 0.50408, 0.43637}, //63
    {0x0000FF, 0.4961, 0.4422},   //64
};



/* ******************internal function declarations****************************/
void DiagnosticSubscribedCmdsHandle(const DiagReqInfo_t * const frameInfo);
void UnconditionalSubscribedCmdsHandle(LIN_Device_Frame_t const *frame);
void UnconditionalPublishedCmdsISR(LIN_Device_Frame_t *const frame);
void UnconditionalPublishedCmdsTxFinishedISR(uint8_t fid, uint8_t resvd);
void DiagnosticSleepRequestHandle(SleepRequestType_t type);
void BusWakeUpRequestHandle(BusWakeUpRequestResult_t result);
void APPL_HandleCHANGEFIDCMD(LIN_Device_Frame_t const *frame);

/* *******************global variants declarations*****************************/
static TaskState_t        linsTaskState = TASK_STATE_INIT;
static uint8_t diagDataRspBuff[DIAG_DATA_BUFF_SIZE];
static uint8_t diagDataRecvBuff[DIAG_DATA_BUFF_SIZE];
static volatile uint8_t busWakeupRetryCount = 0U;
static uint8_t CHANGE_FID_RESPOND;	//决定了从节点回复给主机的报文信息
static uint8_t RECEVICE_FID;		//主机用报文分配给从机的FID

void busWakeupRetryTimerExpired(SoftTimer_t *timer);

static SoftTimer_t busWakeupRetryTimer = {
    .mode     = TIMER_ONE_SHOT_MODE,
    .interval = 2250U,
    .handler  = busWakeupRetryTimerExpired
};

static volatile uint8_t eventTriggeredLedOn = FALSE;

/*  LIN command handle callbacks declarations  */
static ls_LinsFramesCallback_t linsFramesCallback ={
  UnconditionalSubscribedCmdsHandle,            /* received data from master */
  UnconditionalPublishedCmdsISR,                /* send data to master, it's an interrupt function, please fill the data as fast as possible */
  UnconditionalPublishedCmdsTxFinishedISR,      /* send data to master finished, it's an interrupt function, please fill the data as fast as possible  */
  DiagnosticSubscribedCmdsHandle,               /* Diagnostic sigle PDU and MultiPDU received data from master */
  NULL,                                         /* special functional NAD (0x7E) handler*/
  DiagnosticSleepRequestHandle,                 /* Diagnostic sleep request from master */
  BusWakeUpRequestHandle,                       /* Get the bus wake up result this would take effects after call API: ls_send_wake_up_bus_signal()*/
  NULL,                                         /* SAE J2602 -1 Configuration Messages */
  NULL,                                         /*  5.7.2.5 DNN Based Broadcast Messages */
};

/* **FID table declarations****/
static LIN_Device_Frame_t UnconditionalCmdsTable[DEFAULT_LINS_FID_SIZE] = {
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
  [USER_DATA_REPORT_INDEX] = {
      .frame_id = USER_DATA_REPORT,   /* user data report */
      .msg_type = LIN_MSG_TYPE_TX,
      .checksum = LIN_CHECKSUM_ENHANCED,
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
      .linkedEventTriggerFidTableIndex = EVENT_TRIGGERED_INDEX,
  },
  [EVENT_TRIGGERED_INDEX] = {
      .frame_id = EVENT_TRIGGER_DATA_REPORT,   /* event trigger data report */
      .msg_type = LIN_MSG_TYPE_TX_EVENT,       /*event trigger data request from sender for slave; */
      .checksum = LIN_CHECKSUM_ENHANCED,       
      .length   = LIN_BUFF_SIZE,
      .frameIsValid = TRUE,
      .eventTriggered = FALSE,
      .linkedEventTriggerFidTableIndex = USER_DATA_REPORT_INDEX,
  },
  [YZ_CHANGE_FID_INDEX] = {					// 节点配置		  
		.frame_id = YZ_CHANGE_FID_CTRL,		// 0x3f 
		.msg_type = LIN_MSG_TYPE_RX,			  
		.checksum = LIN_CHECKSUM_ENHANCED,		 
		.length   = LIN_BUFF_SIZE,
		.frameIsValid = TRUE,
  },
  [YZ_RESPOND_FID_INDEX] = {				//节点配置时给主机响应发的FID
	  .frame_id = YZ_CHANGE_FID_RESPOND,	//0x3e	   
	  .msg_type = LIN_MSG_TYPE_TX,				
	  .checksum = LIN_CHECKSUM_ENHANCED,	   
	  .length	= LIN_BUFF_SIZE,
	  .frameIsValid = TRUE,

  },
  [FID_DOOR_CTRL_INDEX] = {					// 门控制	
	//	.frame_id = LEFT_CONFIG,	 		// 0x03
		.msg_type = LIN_MSG_TYPE_RX,		 
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
                      SYSCTRLA_REG_RETAIN0 = 0x05U;
                      PMUA_REG_CTRL.UPDATE = 1U;
                      while(PMUA_REG_CTRL.UPDATE == 1U);
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
    (void)ls_clr_go_to_sleep_flag();
    /* Set system to hibranate mode*/
//    PMU_EnterDeepSleepMode();
}

uint8_t ALU_FID_temp;
void CHANGE_DEVICE_FID(void)	//copy
{
	  ALU_FID_temp = GET_ALU_FID();
	  UnconditionalCmdsTable[FID_DOOR_CTRL_INDEX].frame_id = ALU_FID_temp /*GET_ALU_INFO_FID(ALU_FID_temp)*/;
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


void UnconditionalPublishedCmdsTxFinishedISR(uint8_t fid, uint8_t resvd)
{
    if ( fid == UnconditionalCmdsTable[FID_STATUS_FRAME_INDEX].frame_id){
        (void)ls_ifc_clear_error_status();
        (void)ls_clr_error_code_log();
        (void)ls_clr_overrun_flag();
    }
}
/*  
  User defined published frame callback function which from LINS ISR
  please fill frame data[0-7] only, please don't do any changes on the left info except data[x]!!!!
*/
void UnconditionalPublishedCmdsISR(LIN_Device_Frame_t *const frame)
{
    /*   feed buffer to frame   */
    if (frame->frame_id == UnconditionalCmdsTable[FID_STATUS_FRAME_INDEX].frame_id){
        /* status management frame */
        lin_status_t status = ls_ifc_read_status();
        frame->data[0] = (uint8_t)status.payload;
        frame->data[1] = LINS_GetPIDFromFID(status.pid);
        frame->data[2] = ls_read_error_code_log();
        for (uint8_t i = 3U; i < LIN_BUFF_SIZE; i++){
            frame->data[i] = 0xFFU;
        }
    }else if (frame->frame_id == UnconditionalCmdsTable[USER_DATA_REPORT_INDEX].frame_id){
        /* user defined data report */
        frame->data[0] = (APPL_GetLEDState()) | (eventTriggeredLedOn << 1U) | 0xF8U;
        for (uint8_t i = 1U; i < LIN_BUFF_SIZE; i++){
            frame->data[i] = 0xFFU;
        }
    }else if (frame->frame_id == UnconditionalCmdsTable[EVENT_TRIGGERED_INDEX].frame_id){
        /* event trigger data report */
        if (frame->eventTriggered == TRUE){
            frame->data[0] = LINS_GetPIDFromFID(UnconditionalCmdsTable[UnconditionalCmdsTable[EVENT_TRIGGERED_INDEX].linkedEventTriggerFidTableIndex].frame_id);/* transfer FID to PID  */
            frame->data[1] = (APPL_GetLEDState()) | (eventTriggeredLedOn << 1U) | 0xF8U;
            for (uint8_t i = 2U; i < LIN_BUFF_SIZE; i++){
                frame->data[i] = 0xFFU;
            }
        }
    }
	// ↓copy
	else if(frame->frame_id == UnconditionalCmdsTable[YZ_RESPOND_FID_INDEX].frame_id)
	{
		if (frame != NULL)
			if (CHANGE_FID_RESPOND == 0x02)
			{
				frame->data[0] = 0x02;
				frame->data[1] = PROJECT_NAME;
				frame->data[2] = 0xFF;
				frame->data[3] = 0xFF;
				frame->data[4] = 0xFF;
				frame->data[5] = 0xFF;
				frame->data[6] = 0xFF;
				frame->data[7] = 0xFF;
				CHANGE_FID_RESPOND = 0; 		
			}
			if (CHANGE_FID_RESPOND == 0x04)
			{
				frame->data[0] = 0x04;
				frame->data[1] = PROJECT_NAME;
				frame->data[2] = 0xFF;
				frame->data[3] = 0xFF;
				frame->data[4] = 0xFF;
				frame->data[5] = 0xFF;
				frame->data[6] = RECEVICE_FID;
				frame->data[7] = 0xFF;
				CHANGE_FID_RESPOND = 0; 		
			}
	}
		else{
        /* intent to empty   */
    }
}


void LINS_EventTriggered_ISR(void)
{
    if (eventTriggeredLedOn == TRUE){
        eventTriggeredLedOn = FALSE;
        GPIO_Set(GPIO_PORT_4, GPIO_HIGH);
    }else{
        eventTriggeredLedOn = TRUE;
        GPIO_Set(GPIO_PORT_4, GPIO_LOW);
    }
    UnconditionalCmdsTable[EVENT_TRIGGERED_INDEX].eventTriggered = TRUE;
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

/* 
  User defined subscribed frame received from LIN master
*/
void UnconditionalSubscribedCmdsHandle(LIN_Device_Frame_t const *frame)
{
    if (frame->frame_id == UnconditionalCmdsTable[FID_COLOR_CTRL_INDEX].frame_id){
        APPL_HandleColorControlCommands(frame);
    }

	if (frame->frame_id == UnconditionalCmdsTable[YZ_CHANGE_FID_INDEX].frame_id)	//copy
	{
        APPL_HandleCHANGEFIDCMD(frame);
    }

	if (frame->frame_id == UnconditionalCmdsTable[FID_DOOR_CTRL_INDEX].frame_id)	//进入门FID处理函数
	{
		// 处理颜色
		uint8_t Colour_Buff = frame->data[4];
		uint8_t Colour_Red = rgbTable[Colour_Buff-1].RGB >> 16;
		uint8_t Colour_Green = (rgbTable[Colour_Buff-1].RGB >> 8) & 0xff;
		uint8_t Colour_Blue = (rgbTable[Colour_Buff-1].RGB) & 0xff;
				// 处理亮度
		uint8_t DimLevel_Buff = frame->data[5]; 
		if(Colour_Buff == 0)
		{
			DimLevel_Buff = 0;
		}
		if(Colour_Buff > COLOR_TABLE_SIZE)
			Colour_Buff = COLOR_TABLE_SIZE;


        (void)CLM_SetRGBL(LED0,MES_GetLedTemperature(LED0), Colour_Red, Colour_Green, Colour_Blue,(uint16_t)DimLevel_Buff*10U, (uint16_t)100U);
    }

}


void APPL_HandleCHANGEFIDCMD(LIN_Device_Frame_t const *frame) // copy
{
	static uint32_t old_crc = 0xFFFFFFFF;
	static uint8_t RxChangeState;
	USER_CHANGE_FID_t *USER_CMD;

	
	uint32_t temp = CRC32_GetRunTimeCRC32((uint8_t*)frame->data, LIN_BUFF_SIZE);
	CRC32_Init();
	if (temp != old_crc)
	{
		old_crc == temp;
	
        USER_CMD = (USER_CHANGE_FID_t*)(frame ->data);

        switch(RxChangeState)	
		{
		case CHANGE_INIT:
			if(USER_CMD->matrix.CMD == 0x01)
			{
				ColorTemperature_t temperature = {24,24,24};
				CLM_SetRGBL(LED0, temperature, 0U , 0U , 0U , 0U , 0U);
				RxChangeState = READproject;
			}
			break;
		case READproject:
			
			if(USER_CMD->matrix.CMD == 0x02)
			{
				CHANGE_FID_RESPOND = 0x02;
				RxChangeState = ASSIGNnad;
			}
		case ASSIGNnad:
			if((USER_CMD->matrix.CMD == 0x03) && (USER_CMD->matrix.PROJECT == PROJECT_NAME))
			{
                RECEVICE_FID = GET_ALU_FID();
				if((RECEVICE_FID != USER_CMD->matrix.FID) /*&& (USER_CMD->matrix.FID != 0)*/)
				{
					RECEVICE_FID = USER_CMD->matrix.FID;
					ASSIGN_FID(USER_CMD->matrix.FID);
					RxChangeState = TAKEOUT;
				}
			}
		case TAKEOUT:
			if(USER_CMD->matrix.CMD == 0x04)
			{
				CHANGE_FID_RESPOND = 0x04;
				RECEVICE_FID = GET_ALU_FID();
				RxChangeState = CHANGE_INIT;
			}
		
		default:
		break;
		}
    }
}




void LINS_TaskHandler(void)
{
    switch(linsTaskState){
    case TASK_STATE_ACTIVE:
      break;
    case TASK_STATE_INIT:
	  CHANGE_DEVICE_FID();	//copy
      ls_register_services(LIN_PROTOCOL_LIN2_2A,UnconditionalCmdsTable, (l_u8)(sizeof(UnconditionalCmdsTable)/sizeof(LIN_Device_Frame_t)), DIAG_DATA_BUFF_SIZE,&linsFramesCallback);
      (void)ls_set_tp_timeout(N_AS, N_CR);
      /* Note that please don't change the following glitch configuration!!!!*/
      (void)ls_set_lins_rx_glitch_filter_1st(0x00U,0x08U);
      (void)ls_set_lins_rx_glitch_filter_2nd(0x0AU,0x10U);
      (void)ls_set_lins_rx_glitch_filter_3rd(0x30U,0x30U);

      (void)l_sys_init();
      (void)ls_disable_lin_auto_sleep(TRUE);
      
//      if(GPIO_SFRS->GPIO_CFG[(uint8_t)GPIO_PORT_1].ACTDET != 0U){
//          GPIO_SFRS->GPIO_CFG[(uint8_t)GPIO_PORT_1].CLR = 1U;
//          LINS_EventTriggered_ISR();
//      }
//      GPIO_RegisterIRQ(GPIO_PORT_1, GPIO_EDGE_FALLING, LINS_EventTriggered_ISR);
//      
//      
//      if(GPIO_SFRS->GPIO_CFG[(uint8_t)GPIO_PORT_2].ACTDET != 0U){
//          GPIO_SFRS->GPIO_CFG[(uint8_t)GPIO_PORT_2].CLR = 1U;
//          LINS_SendWakeUpSignal_ISR();
//      }
//      GPIO_RegisterIRQ(GPIO_PORT_2, GPIO_EDGE_FALLING, LINS_SendWakeUpSignal_ISR);
      linsTaskState = TASK_STATE_ACTIVE;
      break;
    default:
      break;
    }
}

#endif