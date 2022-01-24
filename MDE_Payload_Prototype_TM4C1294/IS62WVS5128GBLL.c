//*****************************************************************************
//
// This file contains the functions for the 23LC1024 SRAM
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "MDE_Payload_Prototype.h"
#include "IS62WVS5128GBLL.h"
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
SRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset,
                     uint32_t chip_select_port, uint32_t chip_select_pin)
{
  
  //
  // Write to the SRAM.
  //
  
  // Drop the CS
  GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
  while(GPIOPinRead(chip_select_port, chip_select_pin))
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
    // If seeded errors are needed, change the flag in the primary header
#ifdef SEEDERRORS
    if(byte_num == SEEDERRORS_ADDRESS){
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
  
}

//*****************************************************************************
//
// Retrieve the sequence from the SRAM and check it.
//
//*****************************************************************************
void
SRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset,
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
  // Read from the SRAM
  //
  
  // Set CS low
  GPIOPinWrite(chip_select_port, chip_select_pin, 0x0);
  while(GPIOPinRead(chip_select_port, chip_select_pin))
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
  
  // Universal parts of the error data
  uint64_t data_error_header = (uint64_t)(ERROR_DATA_HEADER_VALUE && ERROR_DATA_HEADER_MASK) << ERROR_DATA_HEADER_SHIFT;
  uint64_t data_chip_num = (uint64_t)(chip_number && ERROR_DATA_CHIP_MASK) << ERROR_DATA_CHIP_SHIFT;
  
  uint32_t byte_num = 0;
  for(byte_num = 0; byte_num < SRAM_SIZE_BYTES; byte_num++){
    
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
    
    // Cancel this transaction if the chip is judged unresponsive, set its
    // unresponsive flag and reset the current error to its previous state
    // to stop spam
    if(blank_byte_count >= BLANK_BYTE_UNRESPONSIVE_COUNT){
      current_error = old_current_error;
      chip_unresponsive |= 1 << chip_number;
      break;
    }
    
    // Compare the recieved data to the sequence and create the error codes
    unsigned char wrong_bits = data ^ sequence;
    if(wrong_bits != 0){
      
      // Prepare the data message bytes
      uint64_t data_address = ((uint64_t)byte_num & ERROR_DATA_BYTE_MASK) << ERROR_DATA_BYTE_SHIFT;
      
      // Loop through and find the errored bits
      unsigned char bit_num;
      unsigned char data_bit = 0;
      unsigned char data_direction = 0;
      for(bit_num = 0; bit_num < 8; bit_num++){
        if((wrong_bits >> bit_num) & 1){
          // Prepare the bit data
          data_bit = ((uint64_t)bit_num & ERROR_DATA_BIT_MASK) << ERROR_DATA_BIT_SHIFT;
          
          // Get the direction of the bit error
          data_direction = ((uint64_t)((data >> bit_num) & 1) & ERROR_DATA_DIRECTION_MASK) << ERROR_DATA_DIRECTION_SHIFT;
          
          
          // Write the error code to the buffer
          error_buffer[current_error] = data_error_header | data_chip_num | data_address | data_direction | data_bit;
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