//*****************************************************************************
//
// This file contains the functions for the BR25A1M EEPROM
//
//*****************************************************************************
/*
#include <stdint.h>
#include <stdbool.h>
#include "MDE_Payload_Prototype.h"
#include "BR25A1M.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"

//*****************************************************************************
//
// Transmit the current sequence to the EEPROM.
//
//*****************************************************************************
void
EEPROMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset)
{
  
  //
  // Write to the EEPROM.
  //
  
  // Sequence and loop variabless
  unsigned char current_data = sequence_start;
  uint32_t data = current_data;
  uint32_t byte_num = 0;
  
  uint32_t current_address = 0;
  for(current_address = 0; current_address < EEPROM_SIZE_BYTES; current_address += 256){
  
    // Drop the CS
    GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, 0x0);
    while(GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
    {
    }
    
    // Enable writing to the memory
    SSIDataPut(SPI_NUM_BASE, EEPROM_WRITE_ENABLE);
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
    
    // Cycle the CS
    GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, EEPROM_CHIP_SELECT_PIN);
    while(!GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
    {
    }
    GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, 0x0);
    while(GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
    {
    }
    
    // Send write command
    SSIDataPut(SPI_NUM_BASE, EEPROM_WRITE);
    
    // We need to send the first address of the current 256 byte block
    SSIDataPut(SPI_NUM_BASE, current_address >> 16);
    SSIDataPut(SPI_NUM_BASE, current_address >> 8);
    SSIDataPut(SPI_NUM_BASE, current_address);
    
    for(byte_num = 0; byte_num < EEPROM_MAX_WRITE; byte_num++){
      // Exit out if we've exceeded the maximum address
      if(current_address + byte_num > EEPROM_SIZE_BYTES){
        break;
      }
      
      // Assign the data to the proper format
      data = current_data;
      
      // Some fake errors for testing
      #ifdef SEEDERRORS
      if(current_address + byte_num == 65536){
        data = 0xFF;
      }
       #endif
      
      // Begin transmitting data
      SSIDataPut(SPI_NUM_BASE, data);
      
      // Increment the data while waiting for the SPI transmission
      current_data += sequence_offset;
      
      // Wait for the transmission to complete before moving on to the next byte
      while(SSIBusy(SPI_NUM_BASE))
      {
      }
      
    }
    
    // Pull CS high again
    GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, EEPROM_CHIP_SELECT_PIN);
    while(!GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
    {
    }
  
    // Wait for the EEPROM to write before starting again
    uint32_t status_register = 1;
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_NUM_BASE, &temp))
    {
    }
    while(status_register & 1){
      
      // Drop the CS
      GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, 0x0);
      while(GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
      {
      }
      
      // Read the status register
      SSIDataPut(SPI_NUM_BASE, EEPROM_RDSR);
      SSIDataPut(SPI_NUM_BASE, 0x0);
      while(SSIBusy(SPI_NUM_BASE))
      {
      }
      
      SSIDataGet(SPI_NUM_BASE, &temp);
      SSIDataGet(SPI_NUM_BASE, &status_register);
      
      // Raise the CS
      GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, EEPROM_CHIP_SELECT_PIN);
      while(!GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
      {
      }
      
    }
  
  }
}

//*****************************************************************************
//
// Retrieve the sequence from the EEPROM and check it.
//
//*****************************************************************************
void
EEPROMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset)
{

  //
  // Clear out the FIFO recieve buffer
  //
  uint32_t temp;
  while(SSIDataGetNonBlocking(SPI_NUM_BASE, &temp))
  {
  }
  
  //
  // Read from the EEPROM
  //
  
  // Set CS low
  GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
  {
  }  

  // Transmit the read command
  SSIDataPut(SPI_NUM_BASE, EEPROM_READ);
  
  // Since this is sequential read, only need to send address zero
  SSIDataPut(SPI_NUM_BASE, 0x0);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  
  // Wait for the transaction to complete
  while(SSIBusy(SPI_NUM_BASE))
  {
  }
  
  // Clear out the empty recieved data from the instruction transmission
  SSIDataGet(SPI_NUM_BASE, &temp);
  SSIDataGet(SPI_NUM_BASE, &temp);
  SSIDataGet(SPI_NUM_BASE, &temp);
  SSIDataGet(SPI_NUM_BASE, &temp);
  
  // Declare variables for the loop
  unsigned char sequence = sequence_start;
  uint32_t data = 0;
  uint32_t blank_byte_count = 0;  
  
  uint32_t byte_num = 0;
  for(byte_num = 0; byte_num < EEPROM_SIZE_BYTES; byte_num++){
    
    // Cancel this transaction if the chip is judged unresponsive, set its
    // unresponsive flag and reset the current error to its previous state
    // to stop spam
    if(blank_byte_count >= BLANK_BYTE_UNRESPONSIVE_COUNT){
      current_error = old_current_error;
      chip_unresponsive[EEPROM_NUMBER] = 1;
      break;
    }
    
    // Transmit 0, to create clock pulses
    SSIDataPut(SPI_NUM_BASE, 0x0);
    
    // Wait for the transaction to complete before moving on to the next byte
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
    
    // Read in the data
    SSIDataGet(SPI_NUM_BASE, &data);
    
    // Check if the byte is blank for determining unresponsiveness or reset it
    // if we did get data out
    if(data == 0x00){
      blank_byte_count++;
    }else{
      blank_byte_count = 0;
    }
    
    // Compare the recieved data to the sequence and create the error codes
    unsigned char wrong_bits = data ^ sequence;
    if(wrong_bits != 0){
      
      // Prepare the shifted message bytes
      uint32_t shifted_num = EEPROM_NUMBER << 20;
      uint32_t shifted_address = byte_num << 3;
      
      // Loop through and find the errored bits
      unsigned char bit_num = 0;
      for(bit_num = 0; bit_num < 8; bit_num++){
        if((wrong_bits >> bit_num) & 1){
          // Write the error code to the buffer
          error_buffer[current_error] = shifted_num | shifted_address | bit_num;
          current_error++;
          
          // Reset error counter in case of overflow
          if(current_error >= ERROR_BUFFER_MAX_SIZE){
            current_error = 0;
          }
          
        }
      }
    }
    
    // Increment the data to the next number in the sequence
    sequence += sequence_offset;
    
  }
  
  // Bring CS high again
  GPIOPinWrite(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN, EEPROM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT_PIN))
  {
  }
  
  // Update old current error for the next retrieve if the chip wasn't
  // unresponsive, and set its unresponsive flag low in case it fixed itself
  if(blank_byte_count < BLANK_BYTE_UNRESPONSIVE_COUNT){
    old_current_error = current_error;
    chip_unresponsive[EEPROM_NUMBER] = 0;
  }
  
}
//*/
