/**
 * @copyright 2021 Indie microcontroller.
 *
 * This file is proprietary to Indie microcontroller.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie microcontroller.
 *
 * @file linsNodeCfgIdentify.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2021/07/06
 */

#include <linStackTask.h>
#include <linsNodeCfgIdentify.h>
#include <linSlaveTask.h>
#include <pdsTask.h>
#include <SoftTimerTask.h>
#include <appConfig.h>

#if LIN_STACK_TYPE == LIN_STACK_TYPE_LIN2_2A

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

uint8_t LNCI_HandleNodeCfgIdentifyRequest(const DiagReqInfo_t *const diagReq,DiagRspInfo_t *const diagRsp)
{
  uint8_t response = FALSE;
  if (diagReq->sid == LIN_SID_ASSIGN_NAD && diagReq->packLength == sizeof(AssignNAD_t)){/* Assign NAD */
      /*
      4.2.5.1 Assign NAD
      Assign NAD is used to resolve conflicting NADs in LIN clusters built using off-theshelves slave nodes or reused slave nodes. 
      This request uses the initial NAD (or the NAD wildcard); this is to avoid the risk of losing the address of a slave node. The NAD
      used for the response shall be the same as in the request, i.e. the initial NAD.
      */
      uint8_t nad = ls_read_nad_from_master();
      if (nad == INIT_LINS_NAD_ADDR || nad == LIN_NAD_WILDCARD){
          AssignNAD_t *assignNAD = (AssignNAD_t *)((void *)diagReq->payload);
          if ((assignNAD->supplierID == LINS_SUPPLIER_ID || assignNAD->supplierID == LIN_SUPPLIER_ID_WILDCARD)&&
              (assignNAD->functionID == LINS_FUNCTION_ID || assignNAD->functionID == LIN_FUNCTION_ID_WILDCARD)){
              /* A response shall only be sent if the NAD, the Supplier ID and the Function ID match. If successful */
              if (assignNAD->newNAD > 0U){
                  diagRsp->packLength = 0U;
                  linParams->nad = assignNAD->newNAD;
                  ls_set_nad(assignNAD->newNAD); /* set nad to lin stack */
                  response = TRUE;
              }
          }
      }
  }else if (diagReq->sid == LIN_SID_SAVE_CONFIGURATION && diagReq->packLength == 0x00U){        /* Save Configuration */
      /*
      4.2.5.4 Save Configuration
      This service tells the slave node(s) that the slave application shall save the current
      configuration. The save configuration request shall be structured as shown in
      Table 4.14. This service is used to notify a slave node to store its configuration. 
      A configuration in the slave node may be valid even without the master node using this
      request (i.e. the slave node does not have to wait for this request to have a valid configuration).
      */
      diagRsp->packLength = 0U;
      response = TRUE;
      TM_PostTask(TASK_ID_PDS); /*start persistance data storage (PDS )task */
  }else if (diagReq->sid == LIN_SID_READ_BY_ID && diagReq->packLength == sizeof(ReadByIdReq_t)){                /* Read by Identifier */
      ReadByIdReq_t *readById = (ReadByIdReq_t *)((void*)diagReq->payload);
      if ((readById->supplierID == LINS_SUPPLIER_ID || readById->supplierID == LIN_SUPPLIER_ID_WILDCARD)&&
          (readById->functionID == LINS_FUNCTION_ID || readById->functionID == LIN_FUNCTION_ID_WILDCARD)){
          /*
          4.2.6.1 Read by identifier
          It is possible to read the supplier identity and other properties from a slave node using
          the request in Table 4.18.
          */
          if (readById->identifier == 0U){
              /* LIN Product Identification */
              ReadByIdProductIdRsp_t *rsp = (ReadByIdProductIdRsp_t *)((void *)diagRsp->payload);
              rsp->supplierID = LINS_SUPPLIER_ID;
              rsp->functionID = LINS_FUNCTION_ID;
              rsp->variant = LINS_VARIANT;
              diagRsp->packLength = (uint16_t)sizeof(ReadByIdProductIdRsp_t);
              response = TRUE;
          }else if (readById->identifier == 1U){
              /* Serial number */
              ReadByIdSerialNoRsp_t *rsp = (ReadByIdSerialNoRsp_t *)((void *)diagRsp->payload);
              rsp->serialNo = LINS_SERIAL_NO;
              diagRsp->packLength = (uint16_t)sizeof(ReadByIdSerialNoRsp_t);
              response = TRUE;
          }else{
              /*
              If the slave is not supporting this request or could not process the request it will
              respond according to Table 4.21.
              */
              ReadByIdNakRsp_t *rsp = (ReadByIdNakRsp_t *)((void *)diagRsp->payload);
              diagRsp->sid = 0x7FU;
              rsp->sid = LIN_SID_READ_BY_ID;
              rsp->errorCode = 0x12U;
              diagRsp->packLength = (uint16_t)sizeof(ReadByIdNakRsp_t);
              response = TRUE;
          }
      }
  }else if (diagReq->sid == LIN_SID_ASSIGN_FRAME_ID_RANGE && diagReq->packLength == sizeof(AssignFrameIDRange_t)){     /* Assign frame identifier range */
      AssignFrameIDRange_t *assignFrameIDRng = (AssignFrameIDRange_t *)((void *)diagReq->payload);
      uint8_t pid;
      uint8_t invalid = 0U;
      /*
      4.2.5.5 Assign frame ID range
      It is important to notice that the request provides the protected identifier, i.e. the frame
      identifier and its parity. Furthermore, frames with frame identifiers 60 (0x3C) to 63
      (0x3F) can not be changed (diagnostic frames and reserved frames)
      */
      for (uint8_t i = 0U; i< 4U;i++){
          if (assignFrameIDRng->pid[i] != LIN_FRAME_ID_DONOTCARE){
              pid = assignFrameIDRng->pid[i] & 0x3FU;
              if (pid >= 0x3CU && pid <= 0x3FU){
                  /* the request is fail for the FID is invalid*/
                  invalid = 1U;
                  break;
              }
          }
      }
      /* PID is not in the range from 0x3C to 0x3D */
      if (invalid == 0U){
          uint8_t tableItemsCount;
          LIN_Device_Frame_t *unconditionalTable = LINS_GetUcndFramesTable(&tableItemsCount);
          /*  startIndex should be less then unconditional Table size  */
          if (assignFrameIDRng->startIndex < tableItemsCount){
              /*
              4.2.5.5 Assign frame ID range
              Example 2
              A slave node has only two frames {status_frame, response_frame}. To assign PIDs to
              these two frames the master application will setup the following request: start index
              set to 0 and PID (index 0..3) set to {0xC4, 0x85, 0xFF, 0xFF}. Since the slave node
              has only two frames the last two must be set to do not care, otherwise the request will fail.
              */
              uint8_t maxConfigurableFrameIndex = assignFrameIDRng->startIndex + 4U;
              if (maxConfigurableFrameIndex > tableItemsCount){/*  overlap with frame table  */
                  uint8_t coutOffset = 4U - (maxConfigurableFrameIndex - tableItemsCount);
                  for (uint8_t i = coutOffset; i < 4U;i++){
                      /*  configurable out of range PID should be 0xFF  */
                      if (assignFrameIDRng->pid[i] != LIN_FRAME_ID_DONOTCARE){
                          /* the request is fail */
                          invalid = 1U;
                          break;
                      }
                  }
                  if (invalid == 0U){
                      /*  Configurable out of range PID setting is corrent, copy PID to frame table */
                      for (uint8_t i = 0U; i< (tableItemsCount - assignFrameIDRng->startIndex);i++){
                          if (assignFrameIDRng->pid[i] == LIN_FRAME_ID_UNASSIGN){
                              /*  set PID to 0xFF to invalid this frame */
                              unconditionalTable[assignFrameIDRng->startIndex + i].frameIsValid = FALSE;
                          }else{
                              if (assignFrameIDRng->pid[i] != LIN_FRAME_ID_DONOTCARE){
                                  /*
                                  4.2.5.5 Assign frame ID range:
                                  The slave node will not validate the given PIDs (i.e. validating the parity flags), the
                                  slave node relies on that the master sets the correct PIDs.
                                  copy PID to frame table:
                                  */
                                  unconditionalTable[assignFrameIDRng->startIndex + i].frame_id = assignFrameIDRng->pid[i] & 0x3FU;
                                  unconditionalTable[assignFrameIDRng->startIndex + i].frameIsValid = TRUE;
                              }
                          }
                      }
                      diagRsp->packLength = 0U;
                      response = TRUE;
                  }
              }else{/*  no overlap frame table  */
                  for (uint8_t i = 0U; i < 4U;i++){
                      if (assignFrameIDRng->pid[i] == LIN_FRAME_ID_UNASSIGN){
                          /*  set PID to 0xFF to invalid this frame */
                          unconditionalTable[assignFrameIDRng->startIndex + i].frameIsValid = FALSE;
                      }else{
                           if (assignFrameIDRng->pid[i] != LIN_FRAME_ID_DONOTCARE){
                              /*
                              4.2.5.5 Assign frame ID range:
                              The slave node will not validate the given PIDs (i.e. validating the parity flags), the
                              slave node relies on that the master sets the correct PIDs.
                              copy PID to frame table:
                              */
                              unconditionalTable[assignFrameIDRng->startIndex + i].frame_id = assignFrameIDRng->pid[i] & 0x3FU;
                              unconditionalTable[assignFrameIDRng->startIndex + i].frameIsValid = TRUE;
                           }
                      }
                  }
                  for (uint8_t i = 0U; i < tableItemsCount; i++){
                      linParams->frameInfo[i].frameId      = frameTable[i].frame_id;
                      linParams->frameInfo[i].frameIsValid = frameTable[i].frameIsValid;
                  }
                  diagRsp->packLength = 0U;
                  response = TRUE;
              }
          }
      }
  }
  return response;
}
#endif