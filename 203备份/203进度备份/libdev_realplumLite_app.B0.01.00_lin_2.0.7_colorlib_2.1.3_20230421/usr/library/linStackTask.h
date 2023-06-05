/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file lin_stack.h
  */

#ifndef LIN_STACK_H__
#define LIN_STACK_H__

#include <stdint.h>
#include <taskManager.h>
#include <lin_device.h>

/**
 * @brief The definitions and enumerations used in the API function in LIN stack.
 */
typedef uint8_t         l_bool; 
typedef uint8_t         l_ioctl_op; 
typedef uint8_t         l_irqmask; 
typedef uint8_t         l_u8; 
typedef uint16_t        l_u16; 
typedef uint32_t        l_u32; 

typedef uint8_t         l_signal_handle; 
typedef uint8_t         l_flag_handle; 
typedef uint8_t         l_ifc_handle; 
typedef uint8_t         l_schedule_handle;

typedef enum{
  LIN_PCI_SF = 0U,
  LIN_PCI_FF,
  LIN_PCI_CF
}PCI_Types_t;

typedef enum{
  PDU_TYPE_SINGLE_PDU = 0U,
  PDU_TYPE_MULTI_PDU,
}PDU_Types_t;

typedef enum{
  TX_IDLE = 0U,
  TX_SENDING,
  TX_FINISHED,
  TX_FAILED
}TxStatus_t;

typedef enum{
  LIN_PROTOCOL_LIN2_2A = 0U,
  LIN_PROTOCOL_SAE_J2602,
}LinProtocolType_t;

typedef enum{
  J2602_ERROR_NO_DETECTED_FAULT  = 0,
  J2602_ERROR_RESET,
  J2602_ERROR_RESERVED0,
  J2602_ERROR_RESERVED1,
  J2602_ERROR_DATA_ERROR,
  J2602_ERROR_CHECKSUM_ERROR,
  J2602_ERROR_BYTE_FIELD_FRAMING_ERROR,
  J2602_ERROR_ID_PARITY_ERROR,
}J2602ErrCode_t;


#define SF_MAX_DATA_LENGTH              (5U)

#define LIN_NAD_FUNCTION                (0x7EU)
#define LIN_NAD_WILDCARD                (0x7FU)
#define LIN_SUPPLIER_ID_WILDCARD        (0x7FFFU)
#define LIN_FUNCTION_ID_WILDCARD        (0xFFFFU)
#define LIN_SERIALNO_WILDCARD           (0x01234567U)

/**
 * @brief The enumerations of frame identifiers.
 */
#define LIN_FRAME_ID_J2602_DNN_BROADCAST_START      (0x38U)
#define LIN_FRAME_ID_J2602_DNN_BROADCAST_END        (0x3BU) 
   
   
#define LIN_FRAME_ID_J2602_DNN_BROADCAST_38      (0x38U)
#define LIN_FRAME_ID_J2602_DNN_BROADCAST_39      (0x39U)
#define LIN_FRAME_ID_J2602_DNN_BROADCAST_3A      (0x3AU)
#define LIN_FRAME_ID_J2602_DNN_BROADCAST_3B      (0x3BU)
   
   
#define LIN_FRAME_ID_MASTER_REQ         (0x3CU)
#define LIN_FRAME_ID_SLAVE_RSP          (0x3DU)
#define LIN_FRAME_ID_MASTER_REQ_3E      (0x3EU)
   
#define LIN_FRAME_ID_FUNCTIONAL         (0x7EU)
#define LIN_FRAME_ID_UNASSIGN           (0x00U)
#define LIN_FRAME_ID_DONOTCARE          (0xFFU)

/**
 * @brief The enumerations of service IDs.
 */
#define LIN_SID_ASSIGN_NAD              (0xB0U)
#define LIN_SID_ASSIGN_FRAME_ID         (0xB1U)
#define LIN_SID_READ_BY_ID              (0xB2U)
#define LIN_SID_CONDITIONAL_CHANGE_NAD  (0xB3U)
#define LIN_SID_DATA_DUMP               (0xB4U)
#define LIN_SID_RESERVED                (0xB5U)
#define LIN_SID_SAVE_CONFIGURATION      (0xB6U)
#define LIN_SID_ASSIGN_FRAME_ID_RANGE   (0xB7U)
   
#define LIN_SID_SNPD                    LIN_SID_RESERVED
#define LIN_SID_J2602_HARDWARE_RST      LIN_SID_RESERVED	
   
