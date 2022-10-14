//*****************************************************************************
//
// This header file contains all the global variables and defines.
//
//*****************************************************************************

//THIS Code HAS BEEN TRANSFERED OVER TO Access_Tools.h

// Make sure this file is only included once
#ifndef MDE_MUX_PROTOTYPE_H
#define MDE_MUX_PROTOTYPE_H

// The primary UART base to read and write to (OBC Communication)
#define UART_PRIMARY UART1_BASE
#define ENABLE_UART1

// The baud rate to use for either UART connection
#define BAUD_RATE 115200

// The total number of chips on the board
#define TOTAL_CHIP_COUNT 16
// Chips the testing MUX can handle. Was included before differently, may not be necessary at all
#define MAX_CHIP_NUMBER 32

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
// TODO:Change this to 90 (5400 seconds) minutes later for flight
#define MEMORY_CYCLE_TIME 10

// Variables for the timer
extern uint32_t timer_current_cycle;

// The target clock speed for the system clock.
// 16 MHz
#define SYS_CLK_SPEED 16000000

// SPI clock speed. Cannot exceed masterclock, defined above, or any of the
// max chip speeds.
#define SPI_CLK_SPEED 4000000

// A state tracker for the menu
enum MENU_STATES {INIT, MAIN, AUTO};
extern enum MENU_STATES menu_state;


// I believe SPI info will need to be moved to Access_Tools.h so chip functions can access them
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

// Chip Select Pins and Ports Board 1 (Moved to Access_Tools.h)
#define CS0_SYSCTL_PORT     SYSCTL_PERIPH_GPIOD
#define CS0_SYSCTL_PIN_0    GPIO_PIN_0
#define CS0_SYSCTL_PIN_1    GPIO_PIN_1
#define CS0_SYSCTL_PIN_2    GPIO_PIN_2
#define CS0_SYSCTL_PIN_3    GPIO_PIN_3

// Chip Select Pins and Ports Board 2 (Moved to Access_tools.h)
#define CS1_SYSCTL_PORT     SYSCTL_PERIPH_GPIOC
#define CS1_SYSCTL_PIN_0    GPIO_PIN_4
#define CS1_SYSCTL_PIN_1    GPIO_PIN_5
#define CS1_SYSCTL_PIN_2    GPIO_PIN_6
#define CS1_SYSCTL_PIN_3    GPIO_PIN_7

// MUX chip select assignments (currently default, update before using)
uint8_t CHIP_SELECT_MUX_PINS[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

#endif /* MDE_MUX_PROTOTYPE_H_ */
