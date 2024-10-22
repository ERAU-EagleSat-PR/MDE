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

// Watchdogs
void WatchdogsEnable(void);
void ChipWatchdogPoke(void);
void MDEWatchdogPoke(void);
void ChipWatchdogInt(void);
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

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME 90 //minutes. nearly the max value timer can wait.
#define MEMORY_CYCLE_COUNT 22 //  0 inclusive. Amount of times last timer will trigger before an MDE cycle begins. 90/4 = 22.5 -> 22

// Variables for the watchdog timers.
#define MDE_WD_SYSCTL SYSCTL_PERIPH_WDOG0
#define CHIP_WD_SYSCTL SYSCTL_PERIPH_WDOG1
#define MDE_WD_BASE WATCHDOG0_BASE
#define CHIP_WD_BASE WATCHDOG1_BASE

// VVV these values need to be pre-calculated with the clock speed to avoid
//     potential issues.
#define MDE_WD_TIME 180 // minutes ( currently 1.5x the total cycle length. need to examine this )
#define CHIP_WD_TIME 4 // 4 minutes. It is so long since each individual flash chip takes ~3 minutes to complete an operation,
                       //and we don't want to poke during it in case it gets stuck.
// NOTE: chips will run on the PIOSC clock, not system clock as it is WDT1. reduced accuracy, give it leeway


// Variables for the timer
extern uint32_t wd_chip_time;
extern uint32_t wd_mde_time;
extern uint32_t timer_current_cycle;
extern uint32_t cycle_time_clockrate;
extern bool reading_chip;

// The target clock speed for the system clock.
// 16 MHz
#define SYS_CLK_SPEED 16000000

//TODO : Timer hardware, ports, pins, and interrupt handling.


#endif /* SOURCE_MDE_TIMERS_H_ */
