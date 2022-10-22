//*****************************************************************************
//
// This file contains the functions for the MR25H10 MRAM
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "MDE_Payload_Prototype.h"
#include "MR25H40.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"

//*****************************************************************************
//
// Transmit the current sequence to the MRAM.
//
//*****************************************************************************
void
MRAMSequenceTransmit(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_number)
{
    //
    // Variables and necessary ports
    //
    uint8_t chip_number_alt;
    uint32_t SPI_base;

    // Chip port
    uint32_t chip_port = RetrieveChipPort(chip_number);
    // SPI port
    if(chip_port = CS0_PORT) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    // Random chip selection for setting CS high
    if(chip_number <= 7) {
        chip_number_alt = chip_number + 7;
    } else {
        chip_number_alt = chip_number - 7;
    }

    //
    // WREN latch
    //

    // CS low
    MUXAddressSelect(chip_number);

    // Transmit WREN command
    SSIDataPut(SPI_base, MRAM_WRITE_ENABLE);
    while(SSIBusy(SPI_base))
    {
    }

    // CS high to latch
    MuxAddressSelect(chip_number_alt);
    // CS low to write
    MuxAddressSelect(chip_number);

    // Transmit the write instruction
    SSIDataPut(SPI_base, MRAM_WRITE);
    while(SSIBusy(SPI_base))
    {
    }

    // Since this is sequential write, only need to send address zero
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
  
    // Declare variables for the loop
    unsigned char current_data = sequence_start;
    uint32_t data = current_data;
  
    uint32_t byte_num = 0;
    for(byte_num = 0; byte_num < MRAM_SIZE_BYTES; byte_num++){
    
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
    SSIDataPut(SPI_base, data);
    
    // Increment the data while waiting for the SPI transmission
    current_data += sequence_offset;
    
    // Wait for the transmission to complete before moving on to the next byte
    while(SSIBusy(SPI_base))
    {
    }
    
    }
  
    // CS High to end write
    MUXAddressSelect(chip_number);

    // CS Low for WRDI
    MUXAddressSelect(chip_number_alt);
  
    // Disable writing to the chip
    SSIDataPut(SPI_base, MRAM_WRITE_DISABLE);
    while(SSIBusy(SPI_base))
    {
    }
  
    // CS high to confirm
    MUXAddressSelect(chip_number_alt);
}


//*****************************************************************************
//
// Retrieve the sequence from the MRAM and check it.
//
//*****************************************************************************
void
MRAMSequenceRetrieve(unsigned char sequence_start, unsigned char sequence_offset, uint32_t chip_number)
{
    //
    // Variables and necessary ports
    //
    uint8_t chip_number_alt;
    uint32_t SPI_base;

    // Chip port
    uint32_t chip_port = RetrieveChipPort(chip_number);

    // SPI port
    if(chip_port = CS0_PORT) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    // Random chip selection for setting CS high
    if(chip_number <= 7) {
        chip_number_alt = chip_number + 7;
    } else {
        chip_number_alt = chip_number - 7;
    }

    //
    // Clear out the FIFO recieve buffer
    //
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }
  
    //
    // Read chip data
    //
    // Set CS low
    MUXAddressSelect(chip_number);

    // Transmit the read command
    SSIDataPut(SPI_base, MRAM_READ);
  
    // Since this is sequential read, only need to send address zero
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
  
    // Wait for the transaction to complete
    while(SSIBusy(SPI_NUM_BASE))
    {
    }
  
    // Clear out the empty recieved data from the instruction transmission
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
  
    // Declare variables for the loop
    unsigned char sequence = sequence_start;
    uint32_t data = 0;
    uint32_t blank_byte_count = 0;
  
    // Universal parts of the error data
    uint64_t data_error_header = (uint64_t)(ERROR_DATA_HEADER_VALUE && ERROR_DATA_HEADER_MASK) << ERROR_DATA_HEADER_SHIFT;
    uint64_t data_chip_num = (uint64_t)(chip_number && ERROR_DATA_CHIP_MASK) << ERROR_DATA_CHIP_SHIFT;
  
    uint64_t byte_num = 0;
    for(byte_num = 0; byte_num < MRAM_SIZE_BYTES; byte_num++){
    
        // Transmit 0, to create clock pulses
        SSIDataPut(SPI_base, 0x0);

        // Wait for the transaction to complete before moving on to the next byte
        while(SSIBusy(SPI_base))
        {
        }

        // Read in the data
        SSIDataGet(SPI_NUM_BASE, &data);
	
        // Check if the byte is blank for determining unresponsiveness or reset it
        // if we did get data out
        if(data == 0x00){
            blank_byte_count++;
        } else{
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
            for(bit_num = 0; bit_num < 8; bit_num++) {
                    if((wrong_bits >> bit_num) & 1) {
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
  
    // Bring CS high again, terminating read
    MuxAddressSelect(chip_number_alt);

    // Update old current error for the next retrieve if the chip wasn't
    // unresponsive, and set its unresponsive flag low in case it fixed itself
    if(blank_byte_count < BLANK_BYTE_UNRESPONSIVE_COUNT){
        old_current_error = current_error;
        chip_unresponsive &= ~(1 << chip_number);
    }

}
