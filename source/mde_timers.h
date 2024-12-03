/*
 *  mde_timer.h
 *
 *  The various timers for the MDE chips
 *
 *  Created on: Dec 30, 2022
 *      Author: Tyler
 */

#ifndef SOURCE_MDE_TIMERS_H_
#define SOURCE_MDE_TIMERS_H_


// The target clock speed for the system clock.
// 16 MHz
#define SYS_CLK_SPEED 16000000
/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/
// MDE timer
void MDETimerConfigure(void);
void MDETimerEnable(void);
void MDETimerDisable(void);
void MDETimerInt(void);

// Chip timer
void ChipTimerConfigure(void);
void ChipTimerEnable(void);
void ChipTimerDisable(void);
void ChipTimerInt(void);

// Watchdogs
void MDEWatchdogEnable(void);
void MDEWatchdogInt(void);
/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/

// MDE General Cycle Timer
#define MDE_TIMER_CTL SYSCTL_PERIPH_TIMER0
#define MDE_TIMER_BASE TIMER0_BASE
#define MDE_TIMER_INT TIMER_TIMA_TIMEOUT
#define MINUTE 60

// Chip timeout timer
#define CHIP_TIMER_CTL SYSCTL_PERIPH_TIMER1
#define CHIP_TIMER_BASE TIMER1_BASE
#define CHIP_TIMER_INT TIMER_TIMA_TIMEOUT

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME 90 //minutes. nearly the max value timer can wait.

// Variables for the watchdog timers.
#define MDE_WD_SYSCTL SYSCTL_PERIPH_WDOG0
#define MDE_WD_BASE WATCHDOG0_BASE


// Number of cycles to load for the MDE watchdog
// setting it to SYS_CLK_SPEED makes the watchdog run once every second
#define MDE_WD_CYCLES ((uint32_t)SYS_CLK_SPEED)


// Number of cycles to load for the timers
#define MDE_TIMER_CYCLES ((uint32_t)(SYS_CLK_SPEED * MINUTE))


// Variables for the timer
extern uint32_t timer_current_cycle;
extern uint16_t chip_timer_triggers;



//TODO : Timer hardware, ports, pins, and interrupt handling.


#endif /* SOURCE_MDE_TIMERS_H_ */
