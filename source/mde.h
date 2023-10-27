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
void EnableSPI(void);

/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/

//#define PART_TM4C123GH6PM

// Clock Speed of the MCU
#define SYS_CLK_SPEED 16000000


// The total number of chips on the boards
#define TOTAL_CHIP_COUNT    31  //(32, but starts at 0 for programming reasons)
#define MDE_BOARD1_CHIP_MAX 15  //  0 - 15
#define MDE_BOARD2_CHIP_MAX 31  // 16 - 31


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

// Tracking variables for writing and reading from chips
extern uint8_t auto_chip_number; // current chip tracked by global variable to allow returns from watchdog interrupts.
extern uint32_t current_data_cycle; // Current data cylce, 0 or 1. Probably doesn't need to be global, should consider it.
extern uint16_t cycle_count; // cycle count - tracks number of memory cycles

//**************************************************************//
//                                                              //
// Information to enable and configure the SSI communication    //
//                                                              //
//**************************************************************//

// SSI pins and ports
#define SPI0_NUM_BASE       SSI0_BASE
#define SPI0_SYS_PERIPH     SYSCTL_PERIPH_SSI0
#define SPI0_SYS_PORT       SYSCTL_PERIPH_GPIOA
#define SPI0_PORT           GPIO_PORTA_BASE
#define SPI0_CLK            GPIO_PA2_SSI0CLK
#define SPI0_CLK_NUM        GPIO_PIN_2
#define SPI0_MISO           GPIO_PA4_SSI0RX
#define SPI0_MOSI           GPIO_PA5_SSI0TX
#define SPI0_MOSI_NUM       GPIO_PIN_5
#define SPI0_MISO_NUM       GPIO_PIN_4

// SSI pins and ports Board 2
#define SPI1_NUM_BASE       SSI1_BASE
#define SPI1_SYS_PERIPH     SYSCTL_PERIPH_SSI1
#define SPI1_SYS_PORT       SYSCTL_PERIPH_GPIOF
#define SPI1_PORT           GPIO_PORTF_BASE
#define SPI1_CLK            GPIO_PF2_SSI1CLK
#define SPI1_CLK_NUM        GPIO_PIN_2
#define SPI1_MOSI           GPIO_PF1_SSI1TX
#define SPI1_MISO           GPIO_PF0_SSI1RX
#define SPI1_MOSI_NUM       GPIO_PIN_1
#define SPI1_MISO_NUM       GPIO_PIN_0

// Clock information needed for SPI timing/rate
// The target clock speed for the system clock, 16 MHz
#define SYS_CLK_SPEED 16000000
// SPI clock speed. Cannot exceed masterclock, defined above, or any of the maximum chip speeds
#define SPI_CLK_SPEED 4000000

//**************************************************************//
//                                                              //
//      GPIO information for controlling power to the boards    //
//                                                              //
//**************************************************************//

void MDEBoard1PowerOn(void);
void MDEBoard1PowerOff(void);
void MDEBoard2PowerOn(void);
void MDEBoard2PowerOff(void);

#endif /* MDE_H_ */
