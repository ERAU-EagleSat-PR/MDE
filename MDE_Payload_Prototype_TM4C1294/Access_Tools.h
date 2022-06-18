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
#define MAX_CHIP_NUMBER 31

// Method prototypes
extern void WriteToChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
extern void ReadFromChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
extern void EnableChipSelects();

// GPIO Ports to enable for chip selects, hard-coded to enable each in the
// EnableChipSelects method. If another port needs to be enabled, add a macro
// here and enable in that method.

// Chip Select ports and pins for primary memory board
#define CS0_0_PORT GPIO_PORTD_BASE
#define CS0_0_PIN GPIO_PIN_0
#define CS0_1_PORT GPIO_PORTD_BASE
#define CS0_1_PIN GPIO_PIN_1
#define CS0_2_PORT GPIO_PORTD_BASE
#define CS0_2_PIN GPIO_PIN_2
#define CS0_3_PORT GPIO_PORTD_BASE
#define CS0_3_PIN GPIO_PIN_3


// Chip Select ports and pins for 2nd memory board
#define CS1_0_PORT GPIO_PORTC_BASE
#define CS1_0_PIN GPIO_PIN_6
#define CS1_1_PORT GPIO_PORTC_BASE
#define CS1_1_PIN GPIO_PIN_7
#define CS1_2_PORT GPIO_PORTD_BASE
#define CS1_2_PIN GPIO_PIN_6
#define CS1_3_PORT GPIO_PORTD_BASE
#define CS1_3_PIN GPIO_PIN_7

uint8_t CHIP_SELECT_COMBOS[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

#endif