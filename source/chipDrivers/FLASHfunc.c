//*****************************************************************************
//
// This file contains the functions for the S25FL256LA Flash
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
#include "FLASHfunc.h"
#include "source/multiplexer.h"
#include "source/mde.h"
#include "source/UART0_func.h"
#include "source/bit_errors.h"
#include "source/chip_health.h"

//*****************************************************************************
//
// Retrieve and check identification information from FLASH
//
//*****************************************************************************
FLASHID
FlashStatusRead(uint8_t chip_number)
{
    uint32_t data_buffer;
    uint8_t chip_number_alt = chip_number + 7;
    uint32_t SPI_base;
    FLASHID infoFlash;
    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    SetChipSelect(chip_number_alt); //CS high
    SetChipSelect(chip_number); //CS low

    SSIDataPut(SPI_base, FLASH_IDENT); //RDID command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }
    SysCtlDelay(10);
    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete clock pulses
    {
    }
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFlash.cypID = data_buffer;

    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete clock pulses
    {
    }
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFlash.prodID1 = data_buffer;

    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete clock pulses
    {
    }
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFlash.prodID2 = data_buffer;

    // CS high
    SetChipSelect(chip_number_alt);

    // CS low
    SetChipSelect(chip_number);

    SSIDataPut(SPI_base, FLASH_RDSR); //Read Status Register command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }
    SysCtlDelay(10);
    SSIDataPut(SPI_base,0x00); // Clocks
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }
    SSIDataGetNonBlocking(SPI_base, &data_buffer);
    infoFlash.RDSR = data_buffer;

    // CS high
    SetChipSelect(chip_number_alt);

    // CS low
    SetChipSelect(chip_number);

    // Return struct
    return infoFlash;
}


//*****************************************************************************
//
// Prepare the flash configuration register for 4 byte operation
//
//*****************************************************************************

void
FlashConfiguration(uint8_t chip_number)
{
    uint32_t volatile_reg_1;
    uint32_t volatile_reg_2;
    uint32_t status_reg;
    uint32_t desired_data;
    uint8_t chip_number_alt = chip_number + 7;
    uint32_t SPI_base;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    //
    // First, read the first and second register so we can edit the second one. The easiest way to write them is sequentially like this.
    //
    SetChipSelect(chip_number_alt); //CS high

    // STATUS REGISTER 1
    SetChipSelect(chip_number); //CS low

    SSIDataPut(SPI_base, FLASH_RDSR); //Config Reg 1 Read command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Read register
    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete clock pulses
    {
    }
    SSIDataGet(SPI_base, &status_reg);

    SetChipSelect(chip_number_alt); //CS high

    // REGISTER 1
    SetChipSelect(chip_number); //CS low

    SSIDataPut(SPI_base, FLASH_CR1VR); //Config Reg 1 Read command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Read register
    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete clock pulses
    {
    }
    SSIDataGet(SPI_base, &volatile_reg_1);

    SetChipSelect(chip_number_alt); //CS high

    // REGISTER 2
    SetChipSelect(chip_number); //CS low

    SSIDataPut(SPI_base, FLASH_CR2VR); //Config Reg 2 Read command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Read register
    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete clock pulses
    {
    }
    SSIDataGet(SPI_base, &volatile_reg_2);

    SetChipSelect(chip_number_alt); //CS high

    // Desired state of the status register
    desired_data = volatile_reg_2 | FLASH_CONFIG;

    // Begin write cycle with WRENV command

    SetChipSelect(chip_number); //CS low

    // WREN command to write to non-volatile registers
    SSIDataPut(SPI_base,FLASH_WRENV);
    while(SSIBusy(SPI_base)){}

    SetChipSelect(chip_number_alt); //CS high

    // Write registers until Config 2 is completed
    SetChipSelect(chip_number); //CS low

    SSIDataPut(SPI_base,FLASH_WRR); // Write registers command
    while(SSIBusy(SPI_base)){}

    SSIDataPut(SPI_base,status_reg);
    while(SSIBusy(SPI_base)){}

    SSIDataPut(SPI_base,volatile_reg_1);
    while(SSIBusy(SPI_base)){}

    SSIDataPut(SPI_base,desired_data);
    while(SSIBusy(SPI_base)){}

    SetChipSelect(chip_number_alt); //CS high ending write
    // WRENV latch is automatically undone

    // REGISTER 2
    SetChipSelect(chip_number); //CS low

    SSIDataPut(SPI_base, FLASH_CR2VR); //Config Reg 2 Read command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Read register
    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete clock pulses
    {
    }
    SSIDataGet(SPI_base, &volatile_reg_2);

    SetChipSelect(chip_number_alt); //CS high


    // Disable writing
    SetChipSelect(chip_number); //CS low

    // WREN command to write to non-volatile registers
    SSIDataPut(SPI_base,FLASH_WRITE_DISABLE);
    while(SSIBusy(SPI_base)){}

    SetChipSelect(chip_number_alt); //CS high
}

