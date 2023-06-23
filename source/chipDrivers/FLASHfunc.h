/*
 * FLASHfunc.h
 *
 *  Created on: Jun 16, 2023
 *      Author: tythu
 */

#ifndef FLASHFUNC_H_
#define FLASHFUNC_H_

// Function Prototypes
void FlashErase(uint8_t chip_number);
void FlashSequenceTransmit(uint8_t currentCycle, uint32_t chip_number);
void FlashSequenceRetrieve(uint8_t currentCycle, uint32_t chip_number);
void FlashStatusRead(uint8_t chip_number);

// Flash Commands
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_WRITE 0x12
#define FLASH_CHIP_ERASE 0x60
#define FLASH_READ 0x03
#define FLASH_RDSR 0x05
#define FLASH_IDENT 0x9F

// Device ID information
#define FLASH_MANUFACTURER 0x01
#define FLASH_DEVICEID_1 0x60
#define FLASH_DEVICEID_2 0x19

#define FLASH_SIZE_BYTES 256
//#define FLASH_SIZE_BYTES 256000000
#define FLASH_PAGE_SIZE 256

#endif /* FLASHFUNC_H_ */
