//*****************************************************************************
//
// This file contains the functions for the SST25VF010A Flash
//
//*****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "MDE_Mux_Prototype.h"
#include "Access_Tools.h"
#include "S25FL256LA.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/ssi.h"

//*****************************************************************************
//
// Transmit the current sequence to the Flash.
//
//*****************************************************************************
void
FlashSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_number)
{
    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    uint32_t chip_port = RetrieveChipPort(chip_number);
    // Random chip selection for setting CS high
    if(chip_number <= 7) {
        chip_number_alt = chip_number + 7;
    } else {
        chip_number_alt = chip_number - 7;
    }
    // SPI port
    if(chip_port == CS0_PORT) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }
    //
    //  Write to the Flash
    //

    // Erase memory before write
  
#ifdef DEBUG
    char buf[80];
    sprintf(buf,"Beginning Flash erase\n\r");
    UARTSend((uint8_t*) buf, strlen(buf));
#endif
    // CS low
    MUXAddressSelect(chip_number);

    // Enable writing to the memory to erase chip
    SSIDataPut(SPI_base, FLASH_WRITE_ENABLE);
    while(SSIBusy(SPI_base))
    {
    }
    // CS high
    MUXAddressSelect(chip_number_alt);
    
    // CS low
    MUXAddressSelect(chip_number);
    // Issue erase command
    SSIDataPut(SPI_base, FLASH_CHIP_ERASE);
    while(SSIBusy(SPI_base))
    {
    }
    // Raise the CS
    MUXAddressSelect(chip_number_alt);
  
    // Wait for the Flash to erase before continuing
    uint32_t status_register = 1;
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }
  
    // Drop the CS
    MUXAddressSelect(chip_number);
  
    // Read the status register
    SSIDataPut(SPI_base, FLASH_RDSR);
    while(SSIBusy(SPI_base))
    {
    }
  
    SSIDataGet(SPI_base, &status_register);
  
#ifdef DEBUG
  sprintf(buf,"Beginning status register read\n\r");
  UARTSend((uint8_t*) buf, strlen(buf));
#endif
  
      // Loop while reading status until the WIP bit is zero
      // Could the program get stuck here if the chip dies? This bit is locked to 1 when P_ERR or W_ERR are 1, indicating an error in the chip.
      while(status_register & 0x01){
    
          SSIDataPut(SPI_base, 0x0);
          while(SSIBusy(SPI_base))
          {
          }
    
          SSIDataGet(SPI_base, &status_register);
    
      }
  
#ifdef DEBUG
  sprintf(buf,"Status register read finished, beginning program\n\r");
  UARTSend((uint8_t*) buf, strlen(buf));
#endif
  
      // Raise the CS
      MUXAddressSelect(chip_number_alt);
  
      // Sequence and loop variables
      unsigned char current_data = sequence_start;
      uint32_t data = current_data;
      uint32_t byte_num = 0;
  
      uint32_t current_address = 0;
      for(current_address = 0; current_address < FLASH_SIZE_BYTES; current_address += FLASH_PAGE_SIZE){
    
          // Drop the CS
          MUXAddressSelect(chip_number);
    
          // Enable writing to the memory
          SSIDataPut(SPI_base, FLASH_WRITE_ENABLE);
          while(SSIBusy(SPI_base))
          {
          }
          // CS high
          MUXAddressSelect(chip_number_alt);
          // CS low
          MUXAddressSelect(chip_number);

          // Send write command
          SSIDataPut(SPI_base, FLASH_WRITE);
    
          // We need to send the first address of the current 256 byte block
          SSIDataPut(SPI_base, current_address >> 24);
          SSIDataPut(SPI_base, current_address >> 16);
          SSIDataPut(SPI_base, current_address >> 8);
          SSIDataPut(SPI_base, current_address);
    
          // Wait for the transmission to complete
          while(SSIBusy(SPI_base))
          {
          }

          for(byte_num = 0; byte_num < FLASH_PAGE_SIZE; byte_num++)
          {
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
              SSIDataPut(SPI_base, data);
      
              // Increment the data while waiting for the SPI transmission
              current_data += sequence_offset;
      
              // Wait for the transmission to complete before moving on to the next byte
              while(SSIBusy(SPI_base))
              {
              }
          }
          // CS high
          MUXAddressSelect(chip_number_alt);
    
          // Wait for the Flash to write before starting again
          while(SSIDataGetNonBlocking(SPI_base, &temp))
          {
          }

          // CS low
          MUXAddressSelect(chip_number_alt);
    
          // Read the status register
          SSIDataPut(SPI_base, FLASH_RDSR);
          while(SSIBusy(SPI_base))
          {
          }
          SSIDataGet(SPI_base, &status_register);
          status_register = 1;
    
          // Loop while reading status until the WIP bit is zero
          while(status_register & 0x01){

              SSIDataPut(SPI_base, 0x0);
              while(SSIBusy(SPI_base))
              {
              }
      
              SSIDataGet(SPI_base, &status_register);
          }
    
#ifdef DEBUG
    if(current_address % 4096 == 0){
      sprintf(buf,"Finished write to page starting at byte %i\n\r", current_address);
      UARTSend((uint8_t*) buf, strlen(buf));
    }
#endif
    
        // Raise the CS
        MUXAddressSelect(chip_number_alt);
        // Move to next page;
      }
}

//*****************************************************************************
//
// Retrieve the sequence from the Flash and check it.
//
//*****************************************************************************
void
FlashSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_number)
{
    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    uint32_t chip_port = RetrieveChipPort(chip_number);
    // Random chip selection for setting CS high
    if(chip_number <= 7) {
        chip_number_alt = chip_number + 7;
    } else {
        chip_number_alt = chip_number - 7;
    }
    // SPI port
    if(chip_port == CS0_PORT) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }
    //
    // Clear out the FIFO recieve buffer
    //
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }
  
    //
    // Read from the Flash
    //
  
    MUXAddressSelect(chip_number);
  
    // Transmit the read command
    SSIDataPut(SPI_base, FLASH_READ);
  
    // Since this is sequential read, only need to send address zero
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
  
    // Wait for the transmission to complete
    while(SSIBusy(SPI_base))
    {
    }
  
    // Clear out the empty recieved data from the instruction transmission
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);

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
        SSIDataPut(SPI_base, 0x0);

        // Wait for the transaction to complete before moving on
        while(SSIBusy(SPI_base))
        {
        }

        // Read in the data
        SSIDataGet(SPI_base, &data);

        // All of this V V can be its own function.

        // Check if the byte is blank for determining unresponsiveness or reset it
        // if we did get data out
        if(data == 0x00 || data == 0xFF){
          blank_byte_count++;
        }else{
          blank_byte_count = 0;
        }

        CheckErrors(data, sequence, byte_num, chip_number);
    
        // Increment the data to the next number in the sequence
        sequence += sequence_offset;
    
    }

    // Bring CS high again, ending read
    MUXAddressSelect(chip_number_alt);

    // Update old current error for the next retrieve if the chip wasn't
    // unresponsive, and set its unresponsive flag low in case it fixed itself
    if(blank_byte_count < BLANK_BYTE_UNRESPONSIVE_COUNT)
    {
        old_current_error = current_error;
        chip_unresponsive &= ~(1 << chip_number);
    }
}
