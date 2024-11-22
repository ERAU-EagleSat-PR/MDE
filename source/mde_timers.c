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
#ifdef TIMER_DEBUG
    char buf [20];
    uint8_t bufSize = 20;
#endif

    // Clear timer interrupt
    TimerIntClear(MDE_TIMER_BASE, MDE_TIMER_INT);


#ifdef TIMER_DEBUG
        snprintf(buf,bufSize, "Timer Tick\n\r");
        UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
    ++timer_current_cycle;

}

/*
*******************************************************************************
*                              Watchdog Functions                             *
*******************************************************************************
*/

/**
 * A quick summary of how watchdogs work:
 * Watchdogs start by behaving like normal continuous timers; they count down
 * from some set value until hitting zero, at which point they trigger an
 * interrupt. After the interrupt is triggered, the watchdog timer restarts from
 * its starting value.
 * HOWEVER, if the watchdog hits 0 again before the interrupt is handled, the
 * watchdog can (if configured to do so) assert a reset signal to the system,
 * causing (you guessed it) a system reset.
 *
 * Essentially, this allows MDE to reset itself in case it gets stuck in some
 * fault
 *
 */

void
WatchdogsEnable(void)
{
    // Enables and configures watch dog timer
    // The timer triggers an interrupt that, if not handled,
    // causes a system reset


    // Enable WD peripheral
    SysCtlPeripheralEnable(MDE_WD_SYSCTL);

    // Wait for WDs to enable
    while(!SysCtlPeripheralReady(MDE_WD_SYSCTL))
    {}

    if(WatchdogLockState(MDE_WD_BASE) == true)
    {
        WatchdogUnlock(MDE_WD_BASE);
    }

    // Register the interrupt handlers.
    WatchdogIntRegister(MDE_WD_BASE, &MDEWatchdogInt);

    // Enable Stall
    // This stops the timer when debug breakpoint hit and steps the timer as
    // you step through the program
    WatchdogStallEnable(MDE_WD_BASE);

    // Allow Watchdog to reset system if interrupt isn't handled
    // Literally the whole point of a watchdog
    WatchdogResetEnable(MDE_WD_BASE);

    // Set reload value
    WatchdogReloadSet(MDE_WD_BASE, wd_mde_time);

    // Finally, enable system watchdog
    WatchdogEnable(MDE_WD_BASE);
}

void
MDEWatchdogInt(void)
{
    // The interrupt handler for the MDE watchdog.

    // Clear the interrupt flag
    WatchdogIntClear(MDE_WD_BASE);
#ifdef DEBUG
    char buf [30];
    uint8_t bufSize = 30;
    snprintf(buf,bufSize, "MDE WATCHDOG\n\r");
    UARTDebugSend((uint8_t*) buf, strlen(buf));
#endif
}
