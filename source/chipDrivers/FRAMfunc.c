//*****************************************************************************
//
// This file contains the functions for the MB85RS16N FRAM
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
#include "FRAMfunc.h"
#include "source/multiplexer.h"
#include "source/mde.h"
#include "source/UART0_func.h"
#include "source/bit_errors.h"

//*****************************************************************************
//
// Retrieve and return the FRAM identification information.
//
//*****************************************************************************

FRAMID
FRAMStatusRead(uint8_t chip_number)
{
    uint32_t data_buffer;
    uint8_t chip_number_alt = chip_number + 7;
    uint32_t SPI_base;
    FRAMID infoFRAM;
    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    SetChipSelect(chip_number_alt); // CS high
    SetChipSelect(chip_number); //CS low

    SSIDataPut(SPI_base, FRAM_IDENT); //RDID command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command returns
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }
    SysCtlDelay(10);
    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFRAM.fujID = data_buffer;

    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFRAM.contCode = data_buffer;

    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFRAM.prodID1 = data_buffer;

    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFRAM.prodID2 = data_buffer;

    // CS high
    SetChipSelect(chip_number_alt);

    return infoFRAM;
}

//*****************************************************************************
//
// Transmit the current sequence to the FRAM.
//
//*****************************************************************************
void
FRAMSequenceTransmit(uint8_t current_cycle, uint32_t chip_number)
{
    // transmit data to fram depending on current cycle

    // chip number 4-7 for fram
    // current cycle 0 for all 0s, 1 for all 1s (255 each byte)

    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    //uint32_t chip_port = RetrieveChipPort(chip_number);
    // Random chip selection for setting CS high
    // FRAM is always 3-7, so always adding 7 is fine.
    chip_number_alt = chip_number + 7;

    // Set data to write depending on current cycle of all 1s or all 0s
    uint32_t data = current_cycle;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    //
    // Write to the FRAM.
    //

    // CS low
    SetChipSelect(chip_number);

    // Transmit the write enable
    SSIDataPut(SPI_base, FRAM_WRITE_ENABLE);
    while(SSIBusy(SPI_base))
    {
    }

    // CS high
    SetChipSelect(chip_number_alt);
    // CS low
    SetChipSelect(chip_number);

    // Transmit the write instruction
    SSIDataPut(SPI_base, FRAM_WRITE);

    // Since this is sequential write, only need to send address zero
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);

    // Declare variables for the loop


    uint32_t byte_num = 0;
    for(byte_num = 0; byte_num < FRAM_SIZE_BYTES; byte_num++){
        data = current_cycle;

        // Seeded errors if in debug mode
#ifdef DEBUG
        if(seedErrors == 1 && (byte_num % SEEDERRORS_ADDRESS) == 0)
            data = SEEDERRORS_VALUE;
#endif
        // Begin transmitting data
        SSIDataPut(SPI_base, data);

        // Wait for the transmission to complete before moving on to the next byte
        while(SSIBusy(SPI_base))
        {
        }
    }
    // CS high
    SetChipSelect(chip_number_alt);

    // CS low to re-enable write lock
    SetChipSelect(chip_number);

    // Transmit the WRDI command
    SSIDataPut(SPI_base, FRAM_WRITE_DISABLE);

    // CS high
    SetChipSelect(chip_number_alt);
}

//*****************************************************************************
//
// Retrieve the sequence from the FRAM and check it.
//
//*****************************************************************************
void
FRAMSequenceRetrieve(uint8_t current_cycle, uint32_t chip_number)
{
    // Retrieve data to FRAM depending on current cycle and send it for error checking

    // Chip number 4-7 for FRAM
    // Current cycle 0 for all 0s, 1 for all 1s (255 each byte) to be compared against data

    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    uint32_t temp;
    //uint32_t chip_port = RetrieveChipPort(chip_number);

#ifdef DEBUG
    char buf[10];
    uint8_t bufSize = 10;
#endif

    // Random chip selection for setting CS high
    // FRAM is always 3-7, so always adding 7 is fine.
    chip_number_alt = chip_number + 7;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }


    //
    // Read from the FRAM
    //

    // Set CS low
    SetChipSelect(chip_number);

    // Transmit the read command
    SSIDataPut(SPI_base, FRAM_READ);

    // Since this is sequential read, only need to send address zero
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);

    // Wait for the transaction to complete
    while(SSIBusy(SPI_base))
    {
    }

    // Clear out the empty recieved data from the instruction transmission
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Declare variables for the loop
    uint32_t data = 0;
    // Loop through all data
    uint32_t byte_num = 0;
    for(byte_num = 0; byte_num < FRAM_SIZE_BYTES; byte_num++){

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
