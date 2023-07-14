/*
 * multiplexer.h
 *
 *  Created on: Dec 29, 2022
 *      Author: Calvin
 */

#ifndef SOURCE_MULTIPLEXER_H_
#define SOURCE_MULTIPLEXER_H_

/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/

extern uint8_t  RetreiveCSCode(uint32_t chipNumber);
extern uint32_t RetrieveCSPort(uint32_t chipNumber);
void            SetChipSelect(uint32_t chipNumber);
void            ResetChipSelect1(void);
void            ResetChipSelect2(void);

/*
*******************************************************************************
*                               Hardware Contants                             *
*******************************************************************************
*/

// GPIO Ports to enable for chip selects, hard-coded to enable each in the
// EnableChipSelects method. If another port needs to be enabled, add a macro
// here and enable in that method.

// Chip Select ports and pins for Board 1
// All pins are on the same port
#define BOARD1_CS_PORT_BASE     GPIO_PORTD_BASE
#define BOARD1_CS_PORT_SYSCTL   SYSCTL_PERIPH_GPIOD
#define CS1_PIN_0           GPIO_PIN_0
#define CS1_PIN_1           GPIO_PIN_1
#define CS1_PIN_2           GPIO_PIN_2
#define CS1_PIN_3           GPIO_PIN_3


// Chip Select ports and pins for Board 2
// All pins are on the same port
#define BOARD2_CS_PORT_BASE     GPIO_PORTC_BASE
#define BOARD2_CS_PORT_SYSCTL   SYSCTL_PERIPH_GPIOC
#define CS2_PIN_0           GPIO_PIN_4
#define CS2_PIN_1           GPIO_PIN_5
#define CS2_PIN_2           GPIO_PIN_6
#define CS2_PIN_3           GPIO_PIN_7


// Multiplexer input for both boards    chip Number: Board 1     Board 2
#define RESET_MUX           0x00        //              //          //
#define FLASH1_MUX_CS       0x00        //              0           16
#define FLASH2_MUX_CS       0x01        //              1           17
#define FLASH3_MUX_CS       0x02        //              2           18
#define FLASH4_MUX_CS       0x03        //              3           19
#define FRAM1_MUX_CS        0x04        //              4           20
#define FRAM2_MUX_CS        0x05        //              5           21
#define FRAM3_MUX_CS        0x06        //              6           22
#define FRAM4_MUX_CS        0x07        //              7           23
#define MRAM1_MUX_CS        0x08        //              8           24
#define MRAM2_MUX_CS        0x09        //              9           25
#define MRAM3_MUX_CS        0x0A        //              10          26
#define MRAM4_MUX_CS        0x0B        //              11          27
#define SRAM1_MUX_CS        0x0C        //              12          28
#define SRAM2_MUX_CS        0x0D        //              13          29
#define SRAM3_MUX_CS        0x0E        //              14          30
#define SRAM4_MUX_CS        0x0F        //              15          31


#endif /* SOURCE_MULTIPLEXER_H_ */
