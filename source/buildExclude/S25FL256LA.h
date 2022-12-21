//*****************************************************************************
//
// This is the header file for the S25FL256LA Flash.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef S25FL256LA_H
#define S25FL256LA_H

extern void FlashSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin);
extern void FlashSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin, uint32_t chip_number);

// FLASH Command Codes
#define FLASH_READ 0x13
#define FLASH_WRITE 0x12
#define FLASH_CHIP_ERASE 0x60
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_WRITE_DISABLE 0x04
#define FLASH_WRSR 0x01
#define FLASH_EWSR 0x50
#define FLASH_RDSR 0x05

// FLASH size
#define FLASH_SIZE_BYTES 32000000
#define FLASH_PAGE_SIZE 256

#endif