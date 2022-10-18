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
#define MAX_CHIP_NUMBER 15

// Method prototypes
extern void WriteToChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
extern void ReadFromChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
extern void EnableChipSelects();

// GPIO Ports to enable for chip selects, hard-coded to enable each in the
// EnableChipSelects method. If another port needs to be enabled, add a macro
// here and enable in that method.

// SSI pins for MOSI/MISO board communication
// SSI pins and ports Board 1
#define SPI0_NUM_BASE SSI0_BASE
#define SPI0_SYS_PERIPH SYSCTL_PERIPH_SSI0
#define SPI0_SYS_PORT SYSCTL_PERIPH_GPIOA
#define SPI0_PORT GPIO_PORTA_BASE
#define SPI0_CLK GPIO_PA2_SSI0CLK
#define SPI0_CLK_NUM GPIO_PIN_2
#define SPI0_MOSI GPIO_PA5_SSI0XDAT0
#define SPI0_MISO GPIO_PA4_SSI0XDAT1
#define SPI0_MOSI_NUM GPIO_PIN_5
#define SPI0_MISO_NUM GPIO_PIN_4

// SSI pins and ports Board 2
#define SPI1_NUM_BASE SSI1_BASE
#define SPI1_SYS_PERIPH SYSCTL_PERIPH_SSI1
#define SPI1_SYS_PORT SYSCTL_PERIPH_GPIOF
#define SPI1_PORT GPIO_PORTF_BASE
#define SPI1_CLK GPIO_PF2_SSI1CLK
#define SPI1_CLK_NUM GPIO_PIN_2
#define SPI1_MOSI GPIO_PF1_SSI1XDAT0
#define SPI1_MISO GPIO_PF0_SSI1XDAT1
#define SPI1_MOSI_NUM GPIO_PIN_1
#define SPI1_MISO_NUM GPIO_PIN_0

// Chip Select ports and pins for primary memory board
// Chip Select Pins and Ports Board 1
#define CS0_SYSCTL_PORT SYSCTL_PERIPH_GPIOD
#define CS0_PORT  GPIO_PORTD_BASE
#define CS0_PIN_0 GPIO_PIN_0
#define CS0_PIN_1 GPIO_PIN_1
#define CS0_PIN_2 GPIO_PIN_2
#define CS0_PIN_3 GPIO_PIN_3

// Chip Select Pins and Ports Board 2
#define CS1_SYSCTL_PORT SYSCTL_PERIPH_GPIOC
#define CS1_PORT  GPIO_PORTC_BASE
#define CS1_PIN_0 GPIO_PIN_4
#define CS1_PIN_1 GPIO_PIN_5
#define CS1_PIN_2 GPIO_PIN_6
#define CS1_PIN_3 GPIO_PIN_7


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

// MUX chip select assignments for looping
uint8_t CHIP_SELECT_MUX_PINS[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};


#endif
