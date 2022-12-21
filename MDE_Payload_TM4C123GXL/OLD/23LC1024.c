//*****************************************************************************
//
// This file contains the functions for the 23LC1024 SRAM
//
//*****************************************************************************

/*
#include <stdint.h>
#include <stdbool.h>
#include "MDE_Payload_Prototype.h"
#include "23LC1024.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"

//*****************************************************************************
//
// Transmit the current sequence to the SRAM.
//
//*****************************************************************************
void
SRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset)
{
  
  //
  // Write to the SRAM.
  //
  
  // Drop the CS
  GPIOPinWrite(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN))
  {
  }
  
  // Transmit the write instruction
  SSIDataPut(SPI_NUM_BASE, SRAM_WRITE);
  
  // Since this is sequential write, only need to send address zero
  SSIDataPut(SPI_NUM_BASE, 0x0);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  SSIDataPut(SPI_NUM_BASE, 0x0);
  
  // Declare variables for the loop
  unsigned char current_data = sequence_start;
  uint32_t data = current_data;
  
  uint32_t byte_num = 0;
  for(byte_num = 0; byte_num < SRAM_SIZE_BYTES; byte_num++){
    
    // Assign the data to the proper format
    data = current_data;
    
    // Some fake errors for testing
#ifdef SEEDERRORS
    if(byte_num == 65536){
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
  GPIOPinWrite(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN, SRAM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN))
  {
  }
  
}

//*****************************************************************************
//
// Retrieve the sequence from the SRAM and check it.
//
//*****************************************************************************
void
SRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset)
{

  //
  // Clear out the FIFO recieve buffer
  //
  uint32_t temp;
  while(SSIDataGetNonBlocking(SPI_NUM_BASE, &temp))
  {
  }
  
  //
  // Read from the SRAM
  //
  
  // Set CS low
  GPIOPinWrite(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN, 0x0);
  while(GPIOPinRead(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN))
  {
  }  

  // Transmit the read command
  SSIDataPut(SPI_NUM_BASE, SRAM_READ);
  
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
  for(byte_num = 0; byte_num < SRAM_SIZE_BYTES; byte_num++){
    
    // Cancel this transaction if the chip is judged unresponsive, set its
    // unresponsive flag and reset the current error to its previous state
    // to stop spam
    if(blank_byte_count >= BLANK_BYTE_UNRESPONSIVE_COUNT){
      current_error = old_current_error;
      chip_unresponsive[SRAM_NUMBER] = 1;
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
      uint32_t shifted_num = SRAM_NUMBER << 20;
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
  GPIOPinWrite(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN, SRAM_CHIP_SELECT_PIN);
  while(!GPIOPinRead(SRAM_CHIP_SELECT_PORT, SRAM_CHIP_SELECT_PIN))
  {
  }
  
  // Update old current error for the next retrieve if the chip wasn't
  // unresponsive, and set its unresponsive flag low in case it fixed itself
  if(blank_byte_count < BLANK_BYTE_UNRESPONSIVE_COUNT){
    old_current_error = current_error;
    chip_unresponsive[SRAM_NUMBER] = 0;
  }
  
}
//*/
