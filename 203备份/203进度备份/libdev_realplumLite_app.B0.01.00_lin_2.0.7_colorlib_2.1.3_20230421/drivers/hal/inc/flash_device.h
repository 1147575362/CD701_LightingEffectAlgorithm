/**
 * @copyright 2017 Indie Semiconductor.
 *
 * This file is proprietary to Indie Semiconductor.
 * All rights reserved. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of Indie Semiconductor.
 *
 * @file flash_device.h
 */

#ifndef FLASH_DEVICE_H__
#define FLASH_DEVICE_H__

#include <stdint.h>

/**
 * @brief Erase a sector flash memory and wait for completion
 *
 * @param addr: starting address of the block
 * @return none
 */
void Flash_EraseSector(uint32_t sectorAddress);

/**
 * @brief write flash memory and wait for completion
 *
 * @param addr: starting address of the data
 * @return none
 */
void Flash_WriteWord(uint32_t address,uint32_t data);
/**
 * @brief Copy a data block from RAM to flash memory.
 *
 * @param destAddress: destination address in flash memory
 * @param buff: source address in RAM
 * @param length: size of data in bytes
 * @return none
 */
int8_t Flash_devCopyToFlash(uint32_t destAddress, uint8_t *const buff, uint16_t length);

#endif /* __FLASH_DEVICE_H__ */