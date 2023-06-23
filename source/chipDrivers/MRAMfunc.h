//*****************************************************************************
//
// This is the header file is for the MR25H40 MRAM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef MRAMFUNC_H
#define MRAMFUNC_H

void MRAMSequenceTransmit(uint8_t currentCycle, uint32_t chip_number);
void MRAMSequenceRetrieve(uint8_t currentCycle, uint32_t chip_number);
void MRAMStatusRead(uint8_t chip_number);
void MRAMStatusPrepare(void);

// MRAM Command Codes
#define MRAM_READ 0x03
#define MRAM_WRITE 0x02
#define MRAM_WRITE_ENABLE 0x06
#define MRAM_WRITE_DISABLE 0x04
#define MRAM_WRSR 0x01
#define MRAM_RDSR_READ 0x05
// MRAM Size
//#define MRAM_SIZE_BYTES 524288
#define MRAM_SIZE_BYTES 10

#endif /* MRAMFUNC_H_ */
