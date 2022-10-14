//*****************************************************************************
//
// This is the header file is for the IS62WVS5128GBLL SRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef IS62WVS5128GBLL_H
#define IS62WVS5128GBLL_H

extern void SRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin);
extern void SRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin, uint32_t chip_number);

// SRAM Command Codes
#define SRAM_READ 0x03
#define SRAM_WRITE 0x02
#define SRAM_RDMR 0x05
#define SRAM_WRMR 0x01
#define SRAM_SEQUENTIAL_CODE 0x40

// SRAM Size
#define SRAM_SIZE_BYTES 512000

#endif