//*****************************************************************************
//
// Erase the flash (resetting it to all 1s)
//
//*****************************************************************************

void
FlashErase(uint8_t chip_number)
{
    // Erases all currently stored data from flash, resetting it to it's default state (all 1s in memory)
    // Chip number is a value 0 - 3 for which flash chip you wish to erase
    // Will need a port variable for which board to erase eventually

    // Function Variables
    uint8_t chip_number_alt = chip_number + 7;
    uint32_t status_register;
    uint32_t SPI_base;
    uint32_t temp;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }

    // CS high, just in case
    SetChipSelect(chip_number_alt);

    // CS low
    SetChipSelect(chip_number);

    // Enable writing to the memory to erase chip
    SSIDataPut(SPI_base, FLASH_WRITE_ENABLE);
    while(SSIBusy(SPI_base))
    {
    }
    // CS high
    SetChipSelect(chip_number_alt);

    // CS low
    SetChipSelect(chip_number);

    // Issue erase command
    SSIDataPut(SPI_base, FLASH_CHIP_ERASE);
    while(SSIBusy(SPI_base))
    {
    }

    // CS high
    SetChipSelect(chip_number_alt);

    // Wait for the Flash to erase before continuing
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // CS low
    SetChipSelect(chip_number);

    //Transmit the Write Disable Command
    SSIDataPut(SPI_base, FLASH_WRITE_DISABLE);
    while(SSIBusy(SPI_base))
    {
    }

    // CS high
    SetChipSelect(chip_number_alt);

    // CS low
    SetChipSelect(chip_number);

    SSIDataPut(SPI_base, FLASH_RDSR); //Read Status Register command
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }

    // Clear FIFO from command
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }
    SSIDataPut(SPI_base,0x00);
    while(SSIBusy(SPI_base)) //Wait to complete command
    {
    }
    SSIDataGetNonBlocking(SPI_base, &status_register);

    //ChipWatchdogPoke(); // Poke the chip watchdog before waiting for the flash erase to complete.

    // No need to wait for chip erase to complete; it will either be done by our next cycle, or not. Health check will flag a FLASH chip with WIP bit on.

    // CS high
    SetChipSelect(chip_number_alt);
    //ChipWatchdogPoke(); // Erase success; poke watchdog a final time.
}

//*****************************************************************************
//
// Transmit to the flash depending on selected cycle
//
//*****************************************************************************

