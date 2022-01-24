#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "MDE_Payload_Prototype.h"
#include "Access_Tools.h"
#include "M95M02.h"
#include "CY15B104Q.h"
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
// Retrieves the associated pin for the chip number.
//
//*****************************************************************************
uint32_t
RetrieveChipPin(uint32_t chip_number){

  uint32_t result = 0;
  switch (chip_number)
  {
  case 0:
    result = EEPROM_PIN_0;
    break;
  case 1:
    result = EEPROM_PIN_1;
    break;
  case 2:
    result = EEPROM_PIN_2;
    break;
  case 3:
    result = EEPROM_PIN_3;
    break;
  case 4:
    result = EEPROM_PIN_4;
    break;
  case 5:
    result = EEPROM_PIN_5;
    break;
  case 6:
    result = EEPROM_PIN_6;
    break;
  case 7:
    result = EEPROM_PIN_7;
    break;
  case 8:
    result = FLASH_PIN_0;
    break;
  case 9:
    result = FLASH_PIN_1;
    break;
  case 10:
    result = FLASH_PIN_2;
    break;
  case 11:
    result = FLASH_PIN_3;
    break;
  case 12:
    result = FLASH_PIN_4;
    break;
  case 13:
    result = FLASH_PIN_5;
    break;
  case 14:
    result = FLASH_PIN_6;
    break;
  case 15:
    result = FLASH_PIN_7;
    break;
  case 16:
    result = FRAM_PIN_0;
    break;
  case 17:
    result = FRAM_PIN_1;
    break;
  case 18:
    result = FRAM_PIN_2;
    break;
  case 19:
    result = FRAM_PIN_3;
    break;
  case 20:
    result = FRAM_PIN_4;
    break;
  case 21:
    result = FRAM_PIN_5;
    break;
  case 22:
    result = FRAM_PIN_6;
    break;
  case 23:
    result = FRAM_PIN_7;
    break;
  case 24:
    result = MRAM_PIN_0;
    break;
  case 25:
    result = MRAM_PIN_1;
    break;
  case 26:
    result = MRAM_PIN_2;
    break;
  case 27:
    result = MRAM_PIN_3;
    break;
  case 28:
    result = MRAM_PIN_4;
    break;
  case 29:
    result = MRAM_PIN_5;
    break;
  case 30:
    result = MRAM_PIN_6;
    break;
  case 31:
    result = MRAM_PIN_7;
    break;
  case 32:
    result = SRAM_PIN_0;
    break;
  case 33:
    result = SRAM_PIN_1;
    break;
  case 34:
    result = SRAM_PIN_2;
    break;
  case 35:
    result = SRAM_PIN_3;
    break;
  case 36:
    result = SRAM_PIN_4;
    break;
  case 37:
    result = SRAM_PIN_5;
    break;
  case 38:
    result = SRAM_PIN_6;
    break;
  case 39:
    result = SRAM_PIN_7;
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
  switch (chip_number)
  {
  case 0:
    result = EEPROM_PORT_0;
    break;
  case 1:
    result = EEPROM_PORT_1;
    break;
  case 2:
    result = EEPROM_PORT_2;
    break;
  case 3:
    result = EEPROM_PORT_3;
    break;
  case 4:
    result = EEPROM_PORT_4;
    break;
  case 5:
    result = EEPROM_PORT_5;
    break;
  case 6:
    result = EEPROM_PORT_6;
    break;
  case 7:
    result = EEPROM_PORT_7;
    break;
  case 8:
    result = FLASH_PORT_0;
    break;
  case 9:
    result = FLASH_PORT_1;
    break;
  case 10:
    result = FLASH_PORT_2;
    break;
  case 11:
    result = FLASH_PORT_3;
    break;
  case 12:
    result = FLASH_PORT_4;
    break;
  case 13:
    result = FLASH_PORT_5;
    break;
  case 14:
    result = FLASH_PORT_6;
    break;
  case 15:
    result = FLASH_PORT_7;
    break;
  case 16:
    result = FRAM_PORT_0;
    break;
  case 17:
    result = FRAM_PORT_1;
    break;
  case 18:
    result = FRAM_PORT_2;
    break;
  case 19:
    result = FRAM_PORT_3;
    break;
  case 20:
    result = FRAM_PORT_4;
    break;
  case 21:
    result = FRAM_PORT_5;
    break;
  case 22:
    result = FRAM_PORT_6;
    break;
  case 23:
    result = FRAM_PORT_7;
    break;
  case 24:
    result = MRAM_PORT_0;
    break;
  case 25:
    result = MRAM_PORT_1;
    break;
  case 26:
    result = MRAM_PORT_2;
    break;
  case 27:
    result = MRAM_PORT_3;
    break;
  case 28:
    result = MRAM_PORT_4;
    break;
  case 29:
    result = MRAM_PORT_5;
    break;
  case 30:
    result = MRAM_PORT_6;
    break;
  case 31:
    result = MRAM_PORT_7;
    break;
  case 32:
    result = SRAM_PORT_0;
    break;
  case 33:
    result = SRAM_PORT_1;
    break;
  case 34:
    result = SRAM_PORT_2;
    break;
  case 35:
    result = SRAM_PORT_3;
    break;
  case 36:
    result = SRAM_PORT_4;
    break;
  case 37:
    result = SRAM_PORT_5;
    break;
  case 38:
    result = SRAM_PORT_6;
    break;
  case 39:
    result = SRAM_PORT_7;
    break;
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
  uint32_t chip_pin = RetrieveChipPin(chip_number);
  
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
  
  // Write all the chip selects high now
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
