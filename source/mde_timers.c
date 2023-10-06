/*
 *  mde_timers.c
 *
 *  Created on: Dec 30, 2022
 *      Author: Calvin
 */

/*
*******************************************************************************
*                                   Includes                                  *
*******************************************************************************
*/

// Standard C files
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Hardware Files
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"

// Driver Files
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/ssi.h"
#include "driverlib/watchdog.h"

// Customer Includes
#include "source/mde_timers.h"
#include "source/mde.h"
#include "source/chips.h"
#include "source/chip_health.h"
#include "source/devtools.h"

/*
*******************************************************************************
*                               Functions and Stuff                           *
*******************************************************************************
*/

void
MDETimerConfigure(void)
{
    // Configure the general MDE cycle timer. Does not enable timer.

    // Enable peripheral
    SysCtlPeripheralEnable(MDE_TIMER_CTL);
    // Wait
    while(!SysCtlPeripheralReady(MDE_TIMER_CTL))
    {}

    // Configure as a periodic timer
    TimerConfigure(MDE_TIMER_BASE, TIMER_CFG_PERIODIC);

    // Enable timeout interrupts on the A timer
    TimerIntRegister(MDE_TIMER_BASE, TIMER_A, &MDETimerInt);
    TimerIntEnable(MDE_TIMER_BASE, MDE_TIMER_INT);
}

void MDETimerEnable(void)
{
    // Reset the timer's load and enable it if needed.

    TimerLoadSet(MDE_TIMER_BASE, TIMER_A, cycle_time_clockrate);
    TimerEnable(MDE_TIMER_BASE, TIMER_A);
}

void MDETimerDisable(void)
{
    // Disable the timer.

    TimerDisable(MDE_TIMER_BASE, TIMER_A);
}
void
MDETimerInt(void)
{
    // Clear timer interrupt
    TimerIntClear(MDE_TIMER_BASE, MDE_TIMER_INT);

    // Call the function to begin a new cycle.
    current_chip = 0;
    //MDEProcessCycle();
#ifdef DEBUG
        char str[12];
        sprintf(str, "Beep\r\n");
        UARTDebugSend((uint8_t*) str, strlen(str));
#endif

}

void
MDEWatchdogsEnable(void)
{
    // Enables and configures both watchdogs.


    // For MDE WD, first reset do nothing
    // Second reset send OBC request to affect reset line.

    // Define the global variable
    reading_chip = 0;

    // Enable WD peripherals
    SysCtlPeripheralEnable(MDE_WD_BASE);
    SysCtlPeripheralEnable(CHIP_WD_BASE);
    // Wait for WDs to enable
    while(!SysCtlPeripheralReady(MDE_WD_BASE) && !SysCtlPeripheralReady(CHIP_WD_BASE))
    {}

    // Disable resetting for the chip timer.
    // Perhaps it will later be disabled for the MDE timer; not sure how resetting will work on the sat
    WatchdogResetDisable(CHIP_WD_BASE);

    // Register the interrupt handlers.
    WatchdogIntRegister(MDE_WD_BASE, &MDEWatchdogInt);
    WatchdogIntRegister(CHIP_WD_BASE, &ChipWatchdogInt);

    // Set both reload values
    WatchdogReloadSet(MDE_WD_BASE, MDE_WD_TIME);
    WatchdogReloadSet(CHIP_WD_BASE, CHIP_WD_TIME);

    // Finally, enable both watch dogs.
    // Enables both count downs and interrupts.
    WatchdogEnable(MDE_WD_BASE);
    WatchdogEnable(CHIP_WD_BASE);
}

void
ChipWatchdogPoke(void)
{
    // The Watchdog for all chip operations, such as reading or writing from memory or registers.
    // This function resets the timer and clears the interrupt flag.

    if(WatchdogLockState(CHIP_WD_BASE))
    {   // Check to unlock the watchdog, just in case.
        WatchdogUnlock(CHIP_WD_BASE);
    }

    // Clear interrupt just in case.
    WatchdogIntClear(CHIP_WD_BASE);
    // Set the chip watchdog timer to default value of ~~~ seconds.
    WatchdogReloadSet(CHIP_WD_BASE, CHIP_WD_TIME);
}

void
MDEWatchdogPoke(void)
{
    // Poke the watchdog timer periodically to prevent it from resetting the program.
    // This function resets the timer and clears the interrupt flag.
    // If the interrupt flag is not cleared, the timer will currently restart the program.

    if(WatchdogLockState(MDE_WD_BASE))
    {   // Check to unlock the watchdog, just in case.
        WatchdogUnlock(MDE_WD_BASE);
    }

    // Clear interrupt just in case.
    WatchdogIntClear(MDE_WD_BASE);
    // Set the chip watchdog timer to default value of ~~~ seconds.
    WatchdogReloadSet(MDE_WD_BASE, MDE_WD_TIME);
}

void
MDEWatchdogInt(void)
{
    // The interrupt handler for the MDE watchdog. Will attempt to kickstart the program or reset it if need be. This logic should be discussed.

    // Clear the interrupt flag
    WatchdogIntClear(MDE_WD_BASE);
}

void
ChipWatchdogInt(void)
{
    // The interrupt handler for the chips watchdog.
    // If a chip trips this enough times it will be considered
    // dead and marked as such here.

    // Clear the interrupt flag
    WatchdogIntClear(CHIP_WD_BASE);

    // Watchdogs can't be disabled; if a chip is not being operated on, simply exit the interrupt.
    // This watchdog will be disabled in sleep mode; the other will still function
    if(!reading_chip)
    {
        return;
    }

    // If a chip is currently being worked on, update the health array with which one it was.
    chip_health_array[current_chip].WatchdogCount = chip_health_array[current_chip].WatchdogCount + 1;

    // If the chip has exceeded max watchdog triggers, mark it dead so it is skipped next cycle.
    if(chip_health_array[current_chip].WatchdogCount > CHIP_WD_MAX)
    {
        chip_death_array[current_chip] = 1;
    }

}
