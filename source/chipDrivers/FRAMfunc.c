//*****************************************************************************
//
// This file contains the functions for the MB85RS2MTYPNF FRAM
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
#include "AccessTools.h"

//*****************************************************************************
//
// Retrieve and check identification information from FRAM (no check yet)
//
//*****************************************************************************

void
FRAMStatusRead(uint8_t chip_number)
{
    char buf[30];
    uint32_t data_buffer;
    uint8_t chip_number_alt = chip_number + 7;
    uint32_t SPI_base = SPI0_NUM_BASE;
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

    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    sprintf(buf,"fujitsi id: %d \n\r",data_buffer); //Print to console
    UARTDebugSend((uint8_t*) buf,strlen(buf));

    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    sprintf(buf,"cont code: %d \n\r",data_buffer); //Print to console
    UARTDebugSend((uint8_t*) buf,strlen(buf));

    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    sprintf(buf,"prod ID1: %d \n\r",data_buffer); //Print to console
    UARTDebugSend((uint8_t*) buf,strlen(buf));

    SSIDataPut(SPI_base,0x0);
    while(SSIBusy(SPI_base)) //Wait to complete clock cycle
    {}
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    sprintf(buf,"prod ID2: %d \n\r",data_buffer); //Print to console
    UARTDebugSend((uint8_t*) buf,strlen(buf));

    // CS high
    SetChipSelect(chip_number_alt);
}



//*****************************************************************************
//
// Transmit the current sequence to the FRAM.
//
//*****************************************************************************
void
FRAMSequenceTransmit(uint8_t currentCycle, uint32_t chip_number)
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
    uint32_t data;
    if(currentCycle == 1){
        data = 0b11111111; //255 in binary
    } else {
        data = 0; // == 0b00000000
    }

    // SPI port
    //if(chip_port == CS0_PORT) {
        SPI_base = SPI0_NUM_BASE;
    //} else {
    //    SPI_base = SPI1_NUM_BASE;
    //}

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
FRAMSequenceRetrieve(uint8_t currentCycle, uint32_t chip_number)
{
    // Retrieve data to FRAM depending on current cycle and send it for error checking

    // Chip number 4-7 for FRAM
    // Current cycle 0 for all 0s, 1 for all 1s (255 each byte) to be compared against data

    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    uint32_t temp;
    //uint32_t chip_port = RetrieveChipPort(chip_number);

    // Random chip selection for setting CS high
    // FRAM is always 3-7, so always adding 7 is fine.
    chip_number_alt = chip_number + 7;

    //if(chip_port == CS0_PORT) {
        SPI_base = SPI0_NUM_BASE;
    //} else {
    //    SPI_base = SPI1_NUM_BASE;
    //}


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
    char str[12];
    // Loop through all data
    uint32_t byte_num = 0;
    for(byte_num = 0; byte_num < FRAM_SIZE_BYTES; byte_num++){

        // Transmit 0, to create clock pulses
        SSIDataPut(SPI_base, 0x0);

        // Wait for the transaction to complete before moving on to the next byte
        while(SSIBusy(SPI_base))
        {
        }

        // Read in the data
        SSIDataGet(SPI_base, &data);

        // Send data to be compared and prepared
        //CheckErrors(data, sequence, byte_num, chip_number);
        sprintf(str, "%d ", data);
        UARTDebugSend((uint8_t*) str, strlen(str));
    }

    // Bring CS high, ending read
    SetChipSelect(chip_number_alt);
}
