//*****************************************************************************
//
// This header file contains all the global variables and defines.
//
//*****************************************************************************

// Make sure this file is only included once
#ifndef MDE_PAYLOAD_TM4C123GXL_H
#define MDE_PAYLOAD_TM4C123GXL_H

// Whether we are running in debug mode or not.
// In debug mode, all output is printed in a human readable format, while in
// non-debug (flight) mode, all output is sent in the agreed packet format.
// To enable and disable debug mode, comment or uncomment the following line.
#define DEBUG

// Whether to seed errors in the chips, and where and what to seed.
// Inserting errors at addresses that are multiples of 256 are easiest to verify
// as any address that is a multiple of 256 will have the same value as the
// sequence start value. At these addresses, it's easy to find how many errors
// there should be by comparing the seeded error value and sequence start value.
// To enable or disable seeded errors, comment or uncomment the following lines.
#define SEEDERRORS
#define SEEDERRORS_ADDRESS 65536
#define SEEDERRORS_VALUE 0xFF

// The primary UART base to ready and write to, configured independently of
// whether we're in debug mode or not.
#define UART_PRIMARY UART0_BASE
#define ENABLE_UART0

// The baud rate to use for either UART connection
#define BAUD_RATE 115200

// The total number of chips on the board
#define TOTAL_CHIP_COUNT 32

// Sequence variables, which control what is written to the chips
// Start and offset values are declared in the main c file, not here
// It's best if the offset is prime
#define SEQUENCE_START_VALUES_LENGTH 10
extern const unsigned char sequence_start_values[SEQUENCE_START_VALUES_LENGTH];
extern uint32_t current_sequence_start;
#define SEQUENCE_OFFSET_VALUES_LENGTH 8
extern const unsigned char sequence_offset_values[SEQUENCE_OFFSET_VALUES_LENGTH];
extern uint32_t current_sequence_offset;
extern unsigned short cycle_count;

// The maximum amount of errors that can be stored
// If this is exceeded then the system loops back around, though the looping is
// sort of broken as when transmitting the system will forget about the previous
// set of errors.
#define ERROR_BUFFER_MAX_SIZE 1000
// The buffer variables
extern uint64_t error_buffer[ERROR_BUFFER_MAX_SIZE];
extern uint32_t current_error;
extern uint32_t old_current_error;

// NOTE: In the next two sections, defining shifts and masks for the messages
// masks are applied before shifts!

// The value placed in the two bit header of each error message
// Valid values are 0 to 3
#define ERROR_DATA_HEADER_VALUE 3
// The shifts and masks for the error data, in case the error data formatting
// needs to be redefined. Adding data needs changes in each chip .c file.
#define ERROR_DATA_HEADER_SHIFT 38
#define ERROR_DATA_HEADER_MASK 0x3
#define ERROR_DATA_CHIP_SHIFT 32
#define ERROR_DATA_CHIP_MASK 0x3F
#define ERROR_DATA_BYTE_SHIFT 6
#define ERROR_DATA_BYTE_MASK 0x3FFFFF
#define ERROR_DATA_DIRECTION_SHIFT 3
#define ERROR_DATA_DIRECTION_MASK 0x1
#define ERROR_DATA_BIT_SHIFT 0
#define ERROR_DATA_BIT_MASK 0x7

// The value placed in the two bit header of each health message
// Valid values are 0 to 3
#define HEALTH_DATA_HEADER_VALUE 2
// The shifts and masks for the health data, in case the health data formatting
// needs to be redefined. Adding data needs changes in main .c file.
#define HEALTH_DATA_HEADER_SHIFT 62
#define HEALTH_DATA_HEADER_MASK 0x3
#define HEALTH_DATA_CYCLE_SHIFT 48
#define HEALTH_DATA_CYCLE_MASK 0x3FFF // Only 14 bits allocated for the count
#define HEALTH_DATA_RESPONSIVENESS_SHIFT 8
#define HEALTH_DATA_RESPONSIVENESS_MASK 0xFFFFFFFFFF
#define HEALTH_DATA_START_SHIFT 4
#define HEALTH_DATA_START_MASK 0xF
#define HEALTH_DATA_OFFSET_SHIFT 0
#define HEALTH_DATA_OFFSET_MASK 0xF

// The unresponsiveness buffer
// Chips can be marked as unresponsive if they do not transmit any data
// Eventually this could be used to determine if a power cycle should happen
// This buffer is a single 64 bit value and each bit is written to indicate 
// responsiveness. For example, bit 4 corresponds to chip 4.
extern uint64_t chip_unresponsive;
// The number of bytes to check before declaring a chip unresponsive
#define BLANK_BYTE_UNRESPONSIVE_COUNT 8

// A state tracker for the menu in debug mode
#ifdef DEBUG
enum MENU_STATES {INIT, MAIN, AUTO};
extern enum MENU_STATES menu_state;
#endif

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME 15//90
#define TIMER_CYCLES 1//10
// Variables for the timer
extern uint32_t timer_current_cycle;

// The target clock speed for the system clock.
#define SYS_CLK_SPEED 20000000

// SPI clock speed. Cannot exceed masterclock, defined above, or any of the
// max chip speeds.
#define SPI_CLK_SPEED 4000000

// SSI pins and ports
#define SPI_NUM_BASE SSI3_BASE
#define SPI_SYS_PERIPH SYSCTL_PERIPH_SSI3
#define SPI_PORT GPIO_PORTQ_BASE
#define SPI_SYS_PORT SYSCTL_PERIPH_GPIOQ
#define SPI_CLK GPIO_PQ0_SSI3CLK
#define SPI_MOSI GPIO_PQ2_SSI3XDAT0
#define SPI_MISO GPIO_PQ3_SSI3XDAT1
#define SPI_CLK_NUM GPIO_PIN_0
#define SPI_MOSI_NUM GPIO_PIN_2
#define SPI_MISO_NUM GPIO_PIN_3

// The 2 extra pins for SQI
#define SPI_SYS_DAT_PORT SYSCTL_PERIPH_GPIOP
#define SPI_DAT_PORT GPIO_PORTP_BASE
#define SPI_DAT2 GPIO_PP0_SSI3XDAT2
#define SPI_DAT3 GPIO_PP1_SSI3XDAT3
#define SPI_DAT2_NUM GPIO_PIN_0
#define SPI_DAT3_NUM GPIO_PIN_1

// UARTSend prototype
extern void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);

#endif