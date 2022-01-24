//*****************************************************************************
//
// This header file the prototypes for the 23LC1024 SRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifdef _23LC1024_H
#else
#define _23LC1024_H

extern void SRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset);
extern void SRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset);

// SRAM Command Codes
#define SRAM_NUMBER 4
#define SRAM_READ 0x03
#define SRAM_WRITE 0x02
#define SRAM_RDMR 0x05
#define SRAM_WRMR 0x01
#define SRAM_SEQUENTIAL_CODE 0x40

// SRAM Size
#define SRAM_SIZE_BYTES 125000

#endif