//*****************************************************************************
//
// This is the header file is for the MB85RS16N FRAM.
//
//*****************************************************************************

#ifndef FRAMFUNC_H
#define FRAMFUNC_H

typedef struct {
    uint8_t fujID;
    uint8_t contCode;
    uint8_t prodID1;
    uint8_t prodID2;
} FRAMID;

void FRAMSequenceTransmit(uint8_t current_cycle, uint32_t chip_number);
void FRAMSequenceRetrieve(uint8_t current_cycle, uint32_t chip_number);
FRAMID FRAMStatusRead(uint8_t chip_number);

// FRAM Command Codes
#define FRAM_READ 0x03
#define FRAM_WRITE 0x02
#define FRAM_IDENT 0x9F
#define FRAM_WRITE_ENABLE 0x06
#define FRAM_WRITE_DISABLE 0x04

// Expected values from FRAM device ID register
#define FRAM_FUJ_ID 0x04
#define FRAM_CONT_CODE 0x7F
#define FRAM_PROD1 0x01 //0x48
#define FRAM_PROD2 0x01 //0x0A Docu says these values, but we get 1 and 1 from all 8 chips though they work. Not sure.

// FRAM Size
#define FRAM_SIZE_BYTES 2048

#endif /* FRAMFUNC_H_ */
