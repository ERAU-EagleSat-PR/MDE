/*
 *  mde_timer.c
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

// Hardware and Driver files
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
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

#include "source/mde_timer.h"

/*
*******************************************************************************
*                               Functions and Stuff                           *
*******************************************************************************
*/
