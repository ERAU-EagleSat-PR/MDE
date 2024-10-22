/*
 * chips.h
 *
 * The chips.h/c files contain code related to chip functions as a whole. The
 * individual chips each have their own driver files
 *
 */

#ifndef SOURCE_CHIPS_H_
#define SOURCE_CHIPS_H_


/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/

void WriteToChip(uint32_t current_cycle, uint32_t chip_number);
void ReadFromChip(uint32_t current_cycle, uint32_t chip_number);
// TODO: configure the SPI clock if needed

/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/
//#define MAX_CHIP_NUMBER 16 // 16 chips for one board, DEBUGGING purposes only.
#define MAX_CHIP_NUMBER 32 // 32 chips across 2 boards.


/*
 ********************************************************************************
 *                               Global Variables                               *
 ********************************************************************************
 */
// Global tracker of currently active chip. Should be kept up to date, it is used for watchdog timer tracking.
extern uint8_t current_chip;
extern uint8_t currentCycle;   // Value 0 or 1 for writing 0s or 1s

//-----------------------------------------------------------------------------
// SPI port and pin defines
//-----------------------------------------------------------------------------

// SPI clock speed. Cannot exceed master clock (16MHz) defined above, or any of the
// max chip speeds.
#define SPI_CLK_SPEED 4000000

#endif /* SOURCE_CHIPS_H_ */

