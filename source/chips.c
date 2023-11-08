/*
 * chips.c
 *
 *  Created on: Dec 29, 2022
 *      Author: Calvin
 */

// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware Files
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

// Driver Files
#include <driverlib/debug.h>
#include <driverlib/fpu.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/rom.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>

// Additional Includes
#include "source/chips.h"
#include "chipDrivers/FLASHfunc.h"
#include "chipDrivers/FRAMfunc.h"
#include "chipDrivers/MRAMfunc.h"
#include "chipDrivers/SRAMfunc.h"
#include "source/devtools.h"

//*****************************************************************************
//
// Writes the given sequence to the chip indicated by chip number.
//
//*****************************************************************************
// TODO

void
WriteToChip(uint32_t current_cycle, uint32_t chip_number)
{
    // Given a chip number 0 - 31, this function calls the correct chip function
    // and writes the selected cycle to it.
    if(chip_number >= MAX_CHIP_NUMBER){
#ifdef DEBUG
        char buf[80];
        sprintf(buf,"Tried to write to chip %i but no such chip exists!\n\r", chip_number);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
        return;
    }

    // Write to chips board 1 or 2
    if( (chip_number % 16) < 4){
        FlashSequenceTransmit(current_cycle, chip_number);
    }else if( (chip_number % 16) < 8){
        FRAMSequenceTransmit(current_cycle, chip_number);
    }else if( (chip_number % 16) < 12){
        MRAMSequenceTransmit(current_cycle, chip_number);
    }else {
        SRAMSequenceTransmit(current_cycle, chip_number);
    }
}


//*****************************************************************************
//
// Reads and checks the sequence from a chip against the given sequence for the
// chip indicated by chip number.
//
//*****************************************************************************
// TODO

void
ReadFromChip(uint32_t current_cycle, uint32_t chip_number)
{
    // Given a chip number 0 - 31, this function calls the correct chip function
    // and reads the data, comparing it to to the selected cycle.
  if(chip_number >= MAX_CHIP_NUMBER){
#ifdef DEBUG
    char buf[80];
    sprintf(buf,"Tried to read from chip %i but no such chip exists!\n\r", chip_number);
    UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
    return;
  }

  if ( (chip_number % 16) < 4) {
      FlashSequenceRetrieve(current_cycle, chip_number);
  } else if ( (chip_number % 16) < 8) {
      FRAMSequenceRetrieve(current_cycle, chip_number);
  } else if ( (chip_number % 16) < 12) {
      MRAMSequenceRetrieve(current_cycle, chip_number);
  } else {
      SRAMSequenceRetrieve(current_cycle, chip_number);
  }
}
