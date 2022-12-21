//*****************************************************************************
//
// This is the header file is for the MR25H40 MRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef MR25H40_H
#define MR25H40_H

extern void MRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin);
extern void MRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin, uint32_t chip_number);

// MRAM Command Codes
#define MRAM_READ 0x03
#define MRAM_WRITE 0x02
#define MRAM_WRITE_ENABLE 0x06
#define MRAM_WRITE_DISABLE 0x04
#define MRAM_WRSR 0x01

// MRAM Size
#define MRAM_SIZE_BYTES 524288

#endif