//*****************************************************************************
//
// This header file the prototypes for the BR25A1M EEPROM.
//
//*****************************************************************************

// Make sure this file is only included once
#ifdef BR25A1M_H
#else
#define BR25A1M_H

extern void EEPROMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset);
extern void EEPROMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset);

// EEPROM Command Codes
#define EEPROM_NUMBER 0
#define EEPROM_READ 0x03
#define EEPROM_WRITE 0x02
#define EEPROM_WRITE_ENABLE 0x06
#define EEPROM_WRITE_DISABLE 0x04
#define EEPROM_WRSR 0x01
#define EEPROM_RDSR 0x05

// EEPROM Size
#define EEPROM_SIZE_BYTES 125000
#define EEPROM_MAX_WRITE 256

#endif