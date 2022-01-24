//*****************************************************************************
//
// This header file contains the hardware mappings of every chip.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef ACCESS_TOOLS_H
#define ACCESS_TOOLS_H

// The highest chip number that we consider valid. Since chip numbers go from 0
// to this, it's going to be the total number of chips minus 1. Only changing
// this does not handle higher numbers of chips. Changes must also be made to
// the retrieve ports and pins functions.
#define MAX_CHIP_NUMBER 39

// Method prototypes
extern void WriteToChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
extern void ReadFromChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
extern void EnableChipSelects();

// GPIO Ports to enable for chip selects, hard-coded to enable each in the
// EnableChipSelects method. If another port needs to be enabled, add a macro
// here and enable in that method.
#define CS_SYSCTL_PORT_0 SYSCTL_PERIPH_GPIOM
#define CS_SYSCTL_PORT_1 SYSCTL_PERIPH_GPIOH
#define CS_SYSCTL_PORT_2 SYSCTL_PERIPH_GPIOD
#define CS_SYSCTL_PORT_3 SYSCTL_PERIPH_GPION
#define CS_SYSCTL_PORT_4 SYSCTL_PERIPH_GPIOP
#define CS_SYSCTL_PORT_5 SYSCTL_PERIPH_GPIOF

// Generally, two pieces of information are needed for each chip: a pin and
// a port for the chip select. Each chip also has a number, but the relation
// between these macros and those numbers is hardcoded in the Access_Tools.c
// file. This is an awful solution, but the best that I could think of.

// EEPROM Mapping - chips 0 through 7
#define EEPROM_PORT_0 GPIO_PORTN_BASE
#define EEPROM_PIN_0 GPIO_PIN_3

#define EEPROM_PORT_1 GPIO_PORTP_BASE
#define EEPROM_PIN_1 GPIO_PIN_2

#define EEPROM_PORT_2 0
#define EEPROM_PIN_2 0

#define EEPROM_PORT_3 0
#define EEPROM_PIN_3 0

#define EEPROM_PORT_4 0
#define EEPROM_PIN_4 0

#define EEPROM_PORT_5 0
#define EEPROM_PIN_5 0

#define EEPROM_PORT_6 0
#define EEPROM_PIN_6 0

#define EEPROM_PORT_7 0
#define EEPROM_PIN_7 0

// FLASH Mapping - chips 8 through 15
#define FLASH_PORT_0 GPIO_PORTM_BASE
#define FLASH_PIN_0 GPIO_PIN_3

#define FLASH_PORT_1 GPIO_PORTH_BASE
#define FLASH_PIN_1 GPIO_PIN_2

#define FLASH_PORT_2 0
#define FLASH_PIN_2 0

#define FLASH_PORT_3 0
#define FLASH_PIN_3 0

#define FLASH_PORT_4 0
#define FLASH_PIN_4 0

#define FLASH_PORT_5 0
#define FLASH_PIN_5 0

#define FLASH_PORT_6 0
#define FLASH_PIN_6 0

#define FLASH_PORT_7 0
#define FLASH_PIN_7 0

// FRAM Mapping - chips 16 through 23
#define FRAM_PORT_0 GPIO_PORTF_BASE
#define FRAM_PIN_0 GPIO_PIN_1

#define FRAM_PORT_1 GPIO_PORTF_BASE
#define FRAM_PIN_1 GPIO_PIN_2

#define FRAM_PORT_2 0
#define FRAM_PIN_2 0

#define FRAM_PORT_3 0
#define FRAM_PIN_3 0

#define FRAM_PORT_4 0
#define FRAM_PIN_4 0

#define FRAM_PORT_5 0
#define FRAM_PIN_5 0

#define FRAM_PORT_6 0
#define FRAM_PIN_6 0

#define FRAM_PORT_7 0
#define FRAM_PIN_7 0

// MRAM Mapping - chips 24 through 31
#define MRAM_PORT_0 GPIO_PORTH_BASE
#define MRAM_PIN_0 GPIO_PIN_3

#define MRAM_PORT_1 GPIO_PORTD_BASE
#define MRAM_PIN_1 GPIO_PIN_1

#define MRAM_PORT_2 0
#define MRAM_PIN_2 0

#define MRAM_PORT_3 0
#define MRAM_PIN_3 0

#define MRAM_PORT_4 0
#define MRAM_PIN_4 0

#define MRAM_PORT_5 0
#define MRAM_PIN_5 0

#define MRAM_PORT_6 0
#define MRAM_PIN_6 0

#define MRAM_PORT_7 0
#define MRAM_PIN_7 0

// SRAM Mapping - chips 32 through 39
#define SRAM_PORT_0 GPIO_PORTD_BASE
#define SRAM_PIN_0 GPIO_PIN_0

#define SRAM_PORT_1 GPIO_PORTN_BASE
#define SRAM_PIN_1 GPIO_PIN_2

#define SRAM_PORT_2 0
#define SRAM_PIN_2 0

#define SRAM_PORT_3 0
#define SRAM_PIN_3 0

#define SRAM_PORT_4 0
#define SRAM_PIN_4 0

#define SRAM_PORT_5 0
#define SRAM_PIN_5 0

#define SRAM_PORT_6 0
#define SRAM_PIN_6 0

#define SRAM_PORT_7 0
#define SRAM_PIN_7 0

#endif