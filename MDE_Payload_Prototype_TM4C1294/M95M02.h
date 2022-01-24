//*****************************************************************************
//
// This is the header file for the M95M02 EEPROM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef M95M02_H
#define M95M02_H

extern void EEPROMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin);
extern void EEPROMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_select_port, uint32_t chip_select_pin, uint32_t chip_number);

// EEPROM Command Codes
#define EEPROM_READ 0x03
#define EEPROM_WRITE 0x02
#define EEPROM_WRITE_ENABLE 0x06
#define EEPROM_WRITE_DISABLE 0x04
#define EEPROM_WRSR 0x01
#define EEPROM_RDSR 0x05

// EEPROM Size
#define EEPROM_SIZE_BYTES 250000
#define EEPROM_PAGE_SIZE 256

#endif