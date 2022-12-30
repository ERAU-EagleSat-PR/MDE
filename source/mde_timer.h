/*
 *  mde_timer.h
 *
 *  The 90 minute timer for the MDE Experiemtn
 *
 *  Created on: Dec 30, 2022
 *      Author: Calvin
 */

#ifndef SOURCE_MDE_TIMER_H_
#define SOURCE_MDE_TIMER_H_


/*
*******************************************************************************
*                             Function Prototypes                             *
*******************************************************************************
*/



/*
*******************************************************************************
*                                  Constants                                  *
*******************************************************************************
*/

// Cycle time is the time it takes for a timer interrupt to trigger in seconds
// Timer cycles is how many cycles the timer will do before performing a check
#define MEMORY_CYCLE_TIME 90 //IN
#define TIMER_CYCLES 1//10

// Variables for the timer
extern uint32_t timer_current_cycle;

// The target clock speed for the system clock.
// 16 MHz
#define SYS_CLK_SPEED 16000000

//TODO : Timer hardware, ports, pins, and interrupt handling.


#endif /* SOURCE_MDE_TIMER_H_ */
