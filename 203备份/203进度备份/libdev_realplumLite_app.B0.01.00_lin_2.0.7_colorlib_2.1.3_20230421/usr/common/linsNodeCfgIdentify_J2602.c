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

#if LIN_STACK_TYPE == LIN_STACK_TYPE_SAEJ2602

static LINClusterParams_t *linParams  = NULL;
static LIN_Device_Frame_t *frameTable = NULL;

void HardwareResetTimerExpired(SoftTimer_t *timer);
/*
static SoftTimer_t hardwareResetTimer = {
    .mode     = TIMER_ONE_SHOT_MODE,
    .interval = 50U,
    .handler  = HardwareResetTimerExpired
};
*/
void HardwareResetTimerExpired(SoftTimer_t *timer)
{
    CRGA_REG_RESETCTRL.HARDRSTREQ = 1U;
}

void LNCI_Init(void)
{
    uint8_t frameIsValid = TRUE;
    uint8_t tableItemsCount;
    /* get lin information from flash storage */
    linParams = PDS_GetLINParam();
    /*  get frame table information   */
    frameTable = LINS_GetUcndFramesTable(&tableItemsCount);
    
    /*
    5.7.2.2 Message ID Assignment
    Devices with a DNN of $E or $F will have the protected identifiers marked as 
    invalid as described in of the LIN Diagnostic and Configuration Specification
    */
    if (linParams->nad >= 0x6EU && linParams->nad <= 0x6FU){
        frameIsValid = FALSE;
    }
    for (uint8_t i = 0U; i < tableItemsCount; i++){
        frameTable[i].frame_id     = i + (linParams->nad & 0x0FU)*4U;
        frameTable[i].frameIsValid = frameIsValid;
    }
}

uint8_t LNCI_HandleNodeCfgIdentifyRequest(const DiagReqInfo_t *const diagReq,DiagRspInfo_t *const diagRsp)
{
  uint8_t frameIsValid = TRUE;
  uint8_t response = FALSE;
  if (diagReq->sid == LIN_SID_J2602_HARDWARE_RST && diagReq->packLength == 0U){        /* 0xB5 is hardware reset command in SAEJ2602 */
      /* LIN Product Identification */
      ReadByIdProductIdRsp_t *rsp = (ReadByIdProductIdRsp_t *)((void *)diagRsp->payload);
      rsp->supplierID = LINS_SUPPLIER_ID;
      rsp->functionID = LINS_FUNCTION_ID;
      rsp->variant = LINS_VARIANT;
      diagRsp->packLength = (uint16_t)sizeof(ReadByIdProductIdRsp_t);
      
      if(linParams->nad == ls_read_nad_from_master()){
          response = TRUE;
      }
      ls_set_J2602_hardware_reset(TRUE);
      LINS_SetDeviceUnconfigured();
      //SoftTimer_Stop(&hardwareResetTimer);
      //SoftTimer_Start(&hardwareResetTimer);
  }else if (diagReq->sid == LIN_SID_ASSIGN_NAD && diagReq->packLength == sizeof(AssignNAD_t)){/* Assign NAD */
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
              if (assignNAD->newNAD >= 0x60U && assignNAD->newNAD <= 0x6FU){
                  if ( (((assignNAD->newNAD & 0x0FU)%(MESSAGE_ID_BLOCK_COUNT/4U) ) == 0U) || (assignNAD->newNAD == 0x6FU)  ){
                      /*
                      5.7.2.2 Message ID Assignment
                      Devices with a DNN of $E or $F will have the protected identifiers marked as 
                      invalid as described in of the LIN Diagnostic and Configuration Specification
                      */
#if MESSAGE_ID_BLOCK_COUNT == 8U
                      if (assignNAD->newNAD >= 0x6DU && assignNAD->newNAD <= 0x6FU){
                          frameIsValid = FALSE;
                      }
#else
                      if (assignNAD->newNAD >= 0x6EU && assignNAD->newNAD <= 0x6FU){
                          frameIsValid = FALSE;
                      }
#endif
                      diagRsp->packLength = 0U;
                      linParams->nad = assignNAD->newNAD;
                      ls_set_nad(assignNAD->newNAD); /* set nad to lin stack */
                      /*  adatper Frame ID according to NAD   */
                      uint8_t tableItemsCount;
                      frameTable = LINS_GetUcndFramesTable(&tableItemsCount);
                      linParams->frameIDSize = tableItemsCount;
                    
                      for (uint8_t i = 0U; i < tableItemsCount; i++){
                          frameTable[i].frame_id     = i + (linParams->nad & 0x0FU)*4U;
                          frameTable[i].frameIsValid = frameIsValid;
                          linParams->frameInfo[i].frameId = frameTable[i].frame_id;
                          linParams->frameInfo[i].frameIsValid = frameIsValid;
                      }
                      response = TRUE;
                      /*TM_PostTask(TASK_ID_PDS);  start persistance data storage (PDS )task */
                  }
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
      TM_PostTask(TASK_ID_PDS);  /*start persistance data storage (PDS )task */
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
  }
  return response;
}

#endif