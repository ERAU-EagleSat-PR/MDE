//*****************************************************************************
//
// This file contains the functions for the SST25VF010A Flash
//
//*****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "MDE_Payload_TM4C123GXL.h"
#include "S25FL256LA.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"

//*****************************************************************************
//
// Transmit the current sequence to the Flash.
//
//*****************************************************************************
void
FlashSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset,
                      uint32_t chip_select_port, uint32_t chip_select_pin)
{
  
  //
  //  Write to the Flash
  //

  // Erase memory before write
  
#ifdef DEBUG
  char buf[80];
  sprintf(buf,"Beginning Flash erase\n\r");
  UARTSend((uint8_t*) buf, strlen(buf));
#endif
  
  // Drop the CS
  GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
  while(GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }
    
  // Enable writing to the memory
  SSIDataPut(SPI_NUM_BASE, FLASH_WRITE_ENABLE);
  while(SSIBusy(SPI_NUM_BASE))
  {
  }
  
  // Cycle the CS
  GPIOPinWrite(chip_select_port, chip_select_pin, chip_select_pin);
  while(!GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }
  
  // Drop the CS
  GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
  while(GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }
  
  SSIDataPut(SPI_NUM_BASE, FLASH_CHIP_ERASE);
  while(SSIBusy(SPI_NUM_BASE))
  {
  }
  
  // Raise the CS
  GPIOPinWrite(chip_select_port, chip_select_pin, chip_select_pin);
  while(!GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }
  
  // Wait for the Flash to erase before continuing
  uint32_t status_register = 1;
  uint32_t temp;
  while(SSIDataGetNonBlocking(SPI_NUM_BASE, &temp))
  {
  }
  
  // Drop the CS
  GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
  while(GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }
  
  // Read the status register
  SSIDataPut(SPI_NUM_BASE, FLASH_RDSR);
  while(SSIBusy(SPI_NUM_BASE))
  {
  }
  
  SSIDataGet(SPI_NUM_BASE, &temp);
  
#ifdef DEBUG
  sprintf(buf,"Beginning status register read\n\r");
  UARTSend((uint8_t*) buf, strlen(buf));
#endif
  
  // Loop while reading status until the WIP bit is zero
  while(status_register & 0x01){
    
    SSIDataPut(SPI_NUM_BASE, 0x0);
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
    
    SSIDataGet(SPI_NUM_BASE, &status_register);
    
  }
  
#ifdef DEBUG
  sprintf(buf,"Status register read finished, beginning program\n\r");
  UARTSend((uint8_t*) buf, strlen(buf));
#endif
  
  // Raise the CS
  GPIOPinWrite(chip_select_port, chip_select_pin, chip_select_pin);
  while(!GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }
  
  // Sequence and loop variabless
  unsigned char current_data = sequence_start;
  uint32_t data = current_data;
  uint32_t byte_num = 0;
  
  uint32_t current_address = 0;
  for(current_address = 0; current_address < FLASH_SIZE_BYTES; current_address += 256){
    
    // Drop the CS
    GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
    while(GPIOPinRead(chip_select_port, chip_select_pin))
    {
    }
    
    // Enable writing to the memory
    SSIDataPut(SPI_NUM_BASE, FLASH_WRITE_ENABLE);
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
    
    // Cycle the CS
    GPIOPinWrite(chip_select_port, chip_select_pin, chip_select_pin);
    while(!GPIOPinRead(chip_select_port, chip_select_pin))
    {
    }
    GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
    while(GPIOPinRead(chip_select_port, chip_select_pin))
    {
    }
    
    // Send write command
    SSIDataPut(SPI_NUM_BASE, FLASH_WRITE);
    
    // We need to send the first address of the current 256 byte block
    SSIDataPut(SPI_NUM_BASE, current_address >> 24);
    SSIDataPut(SPI_NUM_BASE, current_address >> 16);
    SSIDataPut(SPI_NUM_BASE, current_address >> 8);
    SSIDataPut(SPI_NUM_BASE, current_address);
    
    // Wait for the transmission to complete
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
    
    for(byte_num = 0; byte_num < FLASH_PAGE_SIZE; byte_num++){
      // Exit out if we've exceeded the maximum address
      if(current_address + byte_num > FLASH_SIZE_BYTES){
        break;
      }
      
      // Assign the data to the proper format
      data = current_data;
      
      // Some fake errors for testing
      // If seeded errors are needed, change the flag in the primary header
#ifdef SEEDERRORS
      if(current_address + byte_num == SEEDERRORS_ADDRESS){
        data = SEEDERRORS_VALUE;
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
    GPIOPinWrite(chip_select_port, chip_select_pin, chip_select_pin);
    while(!GPIOPinRead(chip_select_port, chip_select_pin))
    {
    }
    
    // Wait for the Flash to write before starting again
    while(SSIDataGetNonBlocking(SPI_NUM_BASE, &temp))
    {
    }
    
    // Drop the CS
    GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
    while(GPIOPinRead(chip_select_port, chip_select_pin))
    {
    }
    
    // Read the status register
    SSIDataPut(SPI_NUM_BASE, FLASH_RDSR);
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
    SSIDataGet(SPI_NUM_BASE, &temp);
    status_register = 1;
    
    // Loop while reading status until the WIP bit is zero
    while(status_register & 0x01){
      
      SSIDataPut(SPI_NUM_BASE, 0x0);
      while(SSIBusy(SPI_NUM_BASE))
      {
      }
      
      SSIDataGet(SPI_NUM_BASE, &status_register);
      
    }
    
#ifdef DEBUG
    if(current_address % 4096 == 0){
      sprintf(buf,"Finished write to page starting at byte %i\n\r", current_address);
      UARTSend((uint8_t*) buf, strlen(buf));
    }
#endif
    
    // Raise the CS
    GPIOPinWrite(chip_select_port, chip_select_pin, chip_select_pin);
    while(!GPIOPinRead(chip_select_port, chip_select_pin))
    {
    }
    
  }
  
}

//*****************************************************************************
//
// Retrieve the sequence from the Flash and check it.
//
//*****************************************************************************
void
FlashSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset,
                      uint32_t chip_select_port, uint32_t chip_select_pin, uint32_t chip_number)
{
  
  //
  // Clear out the FIFO recieve buffer
  //
  uint32_t temp;
  while(SSIDataGetNonBlocking(SPI_NUM_BASE, &temp))
  {
  }
  
  //
  // Read from the Flash
  //
  
  // Set CS low
  GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
  while(GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }  
  
  // Transmit the read command
  SSIDataPut(SPI_NUM_BASE, FLASH_READ);
  
  // Since this is sequential read, only need to send address zero
  SSIDataPut(SPI_NUM_BASE, 0x0);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  
  // Wait for the transmission to complete
  while(SSIBusy(SPI_NUM_BASE))
  {
  }
  
  // Clear out the empty recieved data from the instruction transmission
  SSIDataGet(SPI_NUM_BASE, &temp);
  SSIDataGet(SPI_NUM_BASE, &temp);
  SSIDataGet(SPI_NUM_BASE, &temp);
  SSIDataGet(SPI_NUM_BASE, &temp);
  SSIDataGet(SPI_NUM_BASE, &temp);

  // Declare variables for the loop
  unsigned char sequence = sequence_start;
  uint32_t data = 0;
  uint32_t blank_byte_count = 0;
  
  uint32_t byte_num = 0;
  for(byte_num = 0; byte_num < FLASH_SIZE_BYTES; byte_num++){
    
    // Cancel this transaction if the chip is judged unresponsive, set its
    // unresponsive flag and reset the current error to its previous state
    // to stop spam
    if(blank_byte_count >= BLANK_BYTE_UNRESPONSIVE_COUNT){
      current_error = old_current_error;
      chip_unresponsive |= 1 << chip_number;
      break;
    }
    
    // Transmit 0, to create clock pulses
    SSIDataPut(SPI_NUM_BASE, 0x0);
    
    // Wait for the transaction to complete before moving on
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
    
    // Read in the data
    SSIDataGet(SPI_NUM_BASE, &data);
    
    // Check if the byte is blank for determining unresponsiveness or reset it
    // if we did get data out
    if(data == 0x00 || data == 0xFF){
      blank_byte_count++;
    }else{
      blank_byte_count = 0;
    }

    // Compare the recieved data to the sequence and create the error codes
    unsigned char wrong_bits = data ^ sequence;
    if(wrong_bits != 0){
      
      // Prepare the shifted message bytes
      uint32_t shifted_num = chip_number << 20;
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
  GPIOPinWrite(chip_select_port, chip_select_pin, chip_select_pin);
  while(!GPIOPinRead(chip_select_port, chip_select_pin))
  {
  }
  
  // Update old current error for the next retrieve if the chip wasn't
  // unresponsive, and set its unresponsive flag low in case it fixed itself
  if(blank_byte_count < BLANK_BYTE_UNRESPONSIVE_COUNT){
    old_current_error = current_error;
    chip_unresponsive &= ~(1 << chip_number);
  }
  
}