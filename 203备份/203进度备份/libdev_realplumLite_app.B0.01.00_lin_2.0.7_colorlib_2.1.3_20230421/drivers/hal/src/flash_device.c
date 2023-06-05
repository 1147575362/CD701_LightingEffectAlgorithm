/**
 * @copyright 2015 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file flash_device.c
 * @Author: Jack.Pan
 * @E-mail:jack.pan@indiemicro.com
 * @Date: 2020/09/10
 */

#include <stdint.h>
#include <string.h>
#include <flash_device.h>
#include <flash_sfrs.h>

typedef union{
  uint32_t payload;
}dataStruct_t;

static uint8_t flashBuff[FLASH_SECTOR_SIZE];

/* address + length can't overlap section address */
int8_t Flash_devCopyToFlash(uint32_t address, uint8_t *const buff, uint16_t length)
{
    int8_t result = -1;
    uint32_t sectorAddr = (address >> FLASH_SECTOR_SIZE_POS) << FLASH_SECTOR_SIZE_POS;
    uint32_t offsetAddr = address - sectorAddr;
    uint32_t addressInSector = offsetAddr + length;
    if ( (length != 0U) && (address >= FLASH_START_ADDRESS_USER_DATA) && ((address + length) <= FLASH_FINAL_ADDRESS) && (addressInSector <= FLASH_SECTOR_SIZE) ){
        for (uint32_t i = 0U; i < offsetAddr; i++){
            flashBuff[i] = ((uint8_t *)sectorAddr)[i];
        }
        
        for (uint32_t i = offsetAddr; i < addressInSector; i++){
            flashBuff[i] = buff[i - offsetAddr];
        }
        
        for (uint32_t i = addressInSector; i < FLASH_SECTOR_SIZE; i++){
            flashBuff[i] = ((uint8_t *)sectorAddr)[i];
        }
        
        f_FLASH_EraseSector(sectorAddr);
        
        uint32_t *pWord = (uint32_t *)((void *)flashBuff);
        for (uint32_t i = 0U; i < (FLASH_SECTOR_SIZE / 4U); i++){
            f_FLASH_WriteWord(sectorAddr + (i << 2U), pWord[i]);
        }
        result = 0;
    }
    return result;
}


void Flash_EraseSector(uint32_t sectorAddress)
{
    /* Erase sector data */
    f_FLASH_EraseSector(sectorAddress);
}

void Flash_WriteWord(uint32_t address,uint32_t data)
{
    f_FLASH_WriteWord(address, data);
}








