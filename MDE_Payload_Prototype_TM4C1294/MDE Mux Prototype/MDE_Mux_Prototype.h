//*****************************************************************************
//
// This header file contains all the global variables and defines.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef MDE_MUX_PROTOTYPE_H
#define MDE_MUX_PROTOTYPE_H

// The primary UART base to ready and write to
#define UART_PRIMARY UART1_BASE
#define ENABLE_UART1

// The baud rate to use for either UART connection
#define BAUD_RATE 115200

// The total number of chips on the board
#define TOTAL_CHIP_COUNT 8
// Chips the testing MUX can handle. Was included before differently, may not be necessary at all
#define MAX_CHIP_NUMBER 16

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME 10
// Variables for the timer
extern uint32_t timer_current_cycle;

// The target clock speed for the system clock.
#define SYS_CLK_SPEED 20000000

// SPI clock speed. Cannot exceed masterclock, defined above, or any of the
// max chip speeds.
#define SPI_CLK_SPEED 4000000

// A state tracker for the menu
enum MENU_STATES {INIT, MAIN, AUTO};
extern enum MENU_STATES menu_state;

// SSI pins and ports - not needed right now but ports have been updated for board 1
#define SPI_NUM_BASE SSI0_BASE
#define SPI_SYS_PERIPH SYSCTL_PERIPH_SSI0
#define SPI_PORT GPIO_PORTA_BASE
#define SPI_SYS_PORT SYSCTL_PERIPH_GPIOA
#define SPI_CLK GPIO_PA2_SSI0CLK
#define SPI_CLK_NUM GPIO_PIN_2
#define SPI_MOSI GPIO_PA5_SSI0XDAT0
#define SPI_MOSI_NUM GPIO_PIN_5
#define SPI_MISO GPIO_PA4_SSI0XDAT1
#define SPI_MISO_NUM GPIO_PIN_4

// Chip Select Pins and Ports
#define CS_SYSCTL_PORT SYSCTL_PERIPH_GPIOD
#define CS_SYSCTL_PIN_0 GPIO_PIN_0
#define CS_SYSCTL_PIN_1 GPIO_PIN_1
#define CS_SYSCTL_PIN_2 GPIO_PIN_2
#define CS_SYSCTL_PIN_3 GPIO_PIN_3

// MUX chip select assignments (currently default, update before using)
uint8_t CHIP_SELECT_MUX_PINS[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

#endif /* MDE_MUX_PROTOTYPE_H_ */
