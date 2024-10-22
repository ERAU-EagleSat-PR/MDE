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
#include "source/devtools.h"
#include "source/chips.h"
#include "source/mde_timers.h"
#include "source/UART0_func.h"

//******************************************************************//
//                                                                  //
//                        Functions                                 //
//                                                                  //
//******************************************************************//
//                  MDE Process Functions                           //
//******************************************************************//

void
MDEProcessCycle(void)
{
    // Perform a complete cycle of the MDE experiment including
    // health check, data read, error check, and data write.
    // !! Set current_chip global var to 0 before beginning a brand new cycle.
    MDETimerDisable();
#ifdef DEBUG
    char buf[40];
    uint8_t bufSize = 40;

    snprintf(buf,bufSize, "Beginning MDE %u cycle...\n\r", currentCycle);
    UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
    uint8_t i;

    // Enable chips watchdog and give it an initial poke.
    //ChipWatchdogPoke();
    //MDEWatchdogPoke();
    reading_chip = true;

    for(i = current_chip; i < MAX_CHIP_NUMBER; i++)
    {
        //ChipWatchdogPoke(); // Reset watchdog before each chip.
        //MDEWatchdogPoke();

        if(chip_death_array[i] == 0)
        {
            // Perform a health check on all chips, excluding dead ones.
            if(CheckChipHealth(i) == 0)
            {
                // If chip is healthy, immediately get its data.
                ReadFromChip(~currentCycle, i);

                // Write new data to chips
                WriteToChip(currentCycle, i);
#ifdef DEBUG
                snprintf(buf,bufSize, "CHIP %2u OK\n\r", i);
                UARTDebugSend((uint8_t*) buf, strlen(buf));
            } else {
                snprintf(buf,bufSize, "CHIP %2u HEALTH\n\r", i);
                UARTDebugSend((uint8_t*) buf, strlen(buf));
            }
        } else {
            snprintf(buf,bufSize, "CHIP %2u DEAD\n\r", i);
            UARTDebugSend((uint8_t*) buf, strlen(buf));
        }
#else
            }
        }
#endif
        current_chip = i; // Update location tracker
    }

    // Disable chips watchdog
    //ChipWatchdogPoke();
    reading_chip = false;

    // Prepare new data
    if(currentCycle == 0)
        currentCycle = 255;
    else
        currentCycle = 0;
#ifdef DEBUG
    snprintf(buf,bufSize, "MDE Cycle complete.\n\r");
    UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
    MDETimerEnable();
}

//******************************************************************//
//                         SPI enable                               //
//******************************************************************//

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

    // Unlock PF0
    // By default, PF0 is locked for use as a NMI (non maskable interrupt pin)
    // In order to change, this, we have to unlock the pin.
    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

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
//                         Board Power                              //
//******************************************************************//
void BoardPowerInit(void) {
    // Enable Board Power GPIO port
    SysCtlPeripheralEnable(BOARD_POWER_GPIO_SYSCTL_PERIPH);

    // Check for peripheral to be Enabled
    while(!SysCtlPeripheralReady(BOARD_POWER_GPIO_SYSCTL_PERIPH))
    {
    }

    // PD7 is normally locked for usage for complex reasons that we're gonna ignore
    // We have to unlock it to configure it
    // I just grabbed this off the internet
    HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE+GPIO_O_CR) |= GPIO_PIN_7;

    // Set the pins to output 
    GPIOPinTypeGPIOOutput(BOARD_POWER_GPIO_BASE, BOARD_POWER_BOARD_1_PIN | BOARD_POWER_BOARD_2_PIN);

    // Initialize the pin values to being 0 i.e. the boards aren't powered
    GPIOPinWrite(BOARD_POWER_GPIO_BASE, BOARD_POWER_BOARD_1_PIN | BOARD_POWER_BOARD_2_PIN, 0);
}

void Board1PowerOn(void) {
    GPIOPinWrite(BOARD_POWER_GPIO_BASE, BOARD_POWER_BOARD_1_PIN, BOARD_POWER_BOARD_1_PIN);
}

void Board1PowerOff(void) {
    GPIOPinWrite(BOARD_POWER_GPIO_BASE, BOARD_POWER_BOARD_1_PIN, 0);
}

void Board2PowerOn(void) {
    GPIOPinWrite(BOARD_POWER_GPIO_BASE, BOARD_POWER_BOARD_2_PIN, BOARD_POWER_BOARD_2_PIN);
}

void Board2PowerOff(void) {
    GPIOPinWrite(BOARD_POWER_GPIO_BASE, BOARD_POWER_BOARD_2_PIN, 0);
}
