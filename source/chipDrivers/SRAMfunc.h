//*****************************************************************************
//
// This is the header file is for the IS62WVS5128GBLL SRAM.
//
//*****************************************************************************

// Included once
#ifndef SRAMFUNC_H_
#define SRAMFUNC_H_

// Function Prototypes
void SRAMSequenceTransmit(uint8_t current_cycle, uint32_t chip_number);
void SRAMSequenceRetrieve(uint8_t current_cycle, uint32_t chip_number);
uint8_t SRAMStatusRead(uint32_t chip_number);

// SRAM Command Codes
#define SRAM_READ 0x03
#define SRAM_WRITE 0x02
#define SRAM_RDMR 0x05
#define SRAM_WRMR 0x01
#define SRAM_SEQUENTIAL_CODE 0x40

// Expected value from SR.
#define SRAM_EXPECTED 0b01000000
// First two bytes are 01 for sequential mode, rest are reserved 0.

// SRAM maximum Size
#define SRAM_SIZE_BYTES 512000

#endif /* SRAMFUNC_H_ */
