//*****************************************************************************
//
// This file contains the functions for the MR25H10 MRAM
//
//*****************************************************************************

// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware/Drivers
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"

// Additional Includes
#include "MRAMfunc.h"
#include "AccessTools.h"


void
MRAMStatusRead(uint8_t chip_number)
{
    // TODO: Unlike the other chip types, the MRAM chip does not have an RDID OP-code
    // This function will compare certain bits in the status register byte to a pre-set value. certain bits are used in the operation of the chip and will be ignored here.
}
void MRAMStatusPrepare(void)
{
    // TODO: This function will set certain bits in the MRAM status register to 1 for the purposes of device reliability checking. It must first read the register to use its value as a base
    // to apply our custom bits to, in case any of the bits are already set (they should not be but it is a safety feature)
    // This function should apply to all MRAM chips at the start of the program, on any restarts or interruptions, and probably before a full write cycle has been completed. it should not get stuck if a chip is dead, obviously
}


//*****************************************************************************
//
// Transmit the current sequence to the MRAM.
//
//*****************************************************************************
void
MRAMSequenceTransmit(uint8_t currentCycle, uint32_t chip_number)
{
    // Transmit the selected sequence to MRAM based on current cycle
    //
    // Chip number value 8-11 for MRAM
    // Current cycle 0 for all 0s, 1 for all 1s (255 each byte)

    // Variables and necessary ports
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    //uint32_t chip_port = RetrieveChipPort(chip_number);

    // SPI port
    //if(chip_port == CS0_PORT) {
    SPI_base = SPI0_NUM_BASE;
    //} else {
    //    SPI_base = SPI1_NUM_BASE;
    //}

    // Random chip selection for setting CS high
    // MRAM is always 8 to 11, so subtracting 7 is fine.
    chip_number_alt = chip_number - 7;

    // Set data depending on the cycle
    uint32_t data;
    if(currentCycle == 1) {
         data = 255;
    } else {
        data = 0;
    }
        uint32_t temp;
        while(SSIDataGetNonBlocking(SPI_base, &temp))
        {
        }
    //
    // WREN latch
    //

    // CS low
    SetChipSelect(chip_number);

    // Transmit WREN command
    SSIDataPut(SPI_base, MRAM_WRITE_ENABLE);
    while(SSIBusy(SPI_base))
    {
    }
    // CS high to latch
    SetChipSelect(chip_number_alt);

    // CS low to write
    SetChipSelect(chip_number);

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
    uint32_t byte_num = 0;
    // Loop through all of memory
    for(byte_num = 0; byte_num < MRAM_SIZE_BYTES; byte_num++){

        // Some fake errors for testing
        // If seeded errors are needed, change the flag in the primary header
#ifdef SEEDERRORS
        if(byte_num == SEEDERRORS_ADDRESS){
            data = SEEDERRORS_VALUE;
        }
#endif

        // Begin transmitting data
        SSIDataPut(SPI_base, data);

        // Wait for the transmission to complete before moving on to the next byte
        while(SSIBusy(SPI_base))
        {
        }

    }

    // CS High to end write
    SetChipSelect(chip_number);

    // CS Low for WRDI
    SetChipSelect(chip_number_alt);

    // Disable writing to the chip
    SSIDataPut(SPI_base, MRAM_WRITE_DISABLE);
    while(SSIBusy(SPI_base))
    {
    }

    // CS high to confirm
    SetChipSelect(chip_number_alt);
}


//*****************************************************************************
//
// Retrieve the sequence from the MRAM and check it.
//
//*****************************************************************************
void
MRAMSequenceRetrieve(uint8_t currentCycle, uint32_t chip_number)
{
    // Retrieve the selected sequence from MRAM based on current cycle and check for bit flips
    //
    // Chip number value 8-11 for MRAM
    // Current cycle 0 for all 0s, 1 for all 1s (255 each byte)

    // Variables and necessary ports
    //
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    char str[12];

    // Chip port
    //uint32_t chip_port = RetrieveChipPort(chip_number);

    // SPI port
    //if(chip_port == CS0_PORT) {
    SPI_base = SPI0_NUM_BASE;
    //} else {
    //    SPI_base = SPI1_NUM_BASE;
    //}

    // Random chip selection for setting CS high
    // MRAM is always 8 to 11, so subtracting 7 is fine.
    chip_number_alt = chip_number - 7;

    //
    // Clear out the FIFO receive buffer
    //
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }


    //
    // Read chip data
    //
    // Set CS low
    SetChipSelect(chip_number);

    // Transmit the read command
    SSIDataPut(SPI_base, MRAM_READ);

    // Since this is sequential read, only need to send address zero
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);

    // Wait for the transaction to complete
    while(SSIBusy(SPI_base))
    {
    }

    // Clear out the empty received data from the instruction transmission
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);
    SSIDataGet(SPI_base, &temp);

    // Declare variables for the loop
    uint64_t byte_num = 0;
    uint32_t data;

    // Loop through all data
    for(byte_num = 0; byte_num < MRAM_SIZE_BYTES; byte_num++){

        // Transmit 0, to create clock pulses
        SSIDataPut(SPI_base, 0x0);

        // Wait for the transaction to complete before moving on to the next byte
        while(SSIBusy(SPI_base))
        {
        }

        // Read in the data
        SSIDataGet(SPI_base, &data);

        sprintf(str, "%d ", data);
        UARTDebugSend((uint8_t*) str, strlen(str));
    }

    // Bring CS high again, terminating read
    SetChipSelect(chip_number_alt);
    sprintf(str, "\n");
    UARTDebugSend((uint8_t*) str, strlen(str));


}
