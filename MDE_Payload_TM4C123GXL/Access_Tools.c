#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "MDE_Payload_TM4C123GXL.h"
#include "Access_Tools.h"
#include "MB85RS2MTYPNF.h"
#include "MR25H40.h"
#include "IS62WVS5128GBLL.h"
#include "S25FL256LA.h"
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
  if ( (chip_number <= 15) && (chip_number <= 15) ){
    result = BOARD1_CS_PORT;
  }
  else if ( (chip_number >= 15) && (chip_number < 32) ){
    result  = BOARD2_CS_PORT;
  }
  else {
    result = 0;
  }
  return result;
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
  
  if(chip_number < 8){
    EEPROMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if(chip_number < 16){
    FlashSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if(chip_number < 24){
    FRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if(chip_number < 32){
    MRAMSequenceTransmit(sequence_start, sequence_offset, chip_port, chip_pin);
  }else if(chip_number < 40){
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
  
  if(chip_number < 8){
    EEPROMSequenceRetrieve(sequence_start, sequence_offset, chip_port, chip_pin, chip_number);
  }else if(chip_number < 16){
    FlashSequenceRetrieve(sequence_start, sequence_offset, chip_port, chip_pin, chip_number);
  }else if(chip_number < 24){
    FRAMSequenceRetrieve(sequence_start, sequence_offset, chip_port, chip_pin, chip_number);
  }else if(chip_number < 32){
    MRAMSequenceRetrieve(sequence_start, sequence_offset, chip_port, chip_pin, chip_number);
  }else if(chip_number < 40){
    SRAMSequenceRetrieve(sequence_start, sequence_offset, chip_port, chip_pin, chip_number);
  }
}

//*****************************************************************************
//
// Enable the chip select ports and pins for every chip
//
//*****************************************************************************
void
EnableChipSelects(){
  
  SysCtlPeripheralEnable(CS_SYSCTL_PORT_0);
  SysCtlPeripheralEnable(CS_SYSCTL_PORT_1);
  SysCtlPeripheralEnable(CS_SYSCTL_PORT_2);
  SysCtlPeripheralEnable(CS_SYSCTL_PORT_3);
  SysCtlPeripheralEnable(CS_SYSCTL_PORT_4);
  SysCtlPeripheralEnable(CS_SYSCTL_PORT_5);

  //
  // Wait for all the peripherals to be enabled
  //
  while(!SysCtlPeripheralReady(CS_SYSCTL_PORT_0) ||
        !SysCtlPeripheralReady(CS_SYSCTL_PORT_1) ||
        !SysCtlPeripheralReady(CS_SYSCTL_PORT_2) ||
        !SysCtlPeripheralReady(CS_SYSCTL_PORT_3) ||
        !SysCtlPeripheralReady(CS_SYSCTL_PORT_4) ||
        !SysCtlPeripheralReady(CS_SYSCTL_PORT_5))
  {
  }
  
  // Write all the chip selects high (OFF) now
  uint32_t chip_number;
  uint32_t chip_port;
  uint32_t chip_pin;
  for(chip_number = 0; chip_number < MAX_CHIP_NUMBER; chip_number++){
    chip_port = RetrieveChipPort(chip_number);
    chip_pin = RetrieveChipPin(chip_number);
    
    // Protect against non-configured chips
    if(chip_port == 0 || chip_pin == 0){
      continue;
    }
    
    GPIOPinTypeGPIOOutput(chip_port, chip_pin);
    
    //
    // Set chip select lines high for now
    //
    GPIOPinWrite(chip_port, chip_pin, chip_pin);
  }
}
