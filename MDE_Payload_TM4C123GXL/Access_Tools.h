//*****************************************************************************
//
// This header file contains the hardware mappings of every chip.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef ACCESS_TOOLS_H
#define ACCESS_TOOLS_H

// The highest chip number that we consider valid. Since chip numbers go from 0
// to this, it's going to be the (total number of chips) - 1. Only changing
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
// All pins are on the same port
#define BOARD1_CS_PORT  GPIO_PORTD_BASE
#define CS0_0_PIN   GPIO_PIN_0
#define CS0_1_PIN   GPIO_PIN_1
#define CS0_2_PIN   GPIO_PIN_2
#define CS0_3_PIN   GPIO_PIN_3


// Chip Select ports and pins for 2nd memory board
// All pins are on the same port
#define BOARD2_CS_PORT  GPIO_PORTC_BASE
#define CS1_0_PIN   GPIO_PIN_4
#define CS1_1_PIN   GPIO_PIN_5
#define CS1_2_PIN   GPIO_PIN_6
#define CS1_3_PIN   GPIO_PIN_7


// Multiplexer input for both boards
#define FLASH1_MUX_CS   0x00
#define FLASH2_MUX_CS   0x01
#define FLASH3_MUX_CS   0x02
#define FLASH4_MUX_CS   0x03
#define FRAM1_MUX_CS    0x04
#define FRAM2_MUX_CS    0x05
#define FRAM3_MUX_CS    0x06
#define FRAM4_MUX_CS    0x07
#define MRAM1_MUX_CS    0x08
#define MRAM2_MUX_CS    0x09
#define MRAM3_MUX_CS    0x0A
#define MRAM4_MUX_CS    0x0B
#define SRAM1_MUX_CS    0x0C
#define SRAM2_MUX_CS    0x0D
#define SRAM3_MUX_CS    0x0E
#define SRAM4_MUX_CS    0x0F

#endif