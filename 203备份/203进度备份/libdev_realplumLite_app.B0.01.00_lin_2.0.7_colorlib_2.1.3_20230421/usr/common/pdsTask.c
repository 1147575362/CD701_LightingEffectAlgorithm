/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file pdsTask.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <pdsTask.h>
#include <crc32.h>
#include <realplumLite.h>
#include <flash_device.h>
#include <linsNodeCfgIdentify.h>
#include <linStackTask.h>
#include <linSlaveTask.h>

extern void CLM_Init(void);

uint8_t pds_store(uint8_t index);

static SystemParams_t systemParam;
static TaskState_t pdsState = TASK_STATE_INIT;
static PdsStatus_t pdsStatus = PDS_STATUS_USING_NORMAL;

#define PDS_ACTIVE_DATA_INDEX           (0U)
#define PDS_ACTIVE_DATA_BACKUP_INDEX    (1U)

void ASSIGN_FID(uint8_t FID)
{
	systemParam.info.linParams.fid = FID;
    systemParam.info.linParams.nad = GET_ALU_INFO_FID(FID);
	TM_PostTask(TASK_ID_PDS);

}

uint8_t GET_ALU_FID(void)
{
	return (uint8_t)(systemParam.info.linParams.fid);
}


void PDS_Init(void)
{
    uint32_t crcValue;
    CRC32_Init();
    
    SystemParams_t *param = (SystemParams_t *)FLASH_START_ADDRESS_SYSTEM_DATA;
    crcValue = CRC32_GetRunTimeCRC32((uint8_t *)&param->info ,(uint16_t)sizeof(param->info));
    if (crcValue == param->head.crc32){
        systemParam = *param;
        pdsStatus = PDS_STATUS_USING_NORMAL;
    }else{
        param = (SystemParams_t *)FLASH_START_ADDRESS_SYSTEM_DATA_BACK_UP;
        crcValue = CRC32_GetRunTimeCRC32((uint8_t *)&param->info ,(uint16_t)sizeof(param->info));
        if (crcValue == param->head.crc32){
            systemParam = *param;
            pdsStatus = PDS_STATUS_USING_BACKUP;
        }else{
            pdsStatus = PDS_STATUS_USING_DEFAULT;
        }
    }
    if (pdsStatus == PDS_STATUS_USING_DEFAULT){
/*  **********************fix LIN configuration default value ************************************/
        systemParam.head.index = 0U;
        systemParam.info.linParams.nad          = INIT_LINS_NAD_ADDR;
        systemParam.info.linParams.supplierID   = LINS_SUPPLIER_ID;
        systemParam.info.linParams.functionID   = LINS_FUNCTION_ID;
        systemParam.info.linParams.variant      = LINS_VARIANT;
        systemParam.info.linParams.serialNo     = LINS_SERIAL_NO;
        systemParam.info.linParams.fwVersion    = LINS_FW_VERSION;
        systemParam.info.linParams.hwVersion    = LINS_HW_VERSION;
        
        LIN_Device_Frame_t *frame = LINS_GetUcndFramesTable(&systemParam.info.linParams.frameIDSize);
        for (uint8_t i = 0; i < systemParam.info.linParams.frameIDSize; i++){
            systemParam.info.linParams.frameInfo[i].frameId      = frame[i].frame_id;
            systemParam.info.linParams.frameInfo[i].frameIsValid = TRUE;
        }
/*  *************************fix color mixing  configuration default value********************* */
        systemParam.info.ledParams.whitePoint.x                 = DEFAULT_COORDINATE_WHITE_POINT_X;
        systemParam.info.ledParams.whitePoint.y                 = DEFAULT_COORDINATE_WHITE_POINT_Y;
        systemParam.info.ledParams.minIntensity[LED_RED]        = DEFAULT_MIN_INTENSITY_R;
        systemParam.info.ledParams.minIntensity[LED_GREEN]      = DEFAULT_MIN_INTENSITY_G;
        systemParam.info.ledParams.minIntensity[LED_BLUE]       = DEFAULT_MIN_INTENSITY_B;
        for (uint8_t i = 0U; i < CFL_LED_NUM; i++){
            systemParam.info.ledParams.led[i].maxTypicalIntensity[LED_RED]      = DEFAULT_MAX_INTENSITY_R;
            systemParam.info.ledParams.led[i].maxTypicalIntensity[LED_GREEN]    = DEFAULT_MAX_INTENSITY_G;
            systemParam.info.ledParams.led[i].maxTypicalIntensity[LED_BLUE]     = DEFAULT_MAX_INTENSITY_B;
            systemParam.info.ledParams.led[i].color[LED_RED].x                  = DEFAULT_COORDINATE_R_X;
            systemParam.info.ledParams.led[i].color[LED_RED].y                  = DEFAULT_COORDINATE_R_Y;
            systemParam.info.ledParams.led[i].color[LED_GREEN].x                = DEFAULT_COORDINATE_G_X;
            systemParam.info.ledParams.led[i].color[LED_GREEN].y                = DEFAULT_COORDINATE_G_Y;
            systemParam.info.ledParams.led[i].color[LED_BLUE].x                 = DEFAULT_COORDINATE_B_X;
            systemParam.info.ledParams.led[i].color[LED_BLUE].y                 = DEFAULT_COORDINATE_B_Y;
            systemParam.info.ledParams.led[i].ledTypicalPNVolt[LED_RED]         = DEFAULT_LED_PN_VOLT_R;
            systemParam.info.ledParams.led[i].ledTypicalPNVolt[LED_GREEN]       = DEFAULT_LED_PN_VOLT_G;
            systemParam.info.ledParams.led[i].ledTypicalPNVolt[LED_BLUE]        = DEFAULT_LED_PN_VOLT_B;
        }
    }
    ls_set_initialNad(INIT_LINS_NAD_ADDR);
    ls_set_nad(systemParam.info.linParams.nad);
    /* must be call here for init LIN param */
    LNCI_Init();
    /* must be call here for init led param */
    CLM_Init();
}


