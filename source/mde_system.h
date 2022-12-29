//*****************************************************************************
//
// This header file contains all the global variables and defines for the
// experiment
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef TM4C123GXL_SYSTEM_H
#define TM4C123GXL_SYSTEM_H

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/
extern void     EnableChipSelects();
extern uint8_t  RetreiveCSCode(uint32_t chipNumber);
extern uint32_t RetrieveCSPort(uint32_t chipNumber);
extern void     setCSOutput(uint32_t chipNumber);

extern void WriteToChip(    uint32_t chipNumber,
                            unsigned char sequence_start,
                            unsigned char sequence_offset);

extern void ReadFromChip(   uint32_t chipNumber,
                            unsigned char sequence_start,
                            unsigned char sequence_offset);



//*****************************************
// System Hardware Variables
//*****************************************

// The primary UART base to read and write to (OBC Communication)
//#define ENABLE_UART_PRIMARY
#define UART_PRIMARY        UART1_BASE
#define UART_PRIMATY_BASE   UART1_BASE
#define UART_PRIMARY_SYSCTL SYSCTL_PERIPH_UART1

// The UART to Use debug terminal through
#define ENABLE_UART_DEBUG
#define UART_DEBUG          UART0_BASE
#define UART_DEBUG_BASE     UART0_BASE
#define UART_DEBUG_SYSCTL   SYSCTL_PERIPH_UART0

// The baud rate to use for either UART connection
#define BAUD_RATE           115200

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME   90          //in seconds

// Variables for the timer
extern uint32_t timer_current_cycle;

// The target clock speed for the system clock.
#define SYS_CLK_SPEED       16000000    // 16 MHz

//*****************************************
// Chip Select Defines with the Multiplexer
//*****************************************

// The highest chip number that we consider valid. Since chip numbers go from 0
// to this, it's going to be the (total number of chips) - 1. Only changing
// this does not handle higher numbers of chips. Changes must also be made to
// the retrieve ports and pins functions.
#define MAX_CHIP_NUMBER     15

// GPIO Ports to enable for chip selects, hard-coded to enable each in the
// EnableChipSelects method. If another port needs to be enabled, add a macro
// here and enable in that method.

// SSI pins for MOSI/MISO board communication
// SSI pins and ports Board 1
#define SPI0_NUM_BASE       SSI0_BASE
#define SPI0_SYS_PERIPH     SYSCTL_PERIPH_SSI0
#define SPI0_SYS_PORT       SYSCTL_PERIPH_GPIOA
#define SPI0_PORT           GPIO_PORTA_BASE
#define SPI0_CLK            GPIO_PA2_SSI0CLK
#define SPI0_CLK_NUM        GPIO_PIN_2
#define SPI0_MOSI           GPIO_PA5_SSI0XDAT0
#define SPI0_MISO           GPIO_PA4_SSI0XDAT1
#define SPI0_MOSI_NUM       GPIO_PIN_5
#define SPI0_MISO_NUM       GPIO_PIN_4

// SSI pins and ports Board 2
#define SPI1_NUM_BASE       SSI1_BASE
#define SPI1_SYS_PERIPH     SYSCTL_PERIPH_SSI1
#define SPI1_SYS_PORT       SYSCTL_PERIPH_GPIOF
#define SPI1_PORT           GPIO_PORTF_BASE
#define SPI1_CLK            GPIO_PF2_SSI1CLK
#define SPI1_CLK_NUM        GPIO_PIN_2
#define SPI1_MOSI           GPIO_PF1_SSI1XDAT0
#define SPI1_MISO           GPIO_PF0_SSI1XDAT1
#define SPI1_MOSI_NUM       GPIO_PIN_1
#define SPI1_MISO_NUM       GPIO_PIN_0

// Chip Select ports and pins for Board 1
// All pins are on the same port
#define BOARD1_CS_PORT_BASE     GPIO_PORTD_BASE
#define BOARD1_CS_PORT_SYSCTL   SYSCTL_PERIPH_GPIOD
#define CS1_PIN_0           GPIO_PIN_0
#define CS1_PIN_1           GPIO_PIN_1
#define CS1_PIN_2           GPIO_PIN_2
#define CS1_PIN_3           GPIO_PIN_3


// Chip Select ports and pins for Board 2
// All pins are on the same port
#define BOARD2_CS_PORT_BASE     GPIO_PORTC_BASE
#define BOARD2_CS_PORT_SYSCTL   SYSCTL_PERIPH_GPIOC
#define CS2_PIN_0           GPIO_PIN_4
#define CS2_PIN_1           GPIO_PIN_5
#define CS2_PIN_2           GPIO_PIN_6
#define CS2_PIN_3           GPIO_PIN_7


// Multiplexer input for both boards
#define FLASH1_MUX_CS       0x00
#define FLASH2_MUX_CS       0x01
#define FLASH3_MUX_CS       0x02
#define FLASH4_MUX_CS       0x03
#define FRAM1_MUX_CS        0x04
#define FRAM2_MUX_CS        0x05
#define FRAM3_MUX_CS        0x06
#define FRAM4_MUX_CS        0x07
#define MRAM1_MUX_CS        0x08
#define MRAM2_MUX_CS        0x09
#define MRAM3_MUX_CS        0x0A
#define MRAM4_MUX_CS        0x0B
#define SRAM1_MUX_CS        0x0C
#define SRAM2_MUX_CS        0x0D
#define SRAM3_MUX_CS        0x0E
#define SRAM4_MUX_CS        0x0F

// alternative method for
/*
uint8_t CHIP_SELECT_MUX_PINS[16] = { 0x00, 0x01, 0x02, 0x03,
                                     0x04, 0x05, 0x06, 0x07,
                                     0x08, 0x09, 0x0A, 0x0B,
                                     0x0C, 0x0D, 0x0E, 0x0F };
*/

//*************************
// SPI port and pin defines
//*************************

// SPI clock speed. Cannot exceed master clock (16MHz) defined above, or any of
// the max chip speeds.
#define SPI_CLK_SPEED       4000000

// SSI pins and ports Board 1
#define SPI0_NUM_BASE       SSI0_BASE
#define SPI0_SYS_PERIPH     SYSCTL_PERIPH_SSI0
#define SPI0_SYS_PORT       SYSCTL_PERIPH_GPIOA
#define SPI0_PORT           GPIO_PORTA_BASE
#define SPI0_CLK            GPIO_PA2_SSI0CLK
#define SPI0_CLK_NUM        GPIO_PIN_2
#define SPI0_MOSI           GPIO_PA5_SSI0XDAT0
#define SPI0_MISO           GPIO_PA4_SSI0XDAT1
#define SPI0_MOSI_NUM       GPIO_PIN_5
#define SPI0_MISO_NUM       GPIO_PIN_4

// SSI pins and ports Board 2
#define SPI1_NUM_BASE       SSI1_BASE
#define SPI1_SYS_PERIPH     SYSCTL_PERIPH_SSI1
#define SPI1_SYS_PORT       SYSCTL_PERIPH_GPIOF
#define SPI1_PORT           GPIO_PORTF_BASE
#define SPI1_CLK            GPIO_PF2_SSI1CLK
#define SPI1_CLK_           NUM GPIO_PIN_2
#define SPI1_MOSI           GPIO_PF1_SSI1XDAT0
#define SPI1_MISO           GPIO_PF0_SSI1XDAT1
#define SPI1_MOSI_NUM       GPIO_PIN_1
#define SPI1_MISO_NUM       GPIO_PIN_0


#endif