typedef union{
  struct{
    uint8_t  appInfo0      :1;
    uint8_t  appInfo1      :1;
    uint8_t  appInfo2      :1;
    uint8_t  appInfo3      :1;
    uint8_t  appInfo4      :1;
    uint8_t  err           :3;
  };
  uint8_t payload;
}J2602StatusByte_t;

typedef union{
  struct{
      uint8_t  nad;
      union{
          uint8_t pci;
          struct{
            uint8_t  pci_extInfo  :4;
            uint8_t  pci_type     :4;
          };
      };
      union{
        uint8_t sid;
        uint8_t length;
        uint8_t rsid;
        struct{
          uint8_t length_ff;
          uint8_t sid_rsid_ff;
        };
        uint8_t payload[6];
      };
  };
  uint8_t data[8];
}NodeStruct_t;

typedef union
{
struct
	{
	uint8_t CMD;
	uint8_t PROJECT;
	uint8_t RESERVED1;
	uint8_t RESERVED2;
	uint8_t RESERVED3;
	uint8_t RESERVED4;
	uint8_t FID;
	uint8_t NAD;
	}matrix;
uint8_t date[LIN_BUFF_SIZE];
}USER_CHANGE_FID_t;

enum
{
	CHANGE_INIT,
	READproject,
	ASSIGNnad,
	TAKEOUT,
};

typedef struct{
  uint8_t  errorCode;
  uint8_t  sid;                 /*  sid */
  PDU_Types_t type;             /*  pdu type single or multi pdu */
  uint16_t packLength;          /*  the complete valid data buffer length except the length of sid   */
  uint16_t packCurrIndex;       /*  the current data position index in the complete data buffer     */
  uint16_t frameDataLength;     /*  the current frame valid data length    */
  uint8_t  *payload;            /*  data in received buffer */
}DiagReqInfo_t;

typedef struct{
  uint8_t  sid;                 /*  sid */
  PDU_Types_t type;             /*  pdu type single or multi pdu */
  uint16_t packLength;          /*  the complete valid data buffer length except the length of SID   */
  uint8_t  *payload;            /*  data buffer to be sent except SID*/
}DiagRspInfo_t;

typedef enum{
    SLEEP_REQ_TYPE_MASTER_REQ = 0,     /*  sleep request with filled with 0xFF in buff[1-7] */
    SLEEP_REQ_TYPE_MASTER_REQ_INFORMAL,/*  sleep request without filled with 0xFF in buff[1-7] */
    SLEEP_REQ_TYPE_BUS_IDLE_TIMEROUT,  
}SleepRequestType_t;

typedef enum{
    BUS_WAKEUP_REQ_RESULT_MASTER_ACTIVE = 0,     /*  wakeup bus successfully, master send frame ID after wake up */
    BUS_WAKEUP_REQ_RESULT_REQ_TIMEOUT,           /*  wakeup bus failed after 3 tries  */
}BusWakeUpRequestResult_t;

typedef void (*ls_UnconditionalReqCallback_t)(LIN_Device_Frame_t const *frame);
typedef void (*ls_UnconditionalRspCallback_t)(LIN_Device_Frame_t *const frame);
typedef void (*ls_PublisherTxFinishedCallback_t)(uint8_t fid, uint8_t j2602CfgRsp);
typedef void (*ls_DiagReqCallback_t)(const DiagReqInfo_t * const frameInfo);
typedef void (*ls_DiagFunctionalNADCallback_t)(const NodeStruct_t * const frame);
typedef void (*ls_DiagSleepReqCallback_t)(SleepRequestType_t type);
typedef void (*ls_BusWakeUpRequestCallback_t)(BusWakeUpRequestResult_t result);

typedef void (*ls_DiagReqLogCallback_t)(const LIN_Device_Frame_t * const frame);
typedef void (*ls_FrameIdLogCallback_t)(uint8_t frameId);

typedef void (*ls_SAEJ2602CfgReqCallback_t)(uint8_t fid,uint8_t const *payload, uint8_t length);
typedef void (*ls_SAEJ2602DNNBroadcastReqCallback_t)(uint8_t fid,uint8_t const *payload, uint8_t length);

