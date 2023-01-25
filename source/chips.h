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
// TODO: configure the SPI clock if needed

/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// SPI port and pin defines
//-----------------------------------------------------------------------------

// SPI clock speed. Cannot exceed master clock (16MHz) defined above, or any of the
// max chip speeds.
#define SPI_CLK_SPEED 4000000

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

#endif /* SOURCE_CHIPS_H_ */

