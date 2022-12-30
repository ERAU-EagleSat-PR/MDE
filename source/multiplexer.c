/*
 * multiplexer.c
 *
 *  Created on: Dec 29, 2022
 *      Author: Calvin
 */

/*
*******************************************************************************
*                             Include Files                             *
*******************************************************************************
*/

// Standard C files
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware and Driver files
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"

#include "source/multiplexer.h"

/*
*******************************************************************************
*                               MUX Functions                                *
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// Retrieves the associated port for
//-----------------------------------------------------------------------------
uint32_t RetreiveCSPort(uint32_t chip_number){

    uint32_t result = 0;
    if (chip_number < 16)
    {
        result = BOARD1_CS_PORT_BASE;
    }
    else if ( (chip_number >= 16) && (chip_number < 32) )
    {
        result = BOARD2_CS_PORT_BASE;
    }
    else
    {
        result = -1;
    }
    return result;
}


//-----------------------------------------------------------------------------
// Retrieves the associated Hex value for the CS Multiplexer
//-----------------------------------------------------------------------------
uint8_t RetreiveCSCode(uint32_t chipNumber){

    uint32_t result = 0;

    // uint32_t mulitboardCS = chipNumber % 16;

    switch (chipNumber){
        case 0:
            result = FLASH1_MUX_CS;
            break;
        case 1:
            result = FLASH2_MUX_CS;
            break;
        case 2:
            result = FLASH3_MUX_CS;
            break;
        case 3:
            result = FLASH4_MUX_CS;
            break;
        case 4:
            result = FRAM1_MUX_CS;
            break;
        case 5:
            result = FRAM2_MUX_CS;
            break;
        case 6:
            result = FRAM3_MUX_CS;
            break;
        case 7:
            result = FRAM4_MUX_CS;
            break;
        case 8:
            result = MRAM1_MUX_CS;
            break;
        case 9:
            result = MRAM2_MUX_CS;
            break;
        case 10:
            result = MRAM3_MUX_CS;
            break;
        case 11:
            result = MRAM4_MUX_CS;
            break;
        case 12:
            result = SRAM1_MUX_CS;
            break;
        case 13:
            result = SRAM2_MUX_CS;
            break;
        case 14:
            result = SRAM3_MUX_CS;
            break;
        case 15:
            result = SRAM4_MUX_CS;
            break;
  }

  return result;
}

void setChipSelect(uint32_t chipNumber)
{

    IntMasterDisable();

    uint32_t pin0, pin1, pin2, pin3;

    // Identify Port and pin logic for selected chip
    uint8_t chipSelectInput = RetreiveCSCode(chipNumber);
    uint32_t chipSelectPortBase = RetreiveCSPort(chipNumber);

    switch (chipSelectPortBase)
    {
        case BOARD1_CS_PORT_BASE:
            pin0 = CS1_PIN_0;
            pin1 = CS1_PIN_1;
            pin2 = CS1_PIN_2;
            pin3 = CS1_PIN_3;
            break;
        case BOARD2_CS_PORT_BASE:
            pin0 = CS2_PIN_0;
            pin1 = CS2_PIN_1;
            pin2 = CS2_PIN_2;
            pin3 = CS2_PIN_3;
            break;
    }
    // write to mux input pins to enable selected chip
    GPIOPinWrite(chipSelectPortBase, pin0 | pin1 | pin2 | pin3, chipSelectInput);

    IntMasterEnable();
}