typedef struct{
  ls_UnconditionalReqCallback_t         unconditionalSubscribedCallback; /* received data from master */
  ls_UnconditionalRspCallback_t         unconditionalPublishedCallback;  /* send data to master */
  ls_PublisherTxFinishedCallback_t      publisherTxFinishedCallback;     /* send data to master finished */
  ls_DiagReqCallback_t                  diagnosticSubscribedCallback;    /* Diagnostic and MultiPDU received data from master */
  ls_DiagFunctionalNADCallback_t        diagFunctionalNADCallback;       /* special functional NAD (0x7E) handler*/
  ls_DiagSleepReqCallback_t             diagnosticSleepRequestCallback;  /* Diagnostic sleep request from master   */
  ls_BusWakeUpRequestCallback_t         busWakeUpRequestCallback;        /* got the bus state after slave sending wake up signal.*/
  ls_SAEJ2602CfgReqCallback_t           SAEJ2602CfgReqCallback;          /* refers to SAEJ2602-1 5.7.2.3 Configuration Messages */
  ls_SAEJ2602DNNBroadcastReqCallback_t  SAEJ2602DNNBroadcastReqCallback; /*  DNN Based Broadcast Messages  */
}ls_LinsFramesCallback_t;

typedef struct{
  l_u8  device;             /* 1: means realplum(IND83205) , 2: rugby(IND83204)  3: realplumPro(IND82209),4: rugbyPro(IND83211),5: realplumLite(IND83212)*/
  l_u8  protocol;           /* 0:with LIN stack V2.2A ,1: with LIN stack SAE_J2602, 2: with LIN stack V2.2A_SAE_J2602 */
  l_u16 version;            /* 0x0101 means V1.01   */
}lin_stack_version_t;

/**
* @brief A structure to represent LIN status information.
*/
typedef union {
  struct{
    l_u8 error_in_resp                  :1;
    l_u8 success_in_transfer            :1;
    l_u8 overrun                        :1;
    l_u8 goto_sleep                     :1;
    l_u8 bus_activity                   :1;
    l_u8 event_trig_frame_collision     :1;
    l_u8 save_config                    :1;
    l_u8 parity_error                   :1; /* FID parity error bit */
    l_u8 pid;
  };
  l_u16 payload;
}lin_status_t;
   
   
typedef enum{
    LD_SET_OK = 0,
    LD_LENGTH_NOT_CORRECT,
    LD_DATA_ERROR
}ld_set_status_t;

typedef enum{
    LD_READ_OK = 0,
    LD_LENGTH_TOO_SHORT,
}ld_read_status_t;

/*
 *  Lin stack main task which was called by task manager, please don't use for other purpose!!!: 
 *  @param [in] none
 *  @return none
 */
void LinStack_TaskHandler(void);

/*
 *  Get current Lin stack version: 
 *  @param [in] none
 *  @return lin_stack_version: majorVer: main version; subVer: branch version
 */
lin_stack_version_t ls_get_lin_version(void);

/*
 *  Lin stack initialization: 
 *  @param [in] none
 *  @return 0
 */
l_bool l_sys_init(void); 

/*
 *  Set Lin transport layer timeout parameters: 
 *  @param [in] n_as: N_As, refer to LIN spec 2.2A
 *  @param [in] n_cr: N_Cr, refer to LIN spec 2.2A
 *  @return 0
 */
l_u8 ls_set_tp_timeout(uint16_t n_as, uint16_t n_cr);

/*
 *  Set Lin lin tx bit monitor: 
 *  @param [in] enable: TRUE: enable bit monitor;  FALSE: disable bit monitor
 *  @return 0
 */
l_u8 ls_set_tx_bit_monitor(l_u8 enable);

/*
 *  Disable LIN auto sleep when LIN timeout: 
 *  @param [in] TRUE: disable auto sleep;  FALSE: enable auto sleep
 *  @return 0
 */
l_u8 ls_disable_lin_auto_sleep(l_u8 disable);

/*
 *  Set Lin phy 1st rx glitch signal filter timing : 
 *  @param [in] risingTime: level 0 -> level1 delay time;  range 0-127 unit: 62.5us
 *  @param [in] fallingTime:level 0 -> level 1 delay time, range 0-127 unit: 62.5us
 *  @return 0, successfully , others: failed
 */
l_u8 ls_set_lins_rx_glitch_filter_1st(l_u8 risingTime,l_u8 fallingTime);

/*
 *  Set Lin phy 2nd rx glitch signal filter timing : 
 *  @param [in] risingTime: level 0 -> level1 delay time;  range 0-127 unit: 62.5us
 *  @param [in] fallingTime:level 0 -> level 1 delay time, range 0-127 unit: 62.5us
 *  @return 0, successfully , others: failed
 */
l_u8 ls_set_lins_rx_glitch_filter_2nd(l_u8 risingTime,l_u8 fallingTime);

