//*****************************************************************************
//
// This header file the prototypes for the SST25VF010A Flash.
//
//*****************************************************************************

// Make sure this file is only included once
#ifdef SST25VF010A_H
#else
#define SST25VF010A_H

extern void FlashSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t SysClkSpeed);
extern void FlashSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset);

// FLASH Command Codes
#define FLASH_NUMBER 1 
#define FLASH_READ 0x03
#define FLASH_WRITE 0xAF
#define FLASH_CHIP_ERASE 0x60
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_WRSR 0x01
#define FLASH_EWSR 0x50
#define FLASH_RDSR 0x05

// FLASH write and erase max times
#define FLASH_CHIP_ERASE_TIME_MS 100
#define FLASH_WRITE_TIME_US 20

// FLASH size
#define FLASH_SIZE_BYTES 125000

#endif