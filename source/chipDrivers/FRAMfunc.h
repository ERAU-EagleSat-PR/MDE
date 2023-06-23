//*****************************************************************************
//
// This is the header file is for the MB85RS2MTYPNF FRAM.
//
//*****************************************************************************

#ifndef FRAMFUNC_H
#define FRAMFUNC_H

void FRAMSequenceTransmit(uint8_t currentCycle, uint32_t chip_number);
void FRAMSequenceRetrieve(uint8_t currentCycle, uint32_t chip_number);
void FRAMStatusRead(uint8_t chip_number);

// FRAM Command Codes
#define FRAM_READ 0x03
#define FRAM_WRITE 0x02
#define FRAM_IDENT 0x9F
#define FRAM_WRITE_ENABLE 0x06
#define FRAM_WRITE_DISABLE 0x04


// FRAM Size
//#define FRAM_SIZE_BYTES 262144
#define FRAM_SIZE_BYTES 10

#endif /* FRAMFUNC_H_ */
