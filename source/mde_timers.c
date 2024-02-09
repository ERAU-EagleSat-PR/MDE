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
#include "source/UART0_func.h"

/*
*******************************************************************************
*                               Timer Functions                               *
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
    char buf [20];
    uint8_t bufSize = 20;
    // Clear timer interrupt
    TimerIntClear(MDE_TIMER_BASE, MDE_TIMER_INT);

    if(timer_current_cycle >= MEMORY_CYCLE_COUNT)
    {
        // temp for testing
        snprintf(buf,bufSize, "Timer Trigger\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));

        // Call the function to begin a new cycle.
        current_chip = 0;
        MDEProcessCycle();
        timer_current_cycle = 0;
    }
    else
        // temp for testing
        snprintf(buf,bufSize, "Timer Tick\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));

        timer_current_cycle++;

}

/*
*******************************************************************************
*                              Watchdog Functions                             *
*******************************************************************************
*/

void
WatchdogsEnable(void)
{
    // Enables and configures both watchdogs.


    // For MDE WD, first reset do nothing
    // Second reset send OBC request to affect reset line.

    // Define the global variable
    reading_chip = 0;

    // Enable WD peripherals
    SysCtlPeripheralEnable(MDE_WD_SYSCTL);
    SysCtlPeripheralEnable(CHIP_WD_SYSCTL);
    // Wait for WDs to enable
    while(!SysCtlPeripheralReady(MDE_WD_SYSCTL) && !SysCtlPeripheralReady(CHIP_WD_SYSCTL))
    {}

    // Disable resetting for the chip timer.
    // Perhaps it will later be disabled for the MDE timer; not sure how resetting will work on the sat
    WatchdogResetDisable(MDE_WD_BASE); //TODO
    WatchdogResetDisable(CHIP_WD_BASE);

    // Register the interrupt handlers.
    WatchdogIntRegister(MDE_WD_BASE, &MDEWatchdogInt);
    WatchdogIntRegister(CHIP_WD_BASE, &ChipWatchdogInt);

    // Set both reload values
    WatchdogReloadSet(MDE_WD_BASE, wd_mde_time);
    WatchdogReloadSet(CHIP_WD_BASE, wd_chip_time);

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
    WatchdogReloadSet(CHIP_WD_BASE, wd_chip_time);
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
    WatchdogReloadSet(MDE_WD_BASE, wd_mde_time);
}

void
MDEWatchdogInt(void)
{
    // The interrupt handler for the MDE watchdog. Will attempt to kickstart the program or reset it if need be. This logic should be discussed.

    // Clear the interrupt flag
    WatchdogIntClear(MDE_WD_BASE);
#ifdef DEBUG
    char buf [30];
    uint8_t bufSize = 30;
    snprintf(buf,bufSize, "MDE WATCHDOG\n\r");
    UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
}

void
ChipWatchdogInt(void)
{
    // The interrupt handler for the chips watchdog.
    // If a chip trips this enough times it will be considered
    // dead and marked as such here.

    // Clear the interrupt flag
    WatchdogIntClear(CHIP_WD_BASE);
#ifdef DEBUG
    char buf [30];
    uint8_t bufSize = 30;
    snprintf(buf,bufSize, "CHIPS WATCHDOG\n\r");
    UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
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
