
//*****************************************************************************
//
// This file contains the functions for the 23LC1024 SRAM
//
//*****************************************************************************

// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Tools
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

// Additional Includes
#include "SRAMfunc.h"
#include "source/multiplexer.h"
#include "source/mde.h"
#include "source/devtools.h"

//*****************************************************************************
//
// Read and return the SRAM status register.
//
//*****************************************************************************

uint8_t
SRAMStatusRead(uint32_t chip_number)
{
    // Read and return the contents of the SRAM status register
    // SHOULD BE ALL ZEROES.
    //
    // Chip numbers 12-15 for SRAM

    // Variables and necessary ports
    //
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    uint32_t data;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    // Random chip selection for setting CS high
    // SRAM is always 12 to 15, so subtracting 7 is fine.
    chip_number_alt = chip_number - 7;

    // CS Low for RDMR
    SetChipSelect(chip_number);

    // Transmit RDMR command
    SSIDataPut(SPI_base, SRAM_RDMR);
    while(SSIBusy(SPI_base))
    {
    }

    // Clear out the empty received data from the instruction transmission
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Send a clock pulse and retrieve the status register
    SSIDataPut(SPI_base, 0x00);
    SSIDataGet(SPI_base, &data);

    // CS high to end read
    SetChipSelect(chip_number_alt);

    //Output
    uint8_t data8 = data;
    return data8;
}

//*****************************************************************************
//
// Transmit the current sequence to the SRAM.
//
//*****************************************************************************

// TODO: SRAM is ending up with a 0 at the beginning of its data, not sure yet if this is happening in writing or reading, adding another SPIdataget to the data retrieval to remove the 0 freezes the program


// The SRAM is split into two dies. To write across them we would need to end and restart the write at that point (40000h). Same for the read.

void
SRAMSequenceTransmit(uint8_t current_cycle, uint32_t chip_number)
{
    // Transmit a sequence to the SRAM depending on current cycle.
    //
    // Chip numbers 12-15 for SRAM
    // Current cycle 0 for all 0s, 1 for all 1s (255 each byte)

    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    //uint32_t chip_port = RetrieveChipPort(chip_number);

    // Random chip selection for setting CS high
    // SRAM are always 12 to 15, so fine to subtract 7.
    chip_number_alt = chip_number - 7;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    // Set data depending on the cycle
    uint32_t data = current_cycle;

    //
    // Write to the SRAM.
    //

    // Drop the CS
    SetChipSelect(chip_number);

    // Transmit the write instruction
    SSIDataPut(SPI_base, SRAM_WRITE);

    // Since this is sequential write, only need to send address zero
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);

    // Declare variable for the loop
    uint32_t byte_num = 0;

    for(byte_num = 0; byte_num < SRAM_SIZE_BYTES; byte_num++)
    {
        // Some fake errors for testing
        // If seeded errors are needed, change the flag in the primary header
#ifdef SEEDERRORS
            if(byte_num == SEEDERRORS_ADDRESS)
            {
                data = SEEDERRORS_VALUE;
            }
#endif

       // Begin transmitting data
       SSIDataPut(SPI_base, data);

       // Wait for the transmission to complete before moving on to the next byte
       while(SSIBusy(SPI_base))
       {}
    }
    // Pull CS high again, unlatching WREN
    SetChipSelect(chip_number_alt);
}

//*****************************************************************************
//
// Retrieve the sequence from the SRAM and check it.
//
//*****************************************************************************
void
SRAMSequenceRetrieve(uint8_t current_cycle, uint32_t chip_number)
{
    // Transmit a sequence to the SRAM depending on current cycle.
    //
    // Chip numbers 12-15 for SRAM
    // Current cycle 0 for all 0s, 1 for all 1s (255 each byte)
    // Currently operating in byte mode

    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    //uint32_t chip_port = RetrieveChipPort(chip_number);

    // Random chip selection for setting CS high
    // SRAM are always 12 to 15, so fine to subtract 7.
    chip_number_alt = chip_number - 7;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    //
    // Read from the SRAM
    //

    // Set CS low
    SetChipSelect(chip_number);

    // Transmit the read command
    SSIDataPut(SPI_base, SRAM_READ);

    // Since this is sequential read, only need to send address zero (24bit)
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);

    // TODO: Needs 8 dummy cycles according to datasheet? look back later.

    // Wait for the transaction to complete
    while(SSIBusy(SPI_base))
    {
    }

    // Clear out the FIFO receive buffer
    //
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Declare variables for the loop
    uint32_t byte_num = 0;
    uint32_t data = 0;

    // Loop through all data

    for(byte_num = 0; byte_num < SRAM_SIZE_BYTES; byte_num++){

        // Transmit 0, to create clock pulses
        SSIDataPut(SPI_base, 0x0);

        // Wait for the transaction to complete before moving on to the next byte
        while(SSIBusy(SPI_base))
        {
        }

        // Read in the data
        SSIDataGet(SPI_base, &data);
#ifdef DEBUG
        char str[12];
        sprintf(str, "%d ", data);
        UARTDebugSend((uint8_t*) str, strlen(str));
#endif
    }
    // Bring CS high, ending read
    SetChipSelect(chip_number_alt);

#ifdef DEBUG
        char str[5];
        sprintf(str, "\r\n");
        UARTDebugSend((uint8_t*) str, strlen(str));
#endif
}
