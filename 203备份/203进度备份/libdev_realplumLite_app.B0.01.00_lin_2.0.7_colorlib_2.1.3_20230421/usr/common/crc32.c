#include <crc32.h>

static uint32_t CRC32_Table[256U];


void CRC32_Init(void)
{
    uint32_t c;

    for (uint32_t i = 0U; i < 256U; i++) {
        c = i;
        for (uint32_t j = 0U; j < 8U; j++) {
            if ((c & 0x01U)!= 0U){
                  c = 0xEDB88320UL ^ (c >> 1U);
            }else{
                  c = c >> 1U;
            }
        }
        CRC32_Table[i] = c;
    }
}


uint32_t CRC32_GetRunTimeCRC32(uint8_t *pBuf ,uint16_t length)
{
    uint32_t retCRCValue = 0xFFFFFFFFU;
    for (uint16_t i = 0; i < length; i++){
        retCRCValue=CRC32_Table[(retCRCValue ^ pBuf[i]) & 0xFFU] ^ (retCRCValue >> 8U);
    }
    return (retCRCValue^0xFFFFFFFFU);
}



