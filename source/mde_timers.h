/*
 *  mde_timer.h
 *
 *  The various timers for the MDE chips
 *
 *  Created on: Dec 30, 2022
 *      Author: Calvin
 */

#ifndef SOURCE_MDE_TIMERS_H_
#define SOURCE_MDE_TIMERS_H_


/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/
void MDETimerEnable(void);
void MDETimerInt(void);
void MDEWatchdogsEnable(void);
void ChipWatchdogPoke(void);
void MDEWatchdogPoke(void);
void MDEWatchdogInt(void);
void ChipWatchdogInt(void);
/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/

// MDE General Cycle Timer
#define MDE_TIMER_CTL SYSCTL_PERIPH_TIMER0
#define MDE_TIMER_BASE TIMER0_BASE
#define MDE_TIMER_INT TIMER_TIMA_TIMEOUT

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME 90 //minutes
#define TIMER_CYCLES 1

// Variables for the watchdog timers.
#define MDE_WD_BASE WATCHDOG0_BASE
#define CHIP_WD_BASE WATCHDOG1_BASE

// VVV these values need to be pre-calculated with the clock speed to avoid
//     potential issues.
#define MDE_WD_TIME 180 // minutes ( currently 1.5x the total cycle length. need to examine this )
#define CHIP_WD_TIME 30 // seconds ( need to time flash operations, they are longest )
// NOTE: chips will run on the PIOSC clock, not system clock as it is WDT1. reduced accuracy, give it leeway


// Variables for the timer
extern uint32_t timer_current_cycle;
extern bool reading_chip;

// The target clock speed for the system clock.
// 16 MHz
#define SYS_CLK_SPEED 16000000

//TODO : Timer hardware, ports, pins, and interrupt handling.


#endif /* SOURCE_MDE_TIMERS_H_ */
