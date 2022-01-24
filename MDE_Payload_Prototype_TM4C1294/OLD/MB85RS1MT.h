//*****************************************************************************
//
// This header file the prototypes for the MB85RS1MT_H FRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifdef MB85RS1MT_H
#else
#define MB85RS1MT_H

extern void FRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset);
extern void FRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset);

// FRAM Command Codes
#define FRAM_NUMBER 2 
#define FRAM_READ 0x03
#define FRAM_WRITE 0x02
#define FRAM_WRITE_ENABLE 0x06
#define FRAM_WRITE_DISABLE 0x04

// FRAM Size
#define FRAM_SIZE_BYTES 125000

#endif