void
FlashSequenceTransmit(uint8_t current_cycle, uint8_t chip_number)
{
    // Write to entire flash memory array
    // Chip number 0-3 for Flash
    // Cycle 0 for all 0s, 1 for all 1s (255 each byte)

    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
    uint32_t status_register;
    uint32_t temp;
    //uint32_t chip_port = RetrieveChipPort(chip_number);

    // Random chip selection for setting CS high; Flash memory are always 0-3, so any value out of that range will work here. Value incremented by 7 to reduce repeated use of same chip select lines.
    chip_number_alt = chip_number + 7;

    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }
    //
    //  Write to the Flash
    //

    // We only need to erase the chip if we are writing all 1s, and we only need to perform a Page Program if we are writing all 0s.
    if (current_cycle == 255)
    {
        // This erase function counts as writing all 1s to memory
        // DEBUG NOTE: error seeding is impossible when writing 1s to flash because of this.
        FlashErase(chip_number);
    }
    else if (current_cycle == 0)
    {
        //
        // Perform a write cycle
        //

        // Sequence and loop variables
        uint32_t byte_num = 0;
        uint32_t current_address = 0;
        uint8_t data = 0;
        // Loop and Write to each page of flash memory
        for(current_address = 0; current_address < FLASH_SIZE_BYTES; current_address += FLASH_PAGE_SIZE)
        {
            // CS low
            SetChipSelect(chip_number);

            // Enable writing to the memory (must be done for every page)
            SSIDataPut(SPI_base, FLASH_WRITE_ENABLE);
            while(SSIBusy(SPI_base))
            {
            }
            // CS high
            SetChipSelect(chip_number_alt);

            // CS low
            SetChipSelect(chip_number);

            // Send page write command
            SSIDataPut(SPI_base, FLASH_WRITE);

            // We need to send the first address of the current 256 byte block
            SSIDataPut(SPI_base, ((current_address >> 24)&0xFF));
            SSIDataPut(SPI_base, ((current_address >> 16)&0xFF));
            SSIDataPut(SPI_base, ((current_address >> 8)&0xFF));
            SSIDataPut(SPI_base, (current_address&0xFF));

            /*SSIDataPut(SPI_base, (current_address >> 24)&0xFF);
                SSIDataPut(SPI_base, (current_address >> 16)&0xFF);
                SSIDataPut(SPI_base, (current_address >> 8)&0xFF);
                SSIDataPut(SPI_base, (current_address)&0xFF);*/


            // Wait for the transmission to complete
            while(SSIBusy(SPI_base))
            {
            }

            // Write to each byte of current page sequentially
            for(byte_num = 0; byte_num < FLASH_PAGE_SIZE; byte_num++)
            {
                // Exit out if we've exceeded the maximum address
                if(current_address + byte_num > FLASH_SIZE_BYTES){
                    break;
                }
                data = current_cycle;

#ifdef DEBUG // Error seeding if in debug mode
                if(seedErrors == 1 && ((current_address + byte_num) % SEEDERRORS_ADDRESS) == 0)
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

            // Wait for write cycle to complete before moving on to next page
            // CS low
            SetChipSelect(chip_number);

            SSIDataPut(SPI_base, FLASH_RDSR); //Read Status Register command
            while(SSIBusy(SPI_base)) //Wait to complete command
            {
            }

            // Clear FIFO from command
            while(SSIDataGetNonBlocking(SPI_base, &temp))
            {
            }
            uint32_t stuckCount = 0;
            status_register = 0x01;
            // Loop until status register says cycle completed
            while(status_register & 0x01)
            {
                SSIDataPut(SPI_base,0x00); // Clock cycles
                while(SSIBusy(SPI_base)) //Wait to complete command
                {}
                SSIDataGet(SPI_base, &status_register);
                SysCtlDelay(1600); // Small delay to prevent mis-reads
                stuckCount++;
                if(stuckCount>10000)
                    break;
            }
            // CS high
            SetChipSelect(chip_number_alt);
            // Move to next page;
            if(stuckCount > 10000) // Failed to write to chip
            {
                chip_health_array[chip_number].HealthCount++;
                break;
            }
        }
        // CS low
        SetChipSelect(chip_number);

        //Transmit the Write Disable Command
        SSIDataPut(SPI_base, FLASH_WRITE_DISABLE);
        while(SSIBusy(SPI_base))
        {
        }
        // CS high
        SetChipSelect(chip_number_alt);
    }
}

//*****************************************************************************
//
// Retrieve the sequence from the Flash and send it to be checked for errors.
//
//*****************************************************************************
void
FlashSequenceRetrieve(uint8_t current_cycle, uint8_t chip_number)
{
    // retrieve from entire flash memory, send each byte to be error checked
    // currently has uart printing for observing integrated

    // chip number 0-3 for flash chips
    // cycle 0 to check for all 0s, cycle 1 to check for all 1s (255 each byte)

    // Necessary Variables
    uint8_t chip_number_alt;
    uint32_t SPI_base;
#ifdef DEBUG
    char buf[10];
    uint8_t bufSize = 10;
#endif
    //uint32_t chip_port = RetrieveChipPort(chip_number);
    // Random chip selection for setting CS high
    // TODO: remove these checks they are redundant since all chips have guaranteed range to be in
    if(chip_number <= 7) {
        chip_number_alt = chip_number + 7;
    } else {
        chip_number_alt = chip_number - 7;
    }
    // SPI port
    if(chip_number < 16) {
        SPI_base = SPI0_NUM_BASE;
    } else {
        SPI_base = SPI1_NUM_BASE;
    }
    //
    // Clear out the FIFO receive buffer
    //
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {
    }

    // Declare variables for the loop
    uint32_t data = 0;
    uint32_t byte_num = 0;
    //
    // Read from the Flash
    //

    // CS low
    SetChipSelect(chip_number);

    // Transmit the read command
    SSIDataPut(SPI_base, FLASH_READ);

    // Send address 0 for sequential read.
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);
    SSIDataPut(SPI_base, 0x0);

    // Wait for the transmission to complete
    while(SSIBusy(SPI_base))
    {
    }

    // Clear out the empty received data from the instruction transmission
    while(SSIDataGetNonBlocking(SPI_base, &temp))
    {}

    for(byte_num = 0; byte_num < FLASH_SIZE_BYTES; byte_num++){

        // Transmit 0, to create clock pulses
        SSIDataPut(SPI_base, 0x0);

        // Wait for the transaction to complete before moving on
        while(SSIBusy(SPI_base))
        {}
        SysCtlDelay(10); // Slight delay to ensure command is processed first
        // Read in the data
        SSIDataGetNonBlocking(SPI_base, &data); // Nonblocking to prevent stuck conditions

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

    // CS high, terminating read
    SetChipSelect(chip_number_alt);
}

