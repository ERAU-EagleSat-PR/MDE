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

void WriteToChips(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
void ReadFromChips(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset);
uint8_t CheckChipHealth(uint32_t chip_number);
// TODO: configure the SPI clock if needed

/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/
#define MAX_CHIP_NUMBER 32 // 32 chips across 2 boards.

//-----------------------------------------------------------------------------
// SPI port and pin defines
//-----------------------------------------------------------------------------

// SPI clock speed. Cannot exceed master clock (16MHz) defined above, or any of the
// max chip speeds.
#define SPI_CLK_SPEED 4000000

#endif /* SOURCE_CHIPS_H_ */

