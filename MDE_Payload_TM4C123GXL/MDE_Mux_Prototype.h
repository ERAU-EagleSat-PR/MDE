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

// A state tracker for the menu
enum MENU_STATES {INIT, MAIN, AUTO};
extern enum MENU_STATES menu_state;


#endif /* MDE_MUX_PROTOTYPE_H_ */
