
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
#include "source/UART0_func.h"
#include "source/bit_errors.h"

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

    //  Initial CS high
    SetChipSelect(chip_number_alt);

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
    SSIDataGetNonBlocking(SPI_base, &temp);

    // Send a clock pulse and retrieve the status register
    SSIDataPut(SPI_base, 0x00);
    SysCtlDelay(10);
    SSIDataGetNonBlocking(SPI_base, &data);

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
        data = current_cycle;

#ifdef DEBUG // Seeded errors in debug mode
        if(seedErrors == 1 && (byte_num % SEEDERRORS_ADDRESS) == 0)
            data = SEEDERRORS_VALUE;
#endif

        // Begin transmitting data
        SSIDataPut(SPI_base, data);

        // Wait for the transmission to complete before moving on to the next byte
        while(SSIBusy(SPI_base))
        {
        }
        if(byte_num == 262140) // SRAM Die Boundary - must re-initiate write when crossing in to second memory cell.
        {             // ^ Magic value based on SRAM address/page organization. Boundary is 0x3FFFF, number is FFFF*4pages.
            // Bring CS high, ending write
            SetChipSelect(chip_number_alt);
            // Set CS low
            SetChipSelect(chip_number);

            // Transmit the write command
            SSIDataPut(SPI_base, SRAM_WRITE);

            // Second half of sequential write; send address for the second die. Address 0x040000 for second die.
            SSIDataPut(SPI_base, 0x04);
            SSIDataPut(SPI_base, 0x00);
            SSIDataPut(SPI_base, 0x00);

            // Wait for the transaction to complete
            while(SSIBusy(SPI_base))
            {
            }
        }
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

#ifdef DEBUG
    char buf[10];
    uint8_t bufSize = 10;
#endif

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
    // 8 dummy cycles per data sheet.
    SSIDataPut(SPI_base, 0x0);

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
        SysCtlDelay(10);
        // Read in the data
        SSIDataGetNonBlocking(SPI_base, &data);
        // Send data to be checked and packaged
        CheckErrors(chip_number, byte_num, data, current_cycle);

        if(byte_num == 262140) // SRAM Die Boundary - must re-initiate read in second memory cell.
        {             // ^ Magic value based on SRAM address/page organization. Boundary is 0x3FFFF
            // Bring CS high, ending read
            SetChipSelect(chip_number_alt);
            // Set CS low
            SetChipSelect(chip_number);

            // Transmit the read command
            SSIDataPut(SPI_base, SRAM_READ);

            // Second half of sequential read; send address for the second die. Address 0x040000 for second die.
            SSIDataPut(SPI_base, 0x04);
            SSIDataPut(SPI_base, 0x00);
            SSIDataPut(SPI_base, 0x00);
            // 8 dummy cycles per data sheet.
            SSIDataPut(SPI_base, 0x0);

            // Wait for the transaction to complete
            while(SSIBusy(SPI_base))
            {
            }

            // Clear out the FIFO receive buffer
            //
            while(SSIDataGetNonBlocking(SPI_base, &temp))
            {
            }
        }

#ifdef DEBUG
        if(byte_num < 10)
        {
            snprintf(buf,bufSize, "%u ", data);
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
#endif
    }
    // Bring CS high, ending read
    SetChipSelect(chip_number_alt);
}
