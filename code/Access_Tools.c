//#include <MDE_Payload_TM4C123GXL_old.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "Access_Tools.h"
// #include "MB85RS2MTYPNF.h"
// #include "MR25H40.h"
// #include "IS62WVS5128GBLL.h"
// #include "S25FL256LA.h"
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


//*****************************************************************************
//
// Retrieves the associated Hex value for the CS Multiplexer
//
//*****************************************************************************
uint8_t
RetriveChipMuxCode(uint32_t chip_number){
  uint32_t result = 0;
  switch (chip_number){
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


//*****************************************************************************
//
// Retrieves the associated port for the chip number.
//
//*****************************************************************************
uint32_t
RetrieveChipPort(uint32_t chip_number){
  
  uint32_t result = 0;
  if (chip_number < 16){
    result = BOARD1_CS_PORT;
  }
  else if ( (chip_number >= 16) && (chip_number < 32) ){
    result  = BOARD1_CS_PORT;
  }
  else {
    result = 0;
  }
  return result;
}

//*****************************************************************************
//
// Mux select for both boards
//
//*****************************************************************************

// Board 1
void
MuxAddressSelect(uint8_t chipNumber)
{
    // Retrieve chip port
    uint32_t chipPort;
    chipPort = RetrieveChipPort(chipNumber);



    // Set pins for board 1/board 2
    if(chipPort == BOARD1_CS_PORT)
    {
        pin_1 = CS0_PIN_0; pin_2 = CS0_PIN_1; pin_3 = CS0_PIN_2; pin_4 = CS0_PIN_3;
    }
    else if(chipPort == BOARD2_CS_PORT)
    {
        pin_1 = CS1_PIN_0; pin_2 = CS1_PIN_1; pin_3 = CS1_PIN_2; pin_4 = CS1_PIN_3;
    }

    // Retrieve MUX address for selected chip
    uint8_t chip_select_addr = RetriveChipMuxCode(chipNumber);

    // Set chip select address to multiplexer
    // TODO: Entry 1, change this to how its done in the micro lab
    GPIOPinWrite(chipPort, pin_1, ( chip_select_addr & 0x1));
    GPIOPinWrite(chipPort, pin_2, ((chip_select_addr >> 1) & 0x1));
    GPIOPinWrite(chipPort, pin_3, ((chip_select_addr >> 2) & 0x1));
    GPIOPinWrite(chipPort, pin_4, ((chip_select_addr >> 3) & 0x1));
}


//*****************************************************************************
//
// Writes the given sequence to the chip indicated by chip number.
//
//*****************************************************************************
void
WriteToChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset){
  
  if(chip_number > MAX_CHIP_NUMBER){
    #ifdef DEBUG
      char buf[80];
      sprintf(buf,"Tried to write to chip %i but no such chip exists!\n\r", chip_number);
      UARTSend((uint8_t*) buf, strlen(buf));
    #endif
      return;
  }
  
  uint32_t chip_port = RetrieveChipPort(chip_number);
  uint8_t chip_mux = RetriveChipMuxCode(chip_number % 16);

  //drip chip select here (look at the chip select sequence in each of the sequence tranits)
  
  // TODO  Change the transmit function for multiplexer (TYLER)
  // for each board teh chiui numbering is the same
  if( (chip_number % 16) < 4){
    FlashSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if( (chip_number % 16) < 8){
    FRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if( (chip_number % 16) < 12){
    MRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if( (chip_number % 16) < 16){
    SRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }
}

//*****************************************************************************
//
// Reads and checks the sequence from a chip against the given sequence for the
// chip indicated by chip number.
//
//*****************************************************************************
void
ReadFromChip(uint32_t chip_number, unsigned char sequence_start, unsigned char sequence_offset){
  
  if(chip_number > MAX_CHIP_NUMBER){
#ifdef DEBUG
    char buf[80];
    sprintf(buf,"Tried to read from chip %i but no such chip exists!\n\r", chip_number);
    UARTSend((uint8_t*) buf, strlen(buf));
#endif
    return;
  }
  
  uint32_t chip_port = RetrieveChipPort(chip_number);
  uint32_t chip_pin = RetrieveChipPin(chip_number);
  
  if (chipNumber < 4) {
      FlashSequenceRetrieve(sequence_start, sequence_offset, chip_number);
  } else if (chipNumber < 8) {
      FRAMSequenceRetrieve(sequence_start, sequence_offset, chip_number)
  } else if (chipNumber < 12) {
      MRAMSequenceRetrieve(sequence_start, sequence_offset, chip_number)
  } else {
      SRAMSequenceRetrieve(sequence_start, sequence_offset, chip_number)
  }
}

//*****************************************************************************
//
// Enable the chip select ports and pins for every chip
//
//*****************************************************************************
void
EnableChipSelects(){

  // Chip Select Port
  SysCtlPeripheralEnable(CS0_SYSCTL_PORT);
  SysCtlPeripheralEnable(CS1_SYSCTL_PORT);

  //
  // Wait for peripheral to be enabled
  //
  while(!SysCtlPeripheralReady(CS0_SYSCTL_PORT) &&
        !SysCtlPeripheralReady(CS1_SYSCTL_PORT))
  {
  }
  
    //
    // Configure the pins
    //
    // Board 1
    GPIOPinTypeGPIOOutput(CS0_PORT, CS0_PIN_0);
    GPIOPinTypeGPIOOutput(CS0_PORT, CS0_PIN_1);
    GPIOPinTypeGPIOOutput(CS0_PORT, CS0_PIN_2);
    GPIOPinTypeGPIOOutput(CS0_PORT, CS0_PIN_3);

    //Board 2
    GPIOPinTypeGPIOOutput(CS1_PORT, CS1_PIN_0);
    GPIOPinTypeGPIOOutput(CS1_PORT, CS1_PIN_1);
    GPIOPinTypeGPIOOutput(CS1_PORT, CS1_PIN_2);
    GPIOPinTypeGPIOOutput(CS1_PORT, CS1_PIN_3);

    //
    // Set chip select lines low for now
    //
    // Board 1
    GPIOPinWrite(CS0_PORT, CS0_PIN_0, 0x0);
    GPIOPinWrite(CS0_PORT, CS0_PIN_1, 0x0);
    GPIOPinWrite(CS0_PORT, CS0_PIN_2, 0x0);
    GPIOPinWrite(CS0_PORT, CS0_PIN_3, 0x0);

    //Board 2
    GPIOPinWrite(CS1_PORT, CS1_PIN_0, 0x0);
    GPIOPinWrite(CS1_PORT, CS1_PIN_1, 0x0);
    GPIOPinWrite(CS1_PORT, CS1_PIN_2, 0x0);
    GPIOPinWrite(CS1_PORT, CS1_PIN_3, 0x0);
  }
}
