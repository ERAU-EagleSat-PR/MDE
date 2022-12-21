//*****************************************************************************
//
// This is the header file is for the MB85RS2MTYPNF FRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef MB85RS2MTYPNF
#define MB85RS2MTYPNF

extern void FRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin);
extern void FRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin, uint32_t chip_number);

// FRAM Command Codes
#define FRAM_READ 0x03
#define FRAM_WRITE 0x02
#define FRAM_FAST_READ 0x0B
#define FRAM_WRITE_ENABLE 0x06
#define FRAM_WRITE_DISABLE 0x04


// FRAM Size
#define FRAM_SIZE_BYTES 262144

#endif