/*
 *  Set Lin phy 3rd rx glitch signal filter timing : 
 *  @param [in] risingTime: level 0 -> level1 delay time;  range 0-127 unit: 62.5us
 *  @param [in] fallingTime:level 0 -> level 1 delay time, range 0-127 unit: 62.5us
 *  @return 0, successfully , others: failed
 */
l_u8 ls_set_lins_rx_glitch_filter_3rd(l_u8 risingTime,l_u8 fallingTime);


/*
 *  Reqister supported unconditional frame table 
 *  @param [in] protocol: LIN protocol type 0: LIN_PROTOCOL_LIN2_2A, 1: LIN_PROTOCOL_SAE_J2602.
 *  @param [in] frameTable: frame table location
 *  @param [in] tableSize: frame table size
 *  @param [in] diagPDUSubscriberBuffLength: diagnostic multi pdu max supported data length, this value should be within 7-4095
 *  @param [in] pFunCallback: callback fucntions for diagnostic frame and unconditional frames.
 *  @return none
 */
void ls_register_services(LinProtocolType_t protocol,LIN_Device_Frame_t *frameTable, l_u8 tableSize, uint16_t diagPDUSubscriberBuffLength,const ls_LinsFramesCallback_t *pFunCallback);

/*
 *  Reqister diagnostic request callback for debug purpose.
 *  @param [in] callback: get the meta data for any diagnostic Req (0x3C)frame.
 *  @return none
 */
void ls_registerDiagReqLogPrint(ls_DiagReqLogCallback_t callback);

/*
 *  Reqister got an valid frameId from LIN bus ignore whether it is registered in the table, it's an ISR function!!!!
 *  @param [in] callback: get the frameId .
 *  @return none
 */
void ls_registerFrameIdLogPrint_ISR(ls_FrameIdLogCallback_t callback);

/*
 *  Diagnostic frame response function which is used to send back data to LIN master.
 *  @param [in] rsp: refer to description of structure DiagRspInfo_t definition.
 *  @return none
 */
void ls_handle_diagnostic_response(DiagRspInfo_t *rsp);

/*
 *  SAEJ2602 CFG 0x3C or 0x3E frame response function which is used to send back data to LIN master.
 *  @param [in] rsp: refer to SAEJ2602-1 5.7.2.4 Response Message to Options 2 and 3 in 5.7.2.3.
 *  @param [in] statusByte:J2602-1 Status Byte(defined in 5.8.6) 
 *  @param [in] payload: databuff 0-6 the paylad length should be 7 by default.
 *  @return none
 */
void ls_handle_SAEJ2602Cfg_response(l_u8 statusByte, l_u8 *payload);

/*
 *!!!!!!!!!!!!!!!!!!!Note that obsolete not recommended for future use!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
 *  frame response send status.
 *  @return TxStatus_t.
 */
TxStatus_t ls_tx_status(void);

/*
 *  Set device initial NAD to LIN stack for assigning NAD command.
 *  @param [in] nad: initial NAD of device.
 *  @return none
 */
void ls_set_initialNad(l_u8 nad);
/*
 *  Set device NAD to LIN stack.
 *  @param [in] nad: current NAD of device.
 *  @return none
 */
void ls_set_nad(l_u8 nad);

/*
 *  Red NAD of LIN stack.
 *  @param [in] none.
 *  @return current NAD of LIN stack.
 */
l_u8 ls_read_nad(void);

/*
 *  Red NAD of current diagnostic frame sent from LIN master, there are only probably 2 values: current NAD or 0x7FU(NAD wildcard).
 *  @param [in] none.
 *  @return NAD of current diagnostic frame.
 */
l_u8 ls_read_nad_from_master(void);

/*
 *  Save current LIN Stack parameters such as NAD, PIDs etc. refer to pdsTask for more information.
 *  @param [in] none.
 *  @return 0.
 */
l_u8  ls_save_configuration(void);

/*
 *  clear save_configuration flag.
 *  @param [in] none.
 *  @return 0.
 */
l_u8 ls_clr_save_configuration_flag(void);

/*  
 *  !!!!!!!!!!!!!!!!!!!Note that obsolete not recommended for future use!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
 *   replace with function ls_ifc_read_status() and ls_ifc_read_status()
 *  Read current error flag and system information, refer to struture lin_status_t for more information.
 *  @param [in] none.
 *  @return:
    l_u8 error_in_resp                  :1;
    l_u8 success_in_transfer            :1;
    l_u8 overrun                        :1;
    l_u8 goto_sleep                     :1;
    l_u8 bus_activity                   :1;
    l_u8 event_trig_frame_collision     :1;
    l_u8 save_config                    :1;
    l_u8 parity_error                   :1;
    l_u8 pid;
 */
