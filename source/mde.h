/*
 *  mde.h
 *
 *  Bit string sequence generation and comparison
 *
 *  Created on: Dec 17, 2022
 *
 */

#ifndef MDE_H_
#define MDE_H_

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/


/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/

//#define PART_TM4C123GH6PM

// Clock Speed of the MCU
#define SYS_CLK_SPEED 16000000


// The total number of chips on the boards
#define TOTAL_CHIP_COUNT    32
#define MDE_BOARD1_CHIP_MAX 16
#define MDE_BOARD2_CHIP_MAX 32


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


#endif /* MDE_H_ */
