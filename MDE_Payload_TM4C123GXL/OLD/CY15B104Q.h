//*****************************************************************************
//
// This is the header file is for the CY15B104Q FRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef CY15B104Q_H
#define CY15B104Q_H

extern void FRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin);
extern void FRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin, uint32_t chip_number);

// FRAM Command Codes
#define FRAM_READ 0x03
#define FRAM_WRITE 0x02
#define FRAM_WRITE_ENABLE 0x06
#define FRAM_WRITE_DISABLE 0x04

// FRAM Size
#define FRAM_SIZE_BYTES 512000

#endif