uint8_t pds_store(uint8_t index)
{
/****************************************************************************************************************/
    uint16_t wordLength;
    uint32_t *pWord = (uint32_t *)((void *)&systemParam);
    uint32_t crcValue = CRC32_GetRunTimeCRC32((uint8_t *)(&systemParam.info) ,(uint16_t)sizeof(systemParam.info));
    systemParam.head.crc32 = crcValue;
    systemParam.head.index ++;
    
    /*  sizeof(SystemParams_t) is the timers of word     */
    wordLength = ((uint16_t)sizeof(SystemParams_t) >> 2U);

    pdsStatus = PDS_STATUS_USING_NORMAL;
    
    if (index == PDS_ACTIVE_DATA_INDEX){
        /*write sys data normal*/
        Flash_EraseSector(FLASH_START_ADDRESS_SYSTEM_DATA);
        for (uint32_t i = 0; i < wordLength; i++){
            Flash_WriteWord(FLASH_START_ADDRESS_SYSTEM_DATA+i*4U,pWord[i]);
        }
    }else{
        /*write sys data back up*/
        Flash_EraseSector(FLASH_START_ADDRESS_SYSTEM_DATA_BACK_UP);
        for (uint32_t i = 0; i < wordLength; i++){
            Flash_WriteWord(FLASH_START_ADDRESS_SYSTEM_DATA_BACK_UP+i*4U,pWord[i]);
        }
    }
    return 0U;
}



LedClusterParams_t *PDS_GetLedParam(void)
{
    return &systemParam.info.ledParams;
}


LINClusterParams_t *PDS_GetLINParam(void)
{
    return &systemParam.info.linParams;
}

PdsStatus_t PDS_GetPdsStatus(void)
{
    return pdsStatus;
}


void PDS_TaskHandler(void)
{
    switch(pdsState){
    case TASK_STATE_INIT:
      pdsState = TASK_STATE_ACTIVE;
      break;
    case TASK_STATE_ACTIVE:
      /* storage system parameters */
      (void)pds_store(PDS_ACTIVE_DATA_INDEX);
      pdsState = TASK_STATE_PROCESSING;
      TM_PostTask(TASK_ID_PDS);
      break;
    case TASK_STATE_PROCESSING:
      /* storage system parameters backup*/
      (void)pds_store(PDS_ACTIVE_DATA_BACKUP_INDEX);
      /* set save parameters flag for lin stack*/
      (void)ls_save_configuration();
      pdsState = TASK_STATE_ACTIVE;
      break;
    default:
      break;
    }
}















