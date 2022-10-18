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
#define SYS_CLK_SPEED 16000000

// SPI clock speed. Cannot exceed masterclock, defined above, or any of the
// max chip speeds.
#define SPI_CLK_SPEED 4000000

// A state tracker for the menu
enum MENU_STATES {INIT, MAIN, AUTO};
extern enum MENU_STATES menu_state;

#endif /* MDE_MUX_PROTOTYPE_H_ */
