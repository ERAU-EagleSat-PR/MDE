/*
 * mde.c
 *
 *
 *
 */
//******************************************************************//
//                          Includes                                //
//******************************************************************//
// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware Files
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

// Driver Files
#include <driverlib/debug.h>
#include <driverlib/fpu.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/rom.h>
#include <driverlib/ssi.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>

// Custom Header Files
#include "source/mde.h"
#include "source/chips.h"
#include "source/chip_health.h"

#ifdef DEBUG
#include "devtools.h"
#endif

//******************************************************************//
//                                                                  //
//                        Functions                                 //
//                                                                  //
//******************************************************************//
//                  Enable/Configure SPI                            //
//******************************************************************//

void
MDECycleStart(void)
{
    // Prepare for a new data cycle.

    // Perform a health check on all chips.

    // Prepare new data

    // Begin chip read/write cycle
}

void
ChipCycle(void)
{
    // Loop through all chips.
    // If a watchdog interrupt is triggered, this function should be called again after it is dealt with.

    uint8_t current_chip = auto_chip_number;
    while(auto_chip_number < MAX_CHIP_NUMBER)
    {
        // Is this chip dead? If so, skip it
        if( chip_death_array[current_chip] == 1)
        {
#ifdef DEBUG
            char buf[80];
        sprintf(buf,"Chip %i is marked as dead!\n\r", current_chip);
        UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
        }
        else // Chip is not dead; continue with cycle
        {
            CheckChipHealth(current_chip); // Check chip health. If it fails this cycle, skip for now.
        }
    }

    return;
}

// This should probably go somewhere else.
void
EnableSPI(void)
{
    // Enable necessary peripherals
    SysCtlPeripheralEnable(SPI0_SYS_PORT);
    SysCtlPeripheralEnable(SPI0_SYS_PERIPH);
    SysCtlPeripheralEnable(SPI1_SYS_PORT);
    SysCtlPeripheralEnable(SPI1_SYS_PERIPH);
    // Wait to be ready
    while(!SysCtlPeripheralReady(SPI0_SYS_PORT)     &&
            !SysCtlPeripheralReady(SPI0_SYS_PERIPH) &&
            !SysCtlPeripheralReady(SPI1_SYS_PORT)   &&
            !SysCtlPeripheralReady(SPI1_SYS_PERIPH))
    {
    }
    // Board 1 SPI
    // Configure communication pins
    GPIOPinConfigure(SPI0_CLK);
    GPIOPinConfigure(SPI0_MOSI);
    GPIOPinConfigure(SPI0_MISO);
    GPIOPinTypeSSI(SPI0_PORT, SPI0_CLK_NUM | SPI0_MOSI_NUM | SPI0_MISO_NUM);

    // Configure SPI communication parameters
    SSIConfigSetExpClk(SPI0_NUM_BASE, SYS_CLK_SPEED, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, SPI_CLK_SPEED, 8);
    SSIEnable(SPI0_NUM_BASE);

    // Board 2 SPI
    // Configure communication pins
    GPIOPinConfigure(SPI1_CLK);
    GPIOPinConfigure(SPI1_MOSI);
    GPIOPinConfigure(SPI1_MISO);
    GPIOPinTypeSSI(SPI1_PORT, SPI1_CLK_NUM | SPI1_MOSI_NUM | SPI1_MISO_NUM);

    // Configure SPI communication parameters
    SSIConfigSetExpClk(SPI1_NUM_BASE, SYS_CLK_SPEED, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, SPI_CLK_SPEED, 8);
    SSIEnable(SPI1_NUM_BASE);

    //
    // Clear out the FIFOs
    //
    uint32_t temp;
    while(SSIDataGetNonBlocking(SPI0_NUM_BASE, &temp) &&
            SSIDataGetNonBlocking(SPI1_NUM_BASE, &temp))
    {
    }
}

//******************************************************************//
//                            Main                                  //
//******************************************************************//

/*
void
main(){

 Not sure if your intention was to eventually move main here but just gonna leave a comment for now
}
 */

