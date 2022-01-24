//*****************************************************************************
//
// This header file the prototypes for the MR25H10 MRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifdef MR25H10_H
#else
#define MR25H10_H

extern void MRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset);
extern void MRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset);

// MRAM Command Codes
#define MRAM_NUMBER 3 
#define MRAM_READ 0x03
#define MRAM_WRITE 0x02
#define MRAM_WRITE_ENABLE 0x06
#define MRAM_WRITE_DISABLE 0x04
#define MRAM_WRSR 0x01

// MRAM Size
#define MRAM_SIZE_BYTES 125000

#endif