/*
 *  mde.h
 *
 *  Created on: Dec 17, 2022
 *
 */

#ifndef MDE_H_
#define MDE_H_

// Whether we are running in debug mode or not.
// In debug mode, all output is printed in a human readable format, while in
// non-debug (flight) mode, all output is sent in the agreed packet format.
// To enable and disable debug mode, comment or un-comment the following line.

#define DEBUG

// Whether to seed errors in the chips, and where and what to seed.
// Inserting errors at addresses that are multiples of 256 are easiest to verify
// as any address that is a multiple of 256 will have the same value as the
// sequence start value. At these addresses, it's easy to find how many errors
// there should be by comparing the seeded error value and sequence start value.
// To enable or disable seeded errors, comment or un-comment the following lines.
#define SEEDERRORS
#define SEEDERRORS_ADDRESS  65536
#define SEEDERRORS_VALUE    0xFF

// The total number of chips on the board
#define TOTAL_CHIP_COUNT    32

// Chip Numbers
#define CS_FLASH1  0
#define CS_FLASH2  1
#define CS_FLASH3  2
#define CS_FLASH4  3
#define CS_FRAM1   4
#define CS_FRAM2   5
#define CS_FRAM3   6
#define CS_FRAM4   7
#define CS_MRAM1   8
#define CS_MRAM2   9
#define CS_MRAM3   10
#define CS_MRAM4   11
#define CS_SRAM1   12
#define CS_SRAM2   13
#define CS_SRAM3   14
#define CS_SRAM4   15


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
#define ERROR_DATA_HEADER_SHIFT     38
#define ERROR_DATA_HEADER_MASK      0x3
#define ERROR_DATA_CHIP_SHIFT       32
#define ERROR_DATA_CHIP_MASK        0x3F
#define ERROR_DATA_BYTE_SHIFT       6
#define ERROR_DATA_BYTE_MASK        0x3FFFFF
#define ERROR_DATA_DIRECTION_SHIFT  3
#define ERROR_DATA_DIRECTION_MASK   0x1
#define ERROR_DATA_BIT_SHIFT        0
#define ERROR_DATA_BIT_MASK         0x7

// The value placed in the two bit header of each health message
// Valid values are 0 to 3
#define HEALTH_DATA_HEADER_VALUE 2
// The shifts and masks for the health data, in case the health data formatting
// needs to be redefined. Adding data needs changes in main .c file.
#define HEALTH_DATA_HEADER_SHIFT    56                     //while byte header
#define HEALTH_DATA_HEADER_MASK     0xF                     // 11111111h
#define HEALTH_DATA_CYCLE_SHIFT     48
#define HEALTH_DATA_CYCLE_MASK      0x3FFF // Only 14 bits allocated for the count
#define HEALTH_DATA_START_SHIFT     4
#define HEALTH_DATA_START_MASK      0xF
#define HEALTH_DATA_OFFSET_SHIFT    0
#define HEALTH_DATA_OFFSET_MASK     0xF
#define HEALTH_DATA_RESPONSIVENESS_SHIFT 8
#define HEALTH_DATA_RESPONSIVENESS_MASK  0xFFFFFFFFFF

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
enum MENU_STATES {  INIT,
                    MAIN,
                    AUTO,
                    CHIP_SELECT_BOARD,
                    CHIP_SELECT_TYPE,
                    CHIP_SELECT_NUM};
extern enum MENU_STATES menu_state;

enum BOARDS      {  NO_BOARD,
                    BOARD1,
                    BOARD2};

enum MEM_TYPES   {  NO_MEM_TYPE,
                    FLASH,
                    FRAM,
                    MRAM,
                    SRAM};

enum CHIP_NUMBERS{  NO_CHIP,
                    FLASH1, FLASH2, FLASH3, FLASH4,
                    FRAM1, FRAM2, FRAM3, FRAM4,
                    MRAM1, MRAM2, MRAM3, MRAM4,
                    SRAM1, SRAM2, SRAM3, SRAM4};

extern enum MENU_STATES menu_state;
#endif

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME 90 //IN
#define TIMER_CYCLES 1//10
// Variables for the timer
extern uint32_t timer_current_cycle;


#endif /* CODE_SCIENCE_H_ */
