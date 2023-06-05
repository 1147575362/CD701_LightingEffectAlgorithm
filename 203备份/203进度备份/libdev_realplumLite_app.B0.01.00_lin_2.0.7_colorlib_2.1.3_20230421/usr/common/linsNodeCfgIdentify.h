#ifndef LINS_NODE_CFG_IDENTIFY_H__
#define LINS_NODE_CFG_IDENTIFY_H__
#include <linStackTask.h>

BEGIN_PACK

typedef struct{
    uint8_t  initNAD;
    uint8_t  pci;
    uint8_t  sid;
}nodeConfig_t;

typedef struct{
    uint16_t supplierID;
    uint16_t functionID;
    uint8_t  newNAD;
}AssignNAD_t;

typedef struct{
    uint16_t supplierID;
    uint16_t functionID;
    uint8_t  newNAD;
}SNPD_t;

typedef struct{
    uint8_t startIndex;
    uint8_t pid[4];
}AssignFrameIDRange_t;

typedef struct{
    uint8_t command;
    uint32_t magicKey;
}EnterBootloaderFrame_t;

typedef struct{
    uint8_t  identifier;
    uint16_t supplierID;
    uint16_t functionID;
}ReadByIdReq_t;

typedef struct{
    uint8_t  sid;
    uint8_t  errorCode;
}ReadByIdNakRsp_t;

typedef struct{
    uint16_t supplierID;
    uint16_t functionID;
    uint8_t  variant;
}ReadByIdProductIdRsp_t;

typedef struct{
    uint32_t serialNo;
}ReadByIdSerialNoRsp_t;

typedef struct{
    uint8_t nad;
    uint32_t magicKey;
}SaeJ2602_3C3ECfg_t;

END_PACK



void LNCI_Init(void);
uint8_t LNCI_HandleNodeCfgIdentifyRequest(const DiagReqInfo_t *const diagReq,DiagRspInfo_t *const diagRsp);

#endif