#ifndef CRC32_H_
#define CRC32_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


void CRC32_Init(void);
uint32_t CRC32_GetRunTimeCRC32(uint8_t *pBuf ,uint16_t length);















#endif