l_u16 ls_read_sys_status(void);

/*  
 *  !!!!!!!!!!!!!!!!!!!Note that obsolete not recommended for future use!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
 *   replace with function ls_ifc_read_status() and ls_ifc_read_status()
 *  Read current error flag and system information, simulate with ls_read_sys_status
 *  @return:
    l_u8 error_in_resp                  :1;
    l_u8 success_in_transfer            :1;
    l_u8 overrun                        :1;
    l_u8 goto_sleep                     :1;
    l_u8 bus_activity                   :1;
    l_u8 event_trig_frame_collision     :1;
    l_u8 save_config                    :1;
    l_u8 parity_error                   :1;
    l_u8 pid;
*/
lin_status_t ls_read_sys_status_ext(void);

/*  
 *  Read current error flag and system information, simulate with ls_read_sys_status not don't clear the status
 *  @return:
    l_u8 error_in_resp                  :1;
    l_u8 success_in_transfer            :1;
    l_u8 overrun                        :1;
    l_u8 goto_sleep                     :1;
    l_u8 bus_activity                   :1;
    l_u8 event_trig_frame_collision     :1;
    l_u8 save_config                    :1;
    l_u8 parity_error                   :1;
    l_u8 pid;
*/
lin_status_t ls_ifc_read_status(void);

/*
 *  reset lin error status.
 *  @param [in] errCode.
 *  @return: 0U;
 */
l_u8 ls_ifc_clear_error_status(void);

/*  
 *  !!!!!!!!!!!!!!!!!!!Note that obsolete not recommended for future use!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
 *   replace with  function : ls_read_error_code_log() and ls_clr_error_code_log() 
 *  Read current lin stack communication error reasons, this is only for internal use.
 *  @param [in] none.
 *  @return: ((1U << ERROR_BITERR_POS) | (1U << ERROR_CHK_POS) | (1U << ERROR_BITMON_POS) | (1U << ERROR_FRAMEERR_POS)) refer to LINS->ERROR register
 */
l_u8  ls_read_error_code(void);

/* 
 *  Read current lin stack communication error reasons, this is only for internal use and don't clear the error log
 *  @param [in] none.
 *  @return: ((1U << ERROR_BITERR_POS) | (1U << ERROR_CHK_POS) | (1 << ERROR_PARITY_POS) | (1U << ERROR_BITMON_POS) | (1U << ERROR_FRAMEERR_POS))refer to LINS->ERROR register
 */
l_u8 ls_read_error_code_log(void);

/*
 *  clear the erroe code log.
 *  @param [in] none.
 *  @return: 0;
 */
l_u8 ls_clr_error_code_log(void);

/*
 *  Read SAEJ2602 communication error state.
 *  @param [in] none.
 *  @return: J2602ErrCode_t
 */
J2602ErrCode_t ls_read_J2602_error_code(void);

/*
 *  reset SAEJ2602 communication error state when response successfully.
 *  @param [in] errCode.
 *  @return: none
 */
void ls_reset_J2602_error_code(J2602ErrCode_t errCode);
   
/*
 *  Notice lin stack it's a hardware reset operation.
 *  @param [in] 0: hardware reset is false, 1: hardware reset is true.
 *  @return: none
 */
void ls_set_J2602_hardware_reset(uint8_t hardwareReset);


/*
 *  read go_to_sleep flag
 *  @param [in] none.
 *  @return: state: 1: got go_to_sleep command, 0: not got go_to_sleep command
 */
l_u8 ls_read_go_to_sleep_flag(void);
/*
 *  Clear go_to_sleep flag
 *  @param [in] none.
 *  @return: 0U
 */
l_u8 ls_clr_go_to_sleep_flag(void);

/*
 *  Clear overrun flag
 *  @param [in] none.
 *  @return: 0U
 */
l_u8 ls_clr_overrun_flag(void);

/*
 *  Read current LIN BUS Status.
 *  @param [in] none.
 *  @return: 1: active, 0: IDLE;, refer to "bus_activity" in function:ls_ifc_read_status();
 */
l_u32 ls_linbus_activity(void);

/*
 *  Slave trigger to send wake up signal on LIN bus for 3 times if there is no LIN frame on the LIN bus after wake up signal.
 *  @param [in] none.
 *  @return: none;
 */
void  ls_send_wake_up_bus_signal(void);

#endif /* __LIN_STACK_H__  */