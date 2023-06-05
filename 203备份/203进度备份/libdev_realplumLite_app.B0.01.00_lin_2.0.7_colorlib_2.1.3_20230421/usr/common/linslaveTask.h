#ifndef _LINS_TASK_H
#define _LINS_TASK_H

#include <appConfig.h>
#include <lin_device.h>


/**
* @brief A structure to represent LIN frame information.
*/

#define N_AS            (1000)
#define N_CR            (1000)

#define LINS_FUNCTION_ID_SNPD_FORCE_SWITCH_ON           (0xAA00U)
#define LINS_FUNCTION_ID_SNPD_INIT                      (0xAA01U)
#define LINS_FUNCTION_ID_SNPD_ASSIGN_NAD                (0xAA02U)
#define LINS_FUNCTION_ID_SNPD_STORAGE                   (0xAA03U)
#define LINS_FUNCTION_ID_SNPD_EXIT                      (0xAA04U)

#define LINS_SUPPLIER_ID                                (0x0123U)
#define LINS_FUNCTION_ID                                (0x0123U)
#define LINS_VARIANT                                    (0x01U)
#define LINS_SERIAL_NO                                  (0x01234567U)
#define LINS_FW_VERSION                                 (0x0123U)
#define LINS_HW_VERSION                                 (0x0001U)

#define LIN_SID_APP_DIAG_LED_GET_CONFIG                 (0xBAU)   /* single pdu receive, multi pdu response */
#define LIN_SID_APP_DIAG_LED_SET_CONFIG                 (0xBBU)   /* multi pdu receive, single pdu response */
#define LIN_SID_APP_DIAG_SINGLE_PDU_RECV_MULTI_RSP      (0x22U)   
#define LIN_SID_APP_DIAG_MULTI_PDU_RECV_SINGLE_RSP      (0x2EU)
#define LIN_SID_APP_DIAG_MULTI_PDU_RECV_MULTI_RSP       (0x2FU)


#define LIN_SID_APP_DIAG_BOOT_MODE                      (0x20U)
#define BOOT_MODE_HANDSHAKE                             (0x00U)
#define BOOT_MODE_MAGIC_KEY_HANDSHAKE                   (0x5A5A55AAU)

/*  *****************LIN configuration default value **************************/
#if LIN_STACK_TYPE == LIN_STACK_TYPE_SAEJ2602
    #define INIT_LINS_NAD_ADDR              (0x6FU) /* NAD range is 0x60-0x6D,defalut*/
    #if (INIT_LINS_NAD_ADDR>0x6FU)
        #error INIT_LINS_NAD_ADDR must be within 0x60U-0x6FU!!!
    #endif

    #define MESSAGE_ID_BLOCK_COUNT           (0x08U)       /* MESSAGE_ID_BLOCK_COUNT = 2^n n= 2,3,4..  */
    #if (MESSAGE_ID_BLOCK_COUNT != 4U && MESSAGE_ID_BLOCK_COUNT != 8U && MESSAGE_ID_BLOCK_COUNT != 16U)
        #error Invalid MESSAGE_ID_BLOCK_COUNT count !!!
    #elif ((INIT_LINS_NAD_ADDR == 0x6CU || INIT_LINS_NAD_ADDR == 0x6DU) && MESSAGE_ID_BLOCK_COUNT != 4U)
        #error MESSAGE_ID_BLOCK_COUNT must be 4 when NAD is 0x6C or 0x6D!!!
    #endif

    /* it's only a reference, the real FID number is based on NAD according to SAEJ2602  */
    #define DEFAULT_LINS_FID0             (0x00U)
    #define DEFAULT_LINS_FID1             (0x01U)
    #define DEFAULT_LINS_FID2             (0x02U)
    #define DEFAULT_LINS_FID3             (0x03U)
    #define DEFAULT_LINS_FID4             (0x04U)
    #define DEFAULT_LINS_FID5             (0x05U)
    #define DEFAULT_LINS_FID6             (0x06U)
    #define DEFAULT_LINS_FID7             (0x07U)

#else

    #define INIT_LINS_NAD_ADDR              (0x01U)
    #define DEFAULT_LINS_FID_SIZE           (0x08U)       /* count number defined by user */

    /*  FID number is set by user  */
    #define DEFAULT_LINS_FID0               (0x20U)
    #define DEFAULT_LINS_FID1               (0x21U)
    #define DEFAULT_LINS_FID2               (0x22U)
    #define DEFAULT_LINS_FID3               (0x23U)

	#define YZ_CHANGE_FID_RESPOND 			(0x3EU)
	
	#define DEFAULT_LINS_LEFT 				(0x03U)
	#define DEFAULT_LINS_RIGHT				(0x04U)
	
    void LINS_EventTriggered_ISR(void);
#endif



void LINS_TaskHandler(void);
LIN_Device_Frame_t *LINS_GetUcndFramesTable(uint8_t *tableItemsCount);
void LINS_SendWakeUpSignal_ISR(void);
uint8_t LINS_GetPIDFromFID(uint8_t frameId);
void LINS_SetDeviceUnconfigured(void);